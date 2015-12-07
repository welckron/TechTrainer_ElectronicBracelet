#ifndef EXTI_H
#define EXTI_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "attributes.h"
#include "gpio.h"

#ifndef EXTI_NVIC_PRIORITY
#define EXTI_NVIC_PRIORITY     0x03
#endif

typedef enum {
	EXTI_Result_Ok = 0x00, /*!< Everything ok */
	EXTI_Result_Error      /*!< An error has occured */
} EXTI_Result_t;

/**
 * @brief  Interrupt trigger enumeration	
 */
typedef enum {
	EXTI_Trigger_Rising = 0x00, /*!< Trigger interrupt on rising edge on line, pull down resistor active */
	EXTI_Trigger_Falling,       /*!< Trigger interrupt on falling edge on line, pull up resistor active */
	EXTI_Trigger_Rising_Falling /*!< Trigger interrupt on any edge on line, no pull resistor active */
} EXTI_Trigger_t;

/**
 * @}
 */

/**
 * @defgroup EXTI_Functions
 * @brief    Library Functions
 * @{
 */

/**
 * @brief  Attach external interrupt on specific GPIO pin 
 * @note   This function automatically enables the clock for GPIO peripheral and also sets pull resistors depending on trigger you use.
 * 	          - Falling edge: pull up is enabled
 * 	          - Rising edge: pull down is enabled
 * 	          - Any edge: no pull activated
 *
 * @note   You can attach only one GPIOx to specific GPIO_PIN.
 *         In other words, GPIO_PIN_5 can not be attached to GPIOA and GPIOB at the same time.
 *         If you will try that, my function will return error to you, because you have to detach GPIO_Line first and attach back on other GPIO port.
 *
 * @param  *GPIOx: GPIO port where you want EXTI interrupt line
 * @param  GPIO_Line: GPIO pin where you want EXTI interrupt line. Use OR (|) operator if you want to attach
 *            interrupt on more than one GPIO pin at the same GPIOx at the same time.
 * @param  trigger: Pin trigger source. This parameter can be a value of @ref EXTI_Trigger_t enumeration
 * @retval Attach result:
 *            - @arg EXTI_Result_Ok: Everything ok, interrupt attached
 *            - @arg EXTI_Result_Error: An error occurred, interrupt was not attached
 * @note   If you use more than one GPIO_Pin with OR (|) operator at single call and if GPIO_Pin can't be attached because
 *         there is already one GPIO_Pin at this line, function will return error and other pins might not be initialized.
 *         If function return @arg EXTI_Result_Ok, then all pins are attached correctly.
 */
EXTI_Result_t EXTI_Attach(GPIO_TypeDef* GPIOx, uint16_t GPIO_Line, EXTI_Trigger_t trigger);

/**
 * @brief  Detach GPIO pin from interrupt lines
 * @param  GPIO_Line: GPIO line you want to disable. Valid GPIO is GPIO_Pin_0 to GPIO_Pin_15. 
 *         Use OR (|) operator if you want to detach interrupt in more than one GPIO pin at the same GPIOx at the same time.
 * @retval Detaching result:
 *            - @arg EXTI_Result_Ok: Everything ok
 *            - @arg EXTI_Result_Error: An error occurred
 */
EXTI_Result_t EXTI_Detach(uint16_t GPIO_Line);

/**
 * @brief  Clears all interrupts on EXTI line
 * @note   It clears bits for external pins (bit 0 to bit 15) only!
 *         It has no effect for internally connected peripherals (like RTC) to EXTI line
 * @param  None
 * @retval None
 */
void EXTI_DeInit(void);

/**
 * @brief  Creates software interrupt for specific external GPIO line
 * @note   This also works for others EXTI lines from 16 to 23
 * @param  GPIO_Line: GPIO line where you want software interrupt
 * @retval None
 * @note   Defined as macro for faster execution
 */
#define EXTI_SoftwareInterrupt(GPIO_Line)	(EXTI->SWIER |= (GPIO_Line))

/**
 * @brief  EXTI Global handler
 * @note   This function is called from TM EXTI library each time any interrupt occurs on EXTI line.
 * @note   With __weak parameter to prevent link errors if not defined by user.
 * @param  GPIO_Pin: GPIO Line where interrupt occurred so you can identify what to do
 * @retval None
 */
void EXTI_Handler(uint16_t GPIO_Pin);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
