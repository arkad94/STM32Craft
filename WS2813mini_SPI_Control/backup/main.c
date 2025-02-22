/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body with WS2812B/WS2813 LED Control
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "gpio.h"
#include <math.h>  // For pow() function

/******************************************************************************
 *  WS2812B/WS2813 “pulse width” defines in timer ticks
 *
 *  Adjust these based on your timer frequency.
 *  For example, if each timer tick is ~93.75 ns (10.67 MHz):
 *    - 3 ticks ~ 281 ns (T0H)
 *    - 7 ticks ~ 656 ns (T1H)
 *  This can be enough to remain in WS2813's valid range.
 *
 *  Make sure your total bit period (ARR+1 in the timer) is enough to
 *  accommodate T1H + T1L (usually ~1.2–1.4 µs total).
 *****************************************************************************/
#define WS2813_T0H  3  // ~300 ns High for ‘0’ bit, example
#define WS2813_T1H  10  // ~937.5 ns High for ‘1’ bit, calculated

/* Gamma correction function */
uint8_t gammaCorrect(uint8_t value) {
    float gamma = 2.2;  // Standard gamma value
    return (uint8_t)(pow((float)value / 255.0, gamma) * 255.0);
}

/* Private defines */
#define NUM_LEDS 10
#define LED_BUFFER_SIZE (NUM_LEDS * 24 + 200)  // 24 bits per LED + reset pulse

/* LED Data Buffers */
static uint16_t LEDBuffer[LED_BUFFER_SIZE] = {0};
static uint8_t LED_Data[NUM_LEDS * 3] = {0};  // GRB color data

/* Private function prototypes */
void WS2812_SetColor(uint8_t led, uint8_t green, uint8_t red, uint8_t blue);
void WS2812_Refresh(void);
void Error_Handler(void);

/**
  * @brief  Set color for a specific LED
  * @param  led: LED index (0-15)
  * @param  green: Green intensity (0-255)
  * @param  red:   Red intensity (0-255)
  * @param  blue:  Blue intensity (0-255)
  */
void WS2812_SetColor(uint8_t led, uint8_t green, uint8_t red, uint8_t blue) {
    if (led >= NUM_LEDS) return;  // Check if the LED index is valid

    uint32_t ledOffset = led * 3;

    // Apply gamma correction to each color component
    LED_Data[ledOffset]     = gammaCorrect(green);  // Corrected Green
    LED_Data[ledOffset + 1] = gammaCorrect(red);    // Corrected Red
    LED_Data[ledOffset + 2] = gammaCorrect(blue);   // Corrected Blue
}

/**
  * @brief  Prepare and send LED data via DMA
  */
void WS2812_Refresh(void) {
    uint16_t pos = 0;

    // Convert LED_Data to PWM signal
    for (int led = 0; led < NUM_LEDS; led++) {
        for (int color = 0; color < 3; color++) {
            uint8_t byte = LED_Data[led * 3 + color];

            // Send each bit (MSB first)
            for (int bit = 7; bit >= 0; bit--) {
                if (byte & (1 << bit)) {
                    // '1' bit => wider pulse
                    LEDBuffer[pos++] = WS2813_T1H;
                } else {
                    // '0' bit => narrower pulse
                    LEDBuffer[pos++] = WS2813_T0H;
                }
            }
        }
    }

    // Add reset pulse (all zeros)
    for (int i = pos; i < LED_BUFFER_SIZE; i++) {
        LEDBuffer[i] = 0;  // Ensure 200+ zero ticks for reset
    }

    // Start DMA transfer using TIM4 Channel 3
    HAL_TIM_PWM_Start_DMA(&htim4, TIM_CHANNEL_3, (uint32_t *)LEDBuffer, LED_BUFFER_SIZE);
}


/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
    /* MCU Configuration--------------------------------------------------------*/
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_DMA_Init();
    MX_TIM4_Init();  // Updated to initialize TIM4 instead of TIM3

    /* Define LED colors */
    uint8_t redShade[3]     = {0x40, 0x00, 0x00};   // Shade of red (GRB format)
    uint8_t currentColor[3] = {0x6B, 0x00, 0x3C};   // Some color (GRB format)
    uint8_t greenShade[3]   = {0x00, 0x40, 0x00};   // Shade of green (GRB format)

    /* Infinite loop */
    while (1)
    {
        // Loop through cycles with different delays
        for (float delay = 1.0; delay >= 0.25; delay /= 2)
        {
            // Loop through all LEDs
            for (int i = 0; i < NUM_LEDS; i++)
            {
                // Set LED color based on position
                if (i % 3 == 0) {
                    WS2812_SetColor(i, redShade[0], redShade[1], redShade[2]);
                } else if (i % 3 == 1) {
                    WS2812_SetColor(i, currentColor[0], currentColor[1], currentColor[2]);
                } else {
                    WS2812_SetColor(i, greenShade[0], greenShade[1], greenShade[2]);
                }

                // Refresh LED strip
                WS2812_Refresh();
                HAL_Delay((int)(delay * 1000)); // Delay in milliseconds
            }

            // Clear all LEDs after a full cycle
            for (int i = 0; i < NUM_LEDS; i++) {
                WS2812_SetColor(i, 0, 0, 0); // Turn off LED
            }
            WS2812_Refresh();
            HAL_Delay((int)(delay * 1000)); // Add a gap before the next cycle
        }
    }
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);

    while(!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY)) {}

    RCC_OscInitStruct.OscillatorType       = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState            = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState        = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType      = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                     |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                     |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource   = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider  = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider  = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK) {
        Error_Handler();
    }
}

/**
  * @brief Error handler
  */
void Error_Handler(void)
{
    // Infinite loop
    while (1)
    {
        // Optionally toggle an LED or use some other mechanism for debugging
    }
}
