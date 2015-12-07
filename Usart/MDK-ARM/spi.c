#include "spi.h"

/* Defines for alternate functions */
#define GPIO_AFx_SPI2    GPIO_AF5_SPI2

/* Private functions */
static void SPIx_Init(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack, SPI_Mode_t SPI_Mode, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_MasterSlave, uint16_t SPI_FirstBit);
void SPI2_INT_InitPins(SPI_PinsPack_t pinspack);

void SPI_Init(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack) {SPIx_Init(SPI2, pinspack, SPI2_MODE, SPI2_PRESCALER, SPI2_MASTERSLAVE, SPI2_FIRSTBIT);}

void SPI_InitWithMode(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack, SPI_Mode_t SPI_Mode) {SPIx_Init(SPI2, pinspack, SPI_Mode, SPI2_PRESCALER, SPI2_MASTERSLAVE, SPI2_FIRSTBIT);}

void SPI_InitFull(
	SPI_TypeDef* SPIx,              \
	SPI_PinsPack_t pinspack,     \
	uint16_t SPI_BaudRatePrescaler, \
	SPI_Mode_t SPI_Mode_t,       \
	uint16_t SPI_Mode,              \
	uint16_t SPI_FirstBit           \
) {
	/* Init FULL SPI settings by user */
	SPIx_Init(SPI2, pinspack, SPI_Mode_t, SPI_BaudRatePrescaler, SPI_Mode, SPI_FirstBit);
}

uint16_t SPI_GetPrescalerFromMaxFrequency(SPI_TypeDef* SPIx, uint32_t MAX_SPI_Frequency) {
	uint32_t APB_Frequency;
	uint8_t i;
	
	/* Prevent false input */
	if (MAX_SPI_Frequency == 0) {
		return SPI_BAUDRATEPRESCALER_256;
	}
	
	/* Calculate max SPI clock */
	APB_Frequency = HAL_RCC_GetPCLK1Freq();

	
	/* Calculate prescaler value */
	/* Bits 5:3 in CR1 SPI registers are prescalers */
	/* 000 = 2, 001 = 4, 002 = 8, ..., 111 = 256 */
	for (i = 0; i < 8; i++) {
		if (APB_Frequency / (1 << (i + 1)) <= MAX_SPI_Frequency) {
			/* Bits for BP are 5:3 in CR1 register */
			return (i << 3);
		}
	}
	
	/* Use max prescaler possible */
	return SPI_BAUDRATEPRESCALER_256;
}

SPI_DataSize_t SPI_SetDataSize(SPI_TypeDef* SPIx, SPI_DataSize_t DataSize) {
	SPI_DataSize_t status;
	
	/* Disable SPI first */
	SPIx->CR1 &= ~SPI_CR1_SPE;
	
	/* Read current SPI status */
	status = (SPIx->CR1 & SPI_CR1_DFF) ? SPI_DataSize_16b : SPI_DataSize_8b;

	/* Set proper value */
	if (DataSize == SPI_DataSize_16b) {
		/* Set bit for frame */
		SPIx->CR1 |= SPI_CR1_DFF;
	} else {
		/* Clear bit for frame */
		SPIx->CR1 &= ~SPI_CR1_DFF;
	}

	/* Enable SPI back */
	SPIx->CR1 |= SPI_CR1_SPE;
	
	/* Return status */
	return status;	
}

void SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint8_t* dataIn, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		*dataIn++ = *(__IO uint8_t *)&SPIx->DR;
	}
}

void SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		(void)*(__IO uint16_t *)&SPIx->DR;
	}
}

void SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t* dataIn, uint8_t dummy, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint8_t *)&SPIx->DR = dummy;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Save data to buffer */
		*dataIn++ = *(__IO uint8_t *)&SPIx->DR;
	}
}

void SPI_SendMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint16_t* dataIn, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint16_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		*dataIn++ = *(__IO uint16_t *)&SPIx->DR;
	}
}

void SPI_WriteMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		*(__IO uint16_t *)&SPIx->DR = *dataOut++;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Read data register */
		(void)*(__IO uint16_t *)&SPIx->DR;
	}
}

