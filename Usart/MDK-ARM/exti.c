#include "exti.h"

EXTI_Result_t EXTI_Attach(GPIO_TypeDef* GPIOx, uint16_t GPIO_Line, EXTI_Trigger_t trigger) {
	GPIO_PuPd_t PuPd;
	uint8_t pinsource, portsource;
	IRQn_Type irqchannel;
	
	/* Check if user wants to init more than one gpio pin for exti at a time */
	if (!(GPIO_Line && !(GPIO_Line & (GPIO_Line - 1)))) {
		uint8_t i;
		/* Check all pins */
		for (i = 0; i < 0x10; i++) {
			if (GPIO_Line & (1 << i)) {
				/* Attach one pin at a time */
				if (EXTI_Attach(GPIOx, 1 << i, trigger) != EXTI_Result_Ok) {
					/* If one failed, return error */
					return EXTI_Result_Error;
				}
			}
		}
				
		/* Return OK, all succedded */
		return EXTI_Result_Ok;
	}
	
	/* Check if line is already in use */
	if (
		(EXTI->IMR & GPIO_Line) || /*!< Interrupt already attached */
		(EXTI->EMR & GPIO_Line)    /*!< Event already attached */
	) {
		/* Return error */
		return EXTI_Result_Error;
	}
	
#if defined(STM32F0xx)
	/* Get IRQ channel */
	switch (GPIO_Line) {
		case GPIO_PIN_0:
		case GPIO_PIN_1:
			irqchannel = EXTI0_1_IRQn;
			break;
		case GPIO_PIN_2:
		case GPIO_PIN_3:
			irqchannel = EXTI2_3_IRQn;
			break;
		case GPIO_Pin_4:
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			irqchannel = EXTI4_15_IRQn;
			break;
		default:
			return EXTI_Result_Error;
	}
#else
	/* Get IRQ channel */
	switch (GPIO_Line) {
		case GPIO_PIN_0:
			irqchannel = EXTI0_IRQn;
			break;
		case GPIO_PIN_1:
			irqchannel = EXTI1_IRQn;
			break;
		case GPIO_PIN_2:
			irqchannel = EXTI2_IRQn;
			break;
		case GPIO_PIN_3:
			irqchannel = EXTI3_IRQn;
			break;
		case GPIO_PIN_4:
			irqchannel = EXTI4_IRQn;
			break;
		case GPIO_PIN_5:
		case GPIO_PIN_6:
		case GPIO_PIN_7:
		case GPIO_PIN_8:
		case GPIO_PIN_9:
			irqchannel = EXTI9_5_IRQn;
			break;
		case GPIO_PIN_10:
		case GPIO_PIN_11:
		case GPIO_PIN_12:
		case GPIO_PIN_13:
		case GPIO_PIN_14:
		case GPIO_PIN_15:
			irqchannel = EXTI15_10_IRQn;
			break;
		default:
			return EXTI_Result_Error;
	}
#endif

	/* Check pull settings */
	if (trigger == EXTI_Trigger_Falling) {
		PuPd = GPIO_PuPd_UP;
	} else if (trigger == EXTI_Trigger_Rising) {
		PuPd = GPIO_PuPd_DOWN;
	} else {
		PuPd = GPIO_PuPd_NOPULL;
	}
	
	/* Init GPIO pin */
	GPIO_Init(GPIOx, GPIO_Line, GPIO_Mode_IN, GPIO_OType_PP, PuPd, GPIO_Speed_Low);
	
	/* Calculate pinsource */
	pinsource = GPIO_GetPinSource(GPIO_Line);
	
	/* Calculate portsource */
	portsource = GPIO_GetPortSource(GPIOx);
	
	/* Enable SYSCFG clock */
	RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN;
	
	/* Connect proper GPIO to SYSCFG */
	SYSCFG->EXTICR[pinsource >> 2] &= ~(0x0F << (0x04 * (pinsource & 0x03)));
	SYSCFG->EXTICR[pinsource >> 2] |= (portsource << (0x04 * (pinsource & 0x03)));
	
	/* Clear first */
	EXTI->IMR &= ~GPIO_Line;
	EXTI->EMR &= ~GPIO_Line;
	
	/* Select interrupt mode */
	EXTI->IMR |= GPIO_Line;
	
	/* Clear first */
	EXTI->FTSR &= ~GPIO_Line;
	EXTI->RTSR &= ~GPIO_Line;
	
	/* Select edge */
	if (trigger == EXTI_Trigger_Falling) {
		/* Write to falling edge register */
		EXTI->FTSR |= GPIO_Line;
	} else if (trigger == EXTI_Trigger_Rising) {
		/* Write to rising edge register */
		EXTI->RTSR |= GPIO_Line;
	} else {
		/* Write to rising and falling edge registers */
		EXTI->FTSR |= GPIO_Line;
		EXTI->RTSR |= GPIO_Line;
	}
	
	/* Add to NVIC */
	HAL_NVIC_SetPriority(irqchannel, EXTI_NVIC_PRIORITY, pinsource);
	/* Enable interrupt */
	HAL_NVIC_EnableIRQ(irqchannel);

	/* Return OK */
	return EXTI_Result_Ok;
}

