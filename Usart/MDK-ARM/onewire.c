#include "onewire.h"

void OneWire_Init(OneWire_t* OneWireStruct, GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
	/* Initialize delay if it was not already */
	DELAY_Init();

	/* Init GPIO pin */
	GPIO_Init(GPIOx, GPIO_Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_UP, GPIO_Speed_Medium);
	
	/* Save settings */
	OneWireStruct->GPIOx = GPIOx;
	OneWireStruct->GPIO_Pin = GPIO_Pin;
}

uint8_t OneWire_Reset(OneWire_t* OneWireStruct) {
	uint8_t i;
	
	/* Line low, and wait 480us */
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(480);
	
	/* Release line and wait for 70us */
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(70);
	
	/* Check bit value */
	i = GPIO_GetInputPinValue(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin);
	
	/* Delay for 410 us */
	ONEWIRE_DELAY(410);
	
	/* Return value of presence pulse, 0 = OK, 1 = ERROR */
	return i;
}

void OneWire_WriteBit(OneWire_t* OneWireStruct, uint8_t bit) {
	if (bit) {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(10);
		
		/* Bit high */
		ONEWIRE_INPUT(OneWireStruct);
		
		/* Wait for 55 us and release the line */
		ONEWIRE_DELAY(55);
		ONEWIRE_INPUT(OneWireStruct);
	} else {
		/* Set line low */
		ONEWIRE_LOW(OneWireStruct);
		ONEWIRE_OUTPUT(OneWireStruct);
		ONEWIRE_DELAY(65);
		
		/* Bit high */
		ONEWIRE_INPUT(OneWireStruct);
		
		/* Wait for 5 us and release the line */
		ONEWIRE_DELAY(5);
		ONEWIRE_INPUT(OneWireStruct);
	}
}

uint8_t OneWire_ReadBit(OneWire_t* OneWireStruct) {
	uint8_t bit = 0;
	
	/* Line low */
	ONEWIRE_LOW(OneWireStruct);
	ONEWIRE_OUTPUT(OneWireStruct);
	ONEWIRE_DELAY(3);
	
	/* Release line */
	ONEWIRE_INPUT(OneWireStruct);
	ONEWIRE_DELAY(10);
	
	/* Read line value */
	if (GPIO_GetInputPinValue(OneWireStruct->GPIOx, OneWireStruct->GPIO_Pin)) {
		/* Bit is HIGH */
		bit = 1;
	}
	
	/* Wait 50us to complete 60us period */
	ONEWIRE_DELAY(50);
	
	/* Return bit value */
	return bit;
}

void OneWire_WriteByte(OneWire_t* OneWireStruct, uint8_t byte) {
	uint8_t i = 8;
	/* Write 8 bits */
	while (i--) {
		/* LSB bit is first */
		OneWire_WriteBit(OneWireStruct, byte & 0x01);
		byte >>= 1;
	}
}

uint8_t OneWire_ReadByte(OneWire_t* OneWireStruct) {
	uint8_t i = 8, byte = 0;
	while (i--) {
		byte >>= 1;
		byte |= (OneWire_ReadBit(OneWireStruct) << 7);
	}
	
	return byte;
}

uint8_t OneWire_First(OneWire_t* OneWireStruct) {
	/* Reset search values */
	OneWire_ResetSearch(OneWireStruct);

	/* Start with searching */
	return OneWire_Search(OneWireStruct, ONEWIRE_CMD_SEARCHROM);
}

uint8_t OneWire_Next(OneWire_t* OneWireStruct) {
   /* Leave the search state alone */
   return OneWire_Search(OneWireStruct, ONEWIRE_CMD_SEARCHROM);
}

void OneWire_ResetSearch(OneWire_t* OneWireStruct) {
	/* Reset the search state */
	OneWireStruct->LastDiscrepancy = 0;
	OneWireStruct->LastDeviceFlag = 0;
	OneWireStruct->LastFamilyDiscrepancy = 0;
}

