#ifndef DELAY_H
#define DELAY_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

#include "stm32f4xx_hal.h"
#include "stdlib.h"

#ifndef DELAY_MAX_CUSTOM_TIMERS
#define DELAY_MAX_CUSTOM_TIMERS   5
#endif

/* Memory allocation function */
#ifndef LIB_ALLOC_FUNC
#define LIB_ALLOC_FUNC    malloc
#endif

/* Memory free function */
#ifndef LIB_FREE_FUNC
#define LIB_FREE_FUNC     free
#endif
 
typedef struct _DELAY_Timer_t {
	union {
		struct {			
			uint8_t AREN:1;  /*!< Auto-reload enabled */
			uint8_t CNTEN:1; /*!< Count enabled */
		} F;
		uint8_t FlagsVal;
	} Flags;
	uint32_t ARR;                                        /*!< Auto reload value */
	uint32_t CNT;                                        /*!< Counter value, counter counts down */
	void (*Callback)(struct _DELAY_Timer_t*, void *); /*!< Callback which will be called when timer reaches zero */
	void* UserParameters;                                /*!< Pointer to user parameters used for callback function */
} DELAY_Timer_t;

extern __IO uint32_t Time;
extern __IO uint32_t Time2;


uint32_t DELAY_Init(void);

/**
 * @brief  Delays for amount of micro seconds
 * @param  micros: Number of microseconds for delay
 * @retval None
 */
__STATIC_INLINE void Delay(__IO uint32_t micros) {
#if !defined(STM32F0xx)
	uint32_t start = DWT->CYCCNT;
	
	/* Go to number of cycles for system */
	micros *= (HAL_RCC_GetHCLKFreq() / 1000000);
	
	/* Delay till end */
	while ((DWT->CYCCNT - start) < micros);
#else
	/* Go to clock cycles */
	micros *= (SystemCoreClock / 1000000) / 5;
	
	/* Wait till done */
	while (micros--);
#endif
}

/**
 * @brief  Delays for amount of milli seconds
 * @param  millis: Number of milliseconds for delay
 * @retval None
 */
__STATIC_INLINE void Delayms(uint32_t millis) {
	/* Use HAL library for delay ms purpose */
	HAL_Delay(millis);
}

/**
 * @brief  Gets the Time variable value
 * @param  None
 * @retval Current time in milliseconds
 */
#define DELAY_Time()					(Time)

/**
 * @brief  Sets value for Time variable
 * @param  time: Time in milliseconds
 * @retval None
 */
#define DELAY_SetTime(time)			(Time = (time))

/**
 * @brief  Gets the Time2 variable value
 * @param  None
 * @retval Current time in milliseconds
 * @note   This is not meant for public use
 */
#define DELAY_Time2()				(Time2)

/**
 * @brief  Sets value for Time variable
 * @param  time: Time in milliseconds
 * @retval None
 * @note   This is not meant for public use
 */
#define DELAY_SetTime2(time)			(Time2 = (time))

/**
 * @defgroup DELAY_Timer_Functions
 * @brief    Software timer functions
 * @{
 */

/**
 * @brief  Creates a new custom timer which has 1ms resolution
 * @note   It uses @ref malloc for memory allocation for timer structure
 * @param  ReloadValue: Number of milliseconds when timer reaches zero and callback function is called
 * @param  AutoReloadCmd: If set to 1, timer will start again when it reaches zero and callback is called
 * @param  StartTimer: If set to 1, timer will start immediately
 * @param  *DELAY_CustomTimerCallback: Pointer to callback function which will be called when timer reaches zero
 * @param  *UserParameters: Pointer to void pointer to user parameters used as first parameter in callback function
 * @retval Pointer to allocated timer structure
 */
DELAY_Timer_t* DELAY_TimerCreate(uint32_t ReloadValue, uint8_t AutoReloadCmd, uint8_t StartTimer, void (*DELAY_CustomTimerCallback)(struct _DELAY_Timer_t*, void *), void* UserParameters);

/**
 * @brief  Deletes already allocated timer
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @retval None
 */
void DELAY_TimerDelete(DELAY_Timer_t* Timer);

/**
 * @brief  Stops custom timer from counting
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @retval Pointer to @ref DELAY_Timer_t structure
 */
DELAY_Timer_t* DELAY_TimerStop(DELAY_Timer_t* Timer);

/**
 * @brief  Starts custom timer counting
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @retval Pointer to @ref DELAY_Timer_t structure
 */
DELAY_Timer_t* DELAY_TimerStart(DELAY_Timer_t* Timer);

/**
 * @brief  Resets custom timer counter value
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @retval Pointer to @ref DELAY_Timer_t structure
 */
DELAY_Timer_t* DELAY_TimerReset(DELAY_Timer_t* Timer);

/**
 * @brief  Sets auto reload feature for timer
 * @note   Auto reload features is used for timer which starts again when zero is reached if auto reload active
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @param  AutoReload: Set to 1 if you want to enable AutoReload or 0 to disable
 * @retval Pointer to @ref DELAY_Timer_t structure
 */
DELAY_Timer_t* DELAY_TimerAutoReloadCommand(DELAY_Timer_t* Timer, uint8_t AutoReloadCmd);

/**
 * @brief  Sets auto reload value for timer
 * @param  *Timer: Pointer to @ref DELAY_Timer_t structure
 * @param  AutoReloadValue: Value for timer to be set when zero is reached and callback is called
 * @note   AutoReload feature must be enabled for timer in order to get this to work properly
 * @retval Pointer to @ref DELAY_Timer_t structure
 */
DELAY_Timer_t* DELAY_TimerAutoReloadValue(DELAY_Timer_t* Timer, uint32_t AutoReloadValue);

/**
 * @}
 */

/**
 * @brief  User function, called each 1ms when interrupt from timer happen
 * @note   Here user should put things which has to be called periodically
 * @param  None
 * @retval None
 * @note   With __weak parameter to prevent link errors if not defined by user
 */
void DELAY_1msHandler(void);

/**
 * @}
 */
 
/**
 * @}
 */
 
/**
 * @}
 */

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif
