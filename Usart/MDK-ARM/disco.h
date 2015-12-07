
#ifndef DISCO_H
#define DISCO_H 110

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "gpio.h"
#define STM32F4_DISCOVERY

#if defined(STM32F4_DISCOVERY)
	#define LED_GREEN					0x1000U
	#define LED_ORANGE					0x2000U
	#define LED_RED						0x4000U
	#define LED_BLUE					0x8000U
	#define LED_ALL						((uint16_t)((uint16_t)LED_GREEN | (uint16_t)LED_RED | (uint16_t)LED_ORANGE | (uint16_t)LED_BLUE))
	
	#define DISCO_LED_PORT				GPIOD
	#define DISCO_LED_PINS				LED_GREEN | LED_RED | LED_ORANGE | LED_BLUE

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0x0001U
	#define DISCO_BUTTON_PRESSED		1
	#define DISCO_BUTTON_PULL			GPIO_PuPd_DOWN
#else
	#warning "stm32_disco.h: Please select your board. Open stm32_disco.h and follow instructions! Support is currently disabled"
	
	#define LED_GREEN					0
	#define LED_RED						0
	#define LED_ORANGE					0
	#define LED_BLUE					0
	#define LED_ALL						0
	
	#define DISCO_LED_PORT				GPIOA
	#define DISCO_LED_PINS				LED_ALL

	#define DISCO_BUTTON_PORT			GPIOA
	#define DISCO_BUTTON_PIN			0
	#define DISCO_BUTTON_PRESSED		0
	#define DISCO_BUTTON_PULL			GPIO_PuPd_NOPULL
#endif

void DISCO_LedInit(void);

/**
 * @brief  Configures Button pin as input
 * @param  None
 * @retval None
 */
void DISCO_ButtonInit(void);

/**
 * @brief  Turns on LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn on
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS) 
void DISCO_LedOn(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define DISCO_LedOn(led)        GPIO_SetPinHigh(DISCO_LED_PORT, (uint16_t)(led))
#else
#define DISCO_LedOn(led)        GPIO_SetPinLow(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Turns off LED on board
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: LED you want to turn off
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS) 
void DISCO_LedOff(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define DISCO_LedOff(led)       GPIO_SetPinLow(DISCO_LED_PORT, (uint16_t)(led))
#else
#define DISCO_LedOff(led)       GPIO_SetPinHigh(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Toggles LED on board
 * @param  led: LED you want to toggle
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @retval None
 */
#if defined(DISCO_USE_FUNCTIONS)
void DISCO_LedToggle(uint16_t led);
#else
#define DISCO_LedToggle(led)        GPIO_TogglePinValue(DISCO_LED_PORT, (uint16_t)(led))
#endif

/**
 * @brief  Checks if led is on
 * @note   STM32F4x9-Eval board uses inverse logic for leds
 * @param  led: Led you want to checking
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 * @retval Led on status:
 *           - 0: Led is off
 *           - > 0: Led is on
 */
#if defined(DISCO_USE_FUNCTIONS)
uint16_t DISCO_LedIsOn(uint16_t led);
#else
#ifndef DISCO_SWAP_LOGIC
#define DISCO_LedIsOn(led)          GPIO_GetOutputPinValue(DISCO_LED_PORT, (uint16_t)(led))
#else
#define DISCO_LedIsOn(led)          !GPIO_GetOutputPinValue(DISCO_LED_PORT, (uint16_t)(led))
#endif
#endif

/**
 * @brief  Sets led value
 * @param  led: LED you want to set value
 *            - LED_RED: Red led
 *            - LED_GREEN: Green led
 *            - LED_BLUE: Blue led
 *            - LED_ORANGE: Orange led
 *            - LED_ALL: All leds
 * @param  state: Set or clear led 
 *            - 0: led is off
 *            - > 0: led is on
 * @retval None
 */
#define DISCO_SetLed(led, state)    ((state) ? DISCO_LedOn(led): DISCO_LedOff(led))

/**
 * @brief  Checks if user button is pressed
 * @param  None
 * @retval Button status
 *            - 0: Button is not pressed
 *            - > 0: Button is pressed
 */
#define DISCO_ButtonPressed()       ((GPIO_GetInputPinValue(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN) == 0) != DISCO_BUTTON_PRESSED)

/**
 * @brief  Checks if button was pressed now, but was not already pressed before
 * @param  None
 * @retval Button on pressed value
 *           - 0: In case that button has been already pressed on last call or was not pressed at all yet
 *           - > 0: Button was pressed, but state before was released
 */
uint8_t DISCO_ButtonOnPressed(void);

/**
 * @brief  Checks if button was released now, but was already pressed before
 * @param  None
 * @retval Button on released value
 *           - 0: In case that button has been already released on last call or was not released at all yet
 *           - > 0: Button was released, but state before was pressed
 */
uint8_t DISCO_ButtonOnReleased(void);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