uint8_t OneWire_Search(OneWire_t* OneWireStruct, uint8_t command) {
	uint8_t id_bit_number;
	uint8_t last_zero, rom_byte_number, search_result;
	uint8_t id_bit, cmp_id_bit;
	uint8_t rom_byte_mask, search_direction;

	/* Initialize for search */
	id_bit_number = 1;
	last_zero = 0;
	rom_byte_number = 0;
	rom_byte_mask = 1;
	search_result = 0;

	/* Check if any devices */
	if (!OneWireStruct->LastDeviceFlag) {
		/* 1-Wire reset */
		if (OneWire_Reset(OneWireStruct)) {
			/* Reset the search */
			OneWireStruct->LastDiscrepancy = 0;
			OneWireStruct->LastDeviceFlag = 0;
			OneWireStruct->LastFamilyDiscrepancy = 0;
			return 0;
		}

		/* Issue the search command */
		OneWire_WriteByte(OneWireStruct, command);  

		/* Loop to do the search */
		do {
			/* Read a bit and its complement */
			id_bit = OneWire_ReadBit(OneWireStruct);
			cmp_id_bit = OneWire_ReadBit(OneWireStruct);

			/* Check for no devices on 1-wire */
			if ((id_bit == 1) && (cmp_id_bit == 1)) {
				break;
			} else {
				/* All devices coupled have 0 or 1 */
				if (id_bit != cmp_id_bit) {
					/* Bit write value for search */
					search_direction = id_bit;
				} else {
					/* If this discrepancy is before the Last Discrepancy on a previous next then pick the same as last time */
					if (id_bit_number < OneWireStruct->LastDiscrepancy) {
						search_direction = ((OneWireStruct->ROM_NO[rom_byte_number] & rom_byte_mask) > 0);
					} else {
						/* If equal to last pick 1, if not then pick 0 */
						search_direction = (id_bit_number == OneWireStruct->LastDiscrepancy);
					}
					
					/* If 0 was picked then record its position in LastZero */
					if (search_direction == 0) {
						last_zero = id_bit_number;

						/* Check for Last discrepancy in family */
						if (last_zero < 9) {
							OneWireStruct->LastFamilyDiscrepancy = last_zero;
						}
					}
				}

				/* Set or clear the bit in the ROM byte rom_byte_number with mask rom_byte_mask */
				if (search_direction == 1) {
					OneWireStruct->ROM_NO[rom_byte_number] |= rom_byte_mask;
				} else {
					OneWireStruct->ROM_NO[rom_byte_number] &= ~rom_byte_mask;
				}
				
				/* Serial number search direction write bit */
				OneWire_WriteBit(OneWireStruct, search_direction);

				/* Increment the byte counter id_bit_number and shift the mask rom_byte_mask */
				id_bit_number++;
				rom_byte_mask <<= 1;

				/* If the mask is 0 then go to new SerialNum byte rom_byte_number and reset mask */
				if (rom_byte_mask == 0) {
					rom_byte_number++;
					rom_byte_mask = 1;
				}
			}
		/* Loop until through all ROM bytes 0-7 */
		} while (rom_byte_number < 8);

		/* If the search was successful then */
		if (!(id_bit_number < 65)) {
			/* Search successful so set LastDiscrepancy, LastDeviceFlag, search_result */
			OneWireStruct->LastDiscrepancy = last_zero;

			/* Check for last device */
			if (OneWireStruct->LastDiscrepancy == 0) {
				OneWireStruct->LastDeviceFlag = 1;
			}

			search_result = 1;
		}
	}

	/* If no device found then reset counters so next 'search' will be like a first */
	if (!search_result || !OneWireStruct->ROM_NO[0]) {
		OneWireStruct->LastDiscrepancy = 0;
		OneWireStruct->LastDeviceFlag = 0;
		OneWireStruct->LastFamilyDiscrepancy = 0;
		search_result = 0;
	}

	return search_result;
}

