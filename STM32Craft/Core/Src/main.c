/**
 ******************************************************************************
 * @file           : main.c
 * @brief          : Main program body
 *
 * Purpose:
 * - Default state: All LEDs off.
 * - Button toggles between running animation continuously and resetting to default.
 * - Animation speeds can be adjusted for each transition individually.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "memorymap.h"
#include "gpio.h"

/* Private variables ---------------------------------------------------------*/
uint8_t animation_active = 0;        // Flag to track animation state (0 = off, 1 = on)
uint8_t button_stable = 0;           // Tracks stable button state (0 = released, 1 = pressed)
uint32_t last_debounce_time = 0;     // Tracks debounce timing
uint32_t last_animation_time = 0;    // Tracks last animation step time
uint8_t animation_step = 0;          // Tracks current animation step

/* Constants -----------------------------------------------------------------*/
#define DEBOUNCE_DELAY 50            // Debounce delay in ms
const uint32_t animation_delays[] = {500, 300, 150}; // Adjustable delays for each LED transition (in ms)
#define NUM_ANIMATION_STEPS (sizeof(animation_delays) / sizeof(animation_delays[0]))

/**
 * @brief Configures the system clock.
 */
void SystemClock_Config(void);

/**
 * @brief The application entry point.
 * @retval int
 */
int main(void)
{
    /* Initialize the Hardware Abstraction Layer (HAL) and reset peripherals. */
    HAL_Init();

    /* Configure the system clock. */
    SystemClock_Config();

    /* Initialize all configured peripherals. */
    MX_GPIO_Init();

    /* Infinite loop */
    while (1)
    {
        /* Debounced button press handling */
        if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET) // Active low
        {
            if ((HAL_GetTick() - last_debounce_time) > DEBOUNCE_DELAY) // Debounce check
            {
                if (!button_stable) // If button state changed
                {
                    button_stable = 1; // Mark button as pressed
                    last_debounce_time = HAL_GetTick(); // Reset debounce timer

                    /* Toggle animation state */
                    animation_active = !animation_active;

                    /* If animation is turned off, ensure all LEDs are off */
                    if (!animation_active)
                    {
                        HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
                        HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
                    }
                }
            }
        }
        else
        {
            if ((HAL_GetTick() - last_debounce_time) > DEBOUNCE_DELAY) // Debounce release
            {
                button_stable = 0; // Mark button as released
                last_debounce_time = HAL_GetTick(); // Reset debounce timer
            }
        }

        /* Run animation if active */
        if (animation_active)
        {
            if (HAL_GetTick() - last_animation_time >= animation_delays[animation_step]) // Use individual delay for each step
            {
                last_animation_time = HAL_GetTick(); // Update last animation time

                /* Perform animation steps */
                switch (animation_step)
                {
                case 0:
                    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_SET);
                    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_RESET);
                    break;
                case 1:
                    HAL_GPIO_WritePin(LD1_GPIO_Port, LD1_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
                    break;
                case 2:
                    HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
                    HAL_GPIO_WritePin(LD3_GPIO_Port, LD3_Pin, GPIO_PIN_SET);
                    break;
                }

                /* Update animation step */
                animation_step = (animation_step + 1) % NUM_ANIMATION_STEPS; // Cycle through steps
            }
        }
    }
}

/**
 * @brief Configures the system clock.
 * @retval None
 */
void SystemClock_Config(void)
{
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

    /* Configure power supply. */
    HAL_PWREx_ConfigSupply(PWR_LDO_SUPPLY);

    /* Set voltage scaling for power efficiency. */
    __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE2);

    /* Wait until the voltage regulator is ready. */
    while (!__HAL_PWR_GET_FLAG(PWR_FLAG_VOSRDY))
    {
    }

    /* Initialize the oscillators. */
    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
    RCC_OscInitStruct.HSIState = RCC_HSI_DIV1;
    RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
    {
        Error_Handler();
    }

    /* Configure the CPU, AHB, and APB clocks. */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2 | RCC_CLOCKTYPE_D3PCLK1 | RCC_CLOCKTYPE_D1PCLK1;
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
 * @brief Handles system-level errors.
 * @retval None
 */
void Error_Handler(void)
{
    /* Disable interrupts and enter infinite loop on error. */
    __disable_irq();
    while (1)
    {
    }
}

#ifdef USE_FULL_ASSERT
/**
 * @brief Reports the name of the source file and line number where an assert_param error occurred.
 * @param file: Pointer to the source file name.
 * @param line: Line number of the error.
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{
    /* Customize error reporting as needed. */
}
#endif /* USE_FULL_ASSERT */

