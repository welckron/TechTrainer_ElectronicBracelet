#ifndef GPIO_H
#define GPIO_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
  
#ifndef GPIO_PIN_0
#define GPIO_PIN_0		((uint16_t)0x0001)
#define GPIO_PIN_1		((uint16_t)0x0002)
#define GPIO_PIN_2		((uint16_t)0x0004)
#define GPIO_PIN_3		((uint16_t)0x0008)
#define GPIO_PIN_4		((uint16_t)0x0010)
#define GPIO_PIN_5		((uint16_t)0x0020)
#define GPIO_PIN_6		((uint16_t)0x0040)
#define GPIO_PIN_7		((uint16_t)0x0080)
#define GPIO_PIN_8		((uint16_t)0x0100)
#define GPIO_PIN_9		((uint16_t)0x0200)
#define GPIO_PIN_10		((uint16_t)0x0400)
#define GPIO_PIN_11		((uint16_t)0x0800)
#define GPIO_PIN_12		((uint16_t)0x1000)
#define GPIO_PIN_13		((uint16_t)0x2000)
#define GPIO_PIN_14		((uint16_t)0x4000)
#define GPIO_PIN_15		((uint16_t)0x8000)
#define GPIO_PIN_ALL	((uint16_t)0xFFFF)
#endif

typedef enum {
	GPIO_Mode_IN = 0x00,  /*!< GPIO Pin as General Purpose Input */
	GPIO_Mode_OUT = 0x01, /*!< GPIO Pin as General Purpose Output */
	GPIO_Mode_AF = 0x02,  /*!< GPIO Pin as Alternate Function */
	GPIO_Mode_AN = 0x03,  /*!< GPIO Pin as Analog input/output */
} GPIO_Mode_t;

typedef enum {
	GPIO_OType_PP = 0x00, /*!< GPIO Output Type Push-Pull */
	GPIO_OType_OD = 0x01  /*!< GPIO Output Type Open-Drain */
} GPIO_OType_t;

typedef enum {
	GPIO_Speed_Low = 0x00,    /*!< GPIO Speed Low */
	GPIO_Speed_Medium = 0x01, /*!< GPIO Speed Medium */
	GPIO_Speed_Fast = 0x02,   /*!< GPIO Speed Fast, not available on STM32F0xx devices */
	GPIO_Speed_High = 0x03    /*!< GPIO Speed High */
} GPIO_Speed_t;

typedef enum {
	GPIO_PuPd_NOPULL = 0x00, /*!< No pull resistor */
	GPIO_PuPd_UP = 0x01,     /*!< Pull up resistor enabled */
	GPIO_PuPd_DOWN = 0x02    /*!< Pull down resistor enabled */
} GPIO_PuPd_t;

void GPIO_Init(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_Mode_t GPIO_Mode, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed);
void GPIO_InitAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_OType_t GPIO_OType, GPIO_PuPd_t GPIO_PuPd, GPIO_Speed_t GPIO_Speed, uint8_t Alternate);
void GPIO_DeInit(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_SetPinAsInput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_SetPinAsOutput(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_SetPinAsAnalog(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_SetPinAsAlternate(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
void GPIO_SetPullResistor(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PuPd_t GPIO_PuPd);

#define GPIO_SetPinLow(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)((GPIO_Pin) << 16))
#define GPIO_SetPinHigh(GPIOx, GPIO_Pin)			((GPIOx)->BSRR = (uint32_t)(GPIO_Pin))
#define GPIO_SetPinValue(GPIOx, GPIO_Pin, val)	((val) ? GPIO_SetPinHigh(GPIOx, GPIO_Pin) : GPIO_SetPinLow(GPIOx, GPIO_Pin))
#define GPIO_TogglePinValue(GPIOx, GPIO_Pin)		((GPIOx)->ODR ^= (GPIO_Pin))
#define GPIO_SetPortValue(GPIOx, value)			((GPIOx)->ODR = (value))
#define GPIO_GetInputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->IDR & (GPIO_Pin)) == 0 ? 0 : 1)
#define GPIO_GetOutputPinValue(GPIOx, GPIO_Pin)	(((GPIOx)->ODR & (GPIO_Pin)) == 0 ? 0 : 1)
#define GPIO_GetPortInputValue(GPIOx)			((GPIOx)->IDR)
#define GPIO_GetPortOutputValue(GPIOx)			((GPIOx)->ODR)

uint16_t GPIO_GetPortSource(GPIO_TypeDef* GPIOx);
uint16_t GPIO_GetPinSource(uint16_t GPIO_Pin);
void GPIO_Lock(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin);
uint16_t GPIO_GetUsedPins(GPIO_TypeDef* GPIOx);
uint16_t GPIO_GetFreePins(GPIO_TypeDef* GPIOx);


/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