int OneWire_Verify(OneWire_t* OneWireStruct) {
	unsigned char rom_backup[8];
	int i,rslt,ld_backup,ldf_backup,lfd_backup;

	/* Keep a backup copy of the current state */
	for (i = 0; i < 8; i++)
	rom_backup[i] = OneWireStruct->ROM_NO[i];
	ld_backup = OneWireStruct->LastDiscrepancy;
	ldf_backup = OneWireStruct->LastDeviceFlag;
	lfd_backup = OneWireStruct->LastFamilyDiscrepancy;

	/* Set search to find the same device */
	OneWireStruct->LastDiscrepancy = 64;
	OneWireStruct->LastDeviceFlag = 0;

	if (OneWire_Search(OneWireStruct, ONEWIRE_CMD_SEARCHROM)) {
		/* Check if same device found */
		rslt = 1;
		for (i = 0; i < 8; i++) {
			if (rom_backup[i] != OneWireStruct->ROM_NO[i]) {
				rslt = 1;
				break;
			}
		}
	} else {
		rslt = 0;
	}

	/* Restore the search state */
	for (i = 0; i < 8; i++) {
		OneWireStruct->ROM_NO[i] = rom_backup[i];
	}
	OneWireStruct->LastDiscrepancy = ld_backup;
	OneWireStruct->LastDeviceFlag = ldf_backup;
	OneWireStruct->LastFamilyDiscrepancy = lfd_backup;

	/* Return the result of the verify */
	return rslt;
}

void OneWire_TargetSetup(OneWire_t* OneWireStruct, uint8_t family_code) {
   uint8_t i;

	/* Set the search state to find SearchFamily type devices */
	OneWireStruct->ROM_NO[0] = family_code;
	for (i = 1; i < 8; i++) {
		OneWireStruct->ROM_NO[i] = 0;
	}
	
	OneWireStruct->LastDiscrepancy = 64;
	OneWireStruct->LastFamilyDiscrepancy = 0;
	OneWireStruct->LastDeviceFlag = 0;
}

void OneWire_FamilySkipSetup(OneWire_t* OneWireStruct) {
	/* Set the Last discrepancy to last family discrepancy */
	OneWireStruct->LastDiscrepancy = OneWireStruct->LastFamilyDiscrepancy;
	OneWireStruct->LastFamilyDiscrepancy = 0;

	/* Check for end of list */
	if (OneWireStruct->LastDiscrepancy == 0) {
		OneWireStruct->LastDeviceFlag = 1;
	}
}

uint8_t OneWire_GetROM(OneWire_t* OneWireStruct, uint8_t index) {
	return OneWireStruct->ROM_NO[index];
}

void OneWire_Select(OneWire_t* OneWireStruct, uint8_t* addr) {
	uint8_t i;
	OneWire_WriteByte(OneWireStruct, ONEWIRE_CMD_MATCHROM);
	
	for (i = 0; i < 8; i++) {
		OneWire_WriteByte(OneWireStruct, *(addr + i));
	}
}

void OneWire_SelectWithPointer(OneWire_t* OneWireStruct, uint8_t *ROM) {
	uint8_t i;
	OneWire_WriteByte(OneWireStruct, ONEWIRE_CMD_MATCHROM);
	
	for (i = 0; i < 8; i++) {
		OneWire_WriteByte(OneWireStruct, *(ROM + i));
	}	
}

void OneWire_GetFullROM(OneWire_t* OneWireStruct, uint8_t *firstIndex) {
	uint8_t i;
	for (i = 0; i < 8; i++) {
		*(firstIndex + i) = OneWireStruct->ROM_NO[i];
	}
}

uint8_t OneWire_CRC8(uint8_t *addr, uint8_t len) {
	uint8_t crc = 0, inbyte, i, mix;
	
	while (len--) {
		inbyte = *addr++;
		for (i = 8; i; i--) {
			mix = (crc ^ inbyte) & 0x01;
			crc >>= 1;
			if (mix) {
				crc ^= 0x8C;
			}
			inbyte >>= 1;
		}
	}
	
	/* Return calculated CRC */
	return crc;
}
