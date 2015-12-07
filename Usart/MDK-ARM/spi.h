#ifndef SPI_H
#define SPI_H 100

/* C++ detection */
#ifdef __cplusplus
extern "C" {
#endif

/*
        |PINS PACK 1            |PINS PACK 2            |PINS PACK 3           |PINS PACK 4
SPIX    |MOSI   MISO    SCK     |MOSI   MISO    SCK     |MOSI   MISO    SCK    |MOSI   MISO    SCK
        |                                                                           
SPI2    |PC3    PC2     PB10    |PB15   PB14    PB13    |PI3    PI2     PI0    |PB15   PB14    PI1

*/

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_spi.h"
#include "attributes.h"
#include "gpio.h"

/**
 * @brief  SPI modes selection
 */
typedef enum {
	SPI_Mode_0 = 0x00, /*!< Clock polarity low, clock phase 1st edge */
	SPI_Mode_1,        /*!< Clock polarity low, clock phase 2nd edge */
	SPI_Mode_2,        /*!< Clock polarity high, clock phase 1st edge */
	SPI_Mode_3         /*!< Clock polarity high, clock phase 2nd edge */
} SPI_Mode_t;

/**
 * @brief  SPI PinsPack enumeration to select pins combination for SPI
 */
typedef enum {
	SPI_PinsPack_1 = 0x00, /*!< Select PinsPack1 from Pinout table for specific SPI */
	SPI_PinsPack_2,        /*!< Select PinsPack2 from Pinout table for specific SPI */
	SPI_PinsPack_3,        /*!< Select PinsPack3 from Pinout table for specific SPI */
	SPI_PinsPack_4,        /*!< Select PinsPack4 from Pinout table for specific SPI */
	SPI_PinsPack_Custom    /*!< Select custom pins for specific SPI, callback will be called, look @ref SPI_InitCustomPinsCallback */
} SPI_PinsPack_t;

/**
 * @brief  Daza size enumeration
 */
typedef enum {
	SPI_DataSize_8b = 0x00, /*!< SPI in 8-bits mode */
	SPI_DataSize_16b        /*!< SPI in 16-bits mode */        
} SPI_DataSize_t;



//----- SPI2 options start -------
//Options can be overwriten in defines.h file
#ifndef SPI2_PRESCALER
#define SPI2_PRESCALER	SPI_BAUDRATEPRESCALER_32
#endif
//Specify datasize
#ifndef SPI2_DATASIZE
#define SPI2_DATASIZE 	SPI_DATASIZE_8BIT
#endif
//Specify which bit is first
#ifndef SPI2_FIRSTBIT
#define SPI2_FIRSTBIT 	SPI_FIRSTBIT_MSB
#endif
//Mode, master or slave
#ifndef SPI2_MASTERSLAVE
#define SPI2_MASTERSLAVE SPI_MODE_MASTER
#endif
//Specify mode of operation, clock polarity and clock phase
#ifndef SPI2_MODE
#define SPI2_MODE		SPI_Mode_0
#endif
//----- SPI2 options end -------


#define SPI_IS_BUSY(SPIx)                   (((SPIx)->SR & (SPI_SR_TXE | SPI_SR_RXNE)) == 0)
#define SPI_WAIT_TX(SPIx)                   while ((SPIx->SR & SPI_FLAG_TXE) == 0 || (SPIx->SR & SPI_FLAG_BSY))
#define SPI_WAIT_RX(SPIx)                   while ((SPIx->SR & SPI_FLAG_RXNE) == 0 || (SPIx->SR & SPI_FLAG_BSY))
#define SPI_CHECK_ENABLED(SPIx)             if (!((SPIx)->CR1 & SPI_CR1_SPE)) {return;}
#define SPI_CHECK_ENABLED_RESP(SPIx, val)   if (!((SPIx)->CR1 & SPI_CR1_SPE)) {return (val);}


void SPI_Init(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack);
void SPI_InitWithMode(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack, SPI_Mode_t SPI_Mode);
void SPI_InitFull(SPI_TypeDef* SPIx, SPI_PinsPack_t pinspack, uint16_t SPI_BaudRatePrescaler, SPI_Mode_t SPI_Mode_t, uint16_t SPI_Mode, uint16_t SPI_FirstBit);
uint16_t SPI_GetPrescalerFromMaxFrequency(SPI_TypeDef* SPIx, uint32_t MAX_SPI_Frequency);
SPI_DataSize_t SPI_SetDataSize(SPI_TypeDef* SPIx, SPI_DataSize_t DataSize);
static __INLINE uint8_t SPI_Send(SPI_TypeDef* SPIx, uint8_t data) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED_RESP(SPIx, 0);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	/* Fill output buffer with data */
	*(__IO uint8_t *)&SPIx->DR = data;
	
	/* Wait for transmission to complete */
	SPI_WAIT_RX(SPIx);
	
	/* Return data from buffer */
	return SPIx->DR;
}

void SPI_SendMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint8_t* dataIn, uint32_t count);
void SPI_WriteMulti(SPI_TypeDef* SPIx, uint8_t* dataOut, uint32_t count);
void SPI_ReadMulti(SPI_TypeDef* SPIx, uint8_t *dataIn, uint8_t dummy, uint32_t count);
static __INLINE uint16_t SPI_Send16(SPI_TypeDef* SPIx, uint8_t data) {
	/* Check if SPI is enabled */
	SPI_CHECK_ENABLED_RESP(SPIx, 0);
	
	/* Wait for previous transmissions to complete if DMA TX enabled for SPI */
	SPI_WAIT_TX(SPIx);
	
	/* Fill output buffer with data */
	SPIx->DR = data;
	
	/* Wait for transmission to complete */
	SPI_WAIT_RX(SPIx);
	
	/* Return data from buffer */
	return SPIx->DR;
}

void SPI_SendMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint16_t* dataIn, uint32_t count);
void SPI_WriteMulti16(SPI_TypeDef* SPIx, uint16_t* dataOut, uint32_t count);
void SPI_ReadMulti16(SPI_TypeDef* SPIx, uint16_t* dataIn, uint16_t dummy, uint32_t count);
void SPI_InitCustomPinsCallback(SPI_TypeDef* SPIx, uint16_t AlternateFunction);

/* C++ detection */
#ifdef __cplusplus
}
#endif

#endif