void SPI_ReadMulti16(SPI_TypeDef* SPIx, uint16_t* dataIn, uint16_t dummy, uint32_t count) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED(SPIx);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	while (count--) {
		/* Wait busy */
		SPI_WAIT_TX(SPIx);
		
		/* Fill output buffer with data */
		SPIx->DR = dummy;
		
		/* Wait for SPI to end everything */
		SPI_WAIT_RX(SPIx);
		
		/* Save data to buffer */
		*dataIn++ = SPIx->DR;
	}
}

__weak void SPI_InitCustomPinsCallback(SPI_TypeDef* SPIx, uint16_t AlternateFunction) { 
	/* NOTE: This function Should not be modified, when the callback is needed,
           the SPI_InitCustomPinsCallback could be implemented in the user file
   */
}

/* Private functions */
static void SPIx_Init(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack, SPI_Mode_t SPI_Mode, uint16_t SPI_BaudRatePrescaler, uint16_t SPI_MasterSlave, uint16_t SPI_FirstBit) {
	SPI_HandleTypeDef SPIHandle;
	
	/* Save instance */
	SPIHandle.Instance = SPIx;
	
	/* Enable SPI clock */
	__HAL_RCC_SPI2_CLK_ENABLE();
		
	/* Init pins */
	SPI2_INT_InitPins(pinspack);
		
	/* Set options */
	SPIHandle.Init.DataSize = SPI2_DATASIZE;

	/* Fill SPI settings */
	SPIHandle.Init.BaudRatePrescaler = SPI_BaudRatePrescaler;
	SPIHandle.Init.FirstBit = SPI_FirstBit;
	SPIHandle.Init.Mode = SPI_MasterSlave;
	
	SPIHandle.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
	SPIHandle.Init.CRCPolynomial = 7;
	SPIHandle.Init.TIMode = SPI_TIMODE_DISABLE;
	SPIHandle.Init.NSS = SPI_NSS_SOFT;
	SPIHandle.Init.Direction = SPI_DIRECTION_2LINES;
	
	/* SPI mode */
	if (SPI_Mode == SPI_Mode_0) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
		SPIHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	} else if (SPI_Mode == SPI_Mode_1) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_LOW;
		SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
	} else if (SPI_Mode == SPI_Mode_2) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
		SPIHandle.Init.CLKPhase = SPI_PHASE_1EDGE;
	} else if (SPI_Mode == SPI_Mode_3) {
		SPIHandle.Init.CLKPolarity = SPI_POLARITY_HIGH;
		SPIHandle.Init.CLKPhase = SPI_PHASE_2EDGE;
	}
	
	/* Disable first */
	__HAL_SPI_DISABLE(&SPIHandle);
	
	/* Init SPI */
	HAL_SPI_Init(&SPIHandle);
	
	/* Enable SPI */
	__HAL_SPI_ENABLE(&SPIHandle);
}

/* Private functions */
void SPI2_INT_InitPins(SPI_PinsPack_t pinspack) {
	/* Init SPI pins */
#if defined(GPIOB) && defined(GPIOC)
	if (pinspack == SPI_PinsPack_1) {
		GPIO_InitAlternate(GPIOB, GPIO_PIN_10, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
		GPIO_InitAlternate(GPIOC, GPIO_PIN_2 | GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOB)
	if (pinspack == SPI_PinsPack_2) {
		GPIO_InitAlternate(GPIOB, GPIO_PIN_13 | GPIO_PIN_14 | GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOI)
	if (pinspack == SPI_PinsPack_3) {
		GPIO_InitAlternate(GPIOI, GPIO_PIN_0 | GPIO_PIN_2 | GPIO_PIN_3, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
#if defined(GPIOB) && defined(GPIOI)
	if (pinspack == SPI_PinsPack_4) {
		GPIO_InitAlternate(GPIOB, GPIO_PIN_14 | GPIO_PIN_15, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
		GPIO_InitAlternate(GPIOI, GPIO_PIN_1, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High, GPIO_AFx_SPI2);
	}
#endif
	if (pinspack == SPI_PinsPack_Custom) {
		/* Call user function */
		SPI_InitCustomPinsCallback(SPI2, GPIO_AFx_SPI2);
	}
}


