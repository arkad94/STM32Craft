/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body with WS2812B LED Control
  ******************************************************************************
  */
/* USER CODE END Header */

#include "main.h"
#include "dma.h"
#include "memorymap.h"
#include "tim.h"
#include "gpio.h"
#include <math.h>  // For pow() function

/* Gamma correction function */
uint8_t gammaCorrect(uint8_t value) {
    float gamma = 2.2;  // Standard gamma value
    return (uint8_t)(pow((float)value / 255.0, gamma) * 255.0);
}

/* Private defines */
#define NUM_LEDS 16
#define LED_BUFFER_SIZE (NUM_LEDS * 24 + 50)  // 24 bits per LED + reset pulse one frame

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
  * @param  red: Red intensity (0-255)
  * @param  blue: Blue intensity (0-255)
  */
void WS2812_SetColor(uint8_t led, uint8_t green, uint8_t red, uint8_t blue) {
    if (led >= NUM_LEDS) return;  // Check if the LED index is valid

    uint32_t ledOffset = led * 3;

    // Apply gamma correction to each color component
    LED_Data[ledOffset] = gammaCorrect(green);  // Corrected Green
    LED_Data[ledOffset + 1] = gammaCorrect(red);  // Corrected Red
    LED_Data[ledOffset + 2] = gammaCorrect(blue);  // Corrected Blue
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
                LEDBuffer[pos++] = (byte & (1 << bit)) ? 56 : 28;
            }
        }
    }

    // Add reset pulse (all zeros)
    for (int i = pos; i < LED_BUFFER_SIZE; i++) {
        LEDBuffer[pos++] = 0;
    }

    // Start DMA transfer
    HAL_TIM_PWM_Start_DMA(&htim3, TIM_CHANNEL_2, (uint32_t*)LEDBuffer, LED_BUFFER_SIZE);
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
    MX_TIM3_Init();

    /* Initialize USER push-button */
    BSP_PB_Init(BUTTON_USER, BUTTON_MODE_EXTI);

    /* Initialize COM1 port */
    COM_InitTypeDef BspCOMInit;
    BspCOMInit.BaudRate   = 115200;
    BspCOMInit.WordLength = COM_WORDLENGTH_8B;
    BspCOMInit.StopBits   = COM_STOPBITS_1;
    BspCOMInit.Parity     = COM_PARITY_NONE;
    BspCOMInit.HwFlowCtl  = COM_HWCONTROL_NONE;

    /* Demonstrate LED Control */
    // Clear all LEDs
    for (int i = 0; i < NUM_LEDS; i++) {
        WS2812_SetColor(i, 0, 0, 0);
    }

    // Set first LED to color with gamma correction
    WS2812_SetColor(0, 0xAA, 0xFF, 0x1D);  // GRB order
    WS2812_Refresh();

    /* Infinite loop */
    while (1)
    {
        HAL_Delay(1000);  // 1-second delay
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

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                                |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2
                                |RCC_CLOCKTYPE_D3PCLK1|RCC_CLOCKTYPE_D1PCLK1;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_HSI;
    RCC_ClkInitStruct.SYSCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB3CLKDivider = RCC_APB3_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_APB1_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_APB2_DIV1;
    RCC_ClkInitStruct.APB4CLKDivider = RCC_APB4_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
    {
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