EXTI_Result_t EXTI_Detach(uint16_t GPIO_Line) {
	/* Disable EXTI for specific GPIO line */
	EXTI->IMR &= ~GPIO_Line;
	EXTI->EMR &= ~GPIO_Line;
	
	/* Clear trigger edges */
	EXTI->FTSR &= ~GPIO_Line;
	EXTI->RTSR &= ~GPIO_Line;
	
	/* Return OK */
	return EXTI_Result_Ok;
}

void EXTI_DeInit(void) {
	/* CLear EXTERNAL lines only = GPIO pins */
	EXTI->IMR &= 0xFFFF0000;
	EXTI->EMR &= 0xFFFF0000;
	EXTI->FTSR &= 0xFFFF0000;
	EXTI->RTSR &= 0xFFFF0000;
	EXTI->PR &= 0xFFFF0000;
}

__weak void EXTI_Handler(uint16_t GPIO_Pin) {
  /* NOTE : This function Should not be modified, when the callback is needed,
            the EXTI_Handler could be implemented in the user file
   */ 
}

/******************************************************************/
/*              STM32F4xx and STM32F7xx IRQ handlers              */
/******************************************************************/

#if defined(STM32F4xx) || defined(STM32F7xx)

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_0
void EXTI0_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR0)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR0;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_0);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_1
void EXTI1_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR1)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR1;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_1);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_2
void EXTI2_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR2)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR2;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_2);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_3
void EXTI3_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR3)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR3;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_3);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_4
void EXTI4_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR4)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR4;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_4);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_9_5
void EXTI9_5_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR5)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR5;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_5);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR6)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR6;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_6);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR7)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR7;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_7);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR8)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR8;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_8);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR9)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR9;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_9);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_15_10
void EXTI15_10_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR10)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR10;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_10);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR11)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR11;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_11);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR12)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR12;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_12);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR13)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR13;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_13);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR14)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR14;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_14);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR15)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR15;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_15);
	}
}
#endif

#endif

/******************************************************************/
/*                     STM32F0xx IRQ handlers                     */
/******************************************************************/

#if defined(STM32F0xx)

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_0_1
void EXTI0_1_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR0)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR0;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_0);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR1)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR1;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_1);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_2_3
void EXTI2_3_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR2)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR2;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_2);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR3)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR3;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_3);
	}
}
#endif

#ifndef EXTI_DISABLE_DEFAULT_HANDLER_4_15
void EXTI4_15_IRQHandler(void) {
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR4)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR4;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_4);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR5)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR5;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_5);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR6)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR6;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_6);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR7)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR7;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_7);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR8)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR8;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_8);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR9)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR9;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_9);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR10)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR10;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_10);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR11)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR11;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_11);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR12)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR12;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_12);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR13)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR13;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_13);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR14)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR14;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_14);
	}
	/* Check status */
	if (EXTI->PR & (EXTI_PR_PR15)) {
		/* Clear bit */
		EXTI->PR = EXTI_PR_PR15;
		/* Call global function */
		EXTI_Handler(GPIO_PIN_15);
	}
}
#endif

#endif
