#include "disco.h"

/* Button pressed status for onpressed/onreleased events */
static volatile uint8_t INT_DISCO_ButtonPressed = 0;

/* Check for functions use */
#if defined(DISCO_USE_FUNCTIONS)

/* Led structure */
typedef struct {
	GPIO_TypeDef* Port;
	uint16_t Pin;
} DISCO_Led_t;

/* Create variable with all leds */
static DISCO_Led_t DISCO_Leds[] = {
	{
#if defined(DISCO_LED_GREEN_PORT)
		DISCO_LED_GREEN_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_GREEN,
	},
	{
#if defined(DISCO_LED_RED_PORT)
		DISCO_LED_RED_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_RED,
	},
	{
#if defined(DISCO_LED_ORANGE_PORT)
		DISCO_LED_ORANGE_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_ORANGE,
	},
	{
#if defined(DISCO_LED_BLUE_PORT)
		DISCO_LED_BLUE_PORT,
#else
		DISCO_LED_PORT,
#endif
		LED_BLUE,
	}
};

#endif

void DISCO_LedInit(void) {
#if defined(DISCO_USE_FUNCTIONS)
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
			/* Set pin as output */
			GPIO_Init(DISCO_Leds[i].Port, DISCO_Leds[i].Pin, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
		}
	}
#else
	/* Set pins as output */
	GPIO_Init(DISCO_LED_PORT, LED_ALL, GPIO_Mode_OUT, GPIO_OType_PP, GPIO_PuPd_NOPULL, GPIO_Speed_High);
#endif
	
	/* Turn leds off */
	DISCO_LedOff(LED_ALL);
}

void DISCO_ButtonInit(void) {
	/* Set pin as input */
	GPIO_Init(DISCO_BUTTON_PORT, DISCO_BUTTON_PIN, GPIO_Mode_IN, GPIO_OType_PP, DISCO_BUTTON_PULL, GPIO_Speed_Low);
}

uint8_t DISCO_ButtonOnPressed(void) {
	/* If button is now pressed, but was not already pressed */
	if (DISCO_ButtonPressed()) {
		if (!INT_DISCO_ButtonPressed) {
			/* Set flag */
			INT_DISCO_ButtonPressed = 1;
			
			/* Return button onpressed */
			return 1;
		}
	} else {
		/* Clear flag */
		INT_DISCO_ButtonPressed = 0;
	}
	
	/* Button is not pressed or it was already pressed before */
	return 0;
}

uint8_t DISCO_ButtonOnReleased(void) {
	/* If button is now released, but was not already released */
	if (!DISCO_ButtonPressed()) {
		if (INT_DISCO_ButtonPressed) {
			/* Set flag */
			INT_DISCO_ButtonPressed = 0;
			
			/* Return button onreleased */
			return 1;
		}
	} else {
		/* Set flag */
		INT_DISCO_ButtonPressed = 1;
	}
	
	/* Button is not released or it was already released before */
	return 0;
}

/* Check for functions use */
#if defined(DISCO_USE_FUNCTIONS)

void DISCO_LedToggle(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
			GPIO_TogglePinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
		}
	}
}

void DISCO_LedOn(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
#if defined(DISCO_SWAP_LOGIC)
			GPIO_SetPinLow(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			GPIO_SetPinHigh(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
}

void DISCO_LedOff(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
#if defined(DISCO_SWAP_LOGIC)
			GPIO_SetPinHigh(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			GPIO_SetPinLow(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
}

uint16_t DISCO_LedIsOn(uint16_t led) {
	uint8_t i;
	
	/* Go through all leds */
	for (i = 0; i < 4; i++) {
		/* Check for LED */
		if (led & DISCO_Leds[i].Pin) {
			/* Check first LED */
#if defined(DISCO_SWAP_LOGIC)
			return !GPIO_GetOutputPinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#else
			return GPIO_GetOutputPinValue(DISCO_Leds[i].Port, DISCO_Leds[i].Pin);
#endif
		}
	}
	
	/* Led not valid */
	return 0;
}

#endif
