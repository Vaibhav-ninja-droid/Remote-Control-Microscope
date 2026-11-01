 /** 
 **************************************************************
 * File: repo/mylib/rcm_rgb.c
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: 
 * REFERENCE: CSSE3010 Blackboard
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_rgb_init()
 * rcm_reg_rgb_brightness_write(unsigned short level)
 * rcm_reg_rgb_brightness_read()
 * rcm_reg_rgb_colour_set(unsigned char rgb_mask)
 ***************************************************************
 */

#include "rcm_rgb.h"

QueueHandle_t rgbQueue = NULL;

/**
 * Initialise the RGB LED driver.
 *
 * This sets up the GPIO pins for the red, green, and blue channels,
 * and also configures the PWM module to control the LED's brightness
 * using the common V pin.
 */
void rcm_reg_rgb_init() {
    __GPIOE_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();
	__GPIOB_CLK_ENABLE();
    
    // CN12 GPIO
    GPIOE->MODER &= ~(0x03 << (9 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (9 * 2));   //Set for push pull
    GPIOE->OTYPER &= ~(0x01 << 9);   //Set for push pull

    GPIOE->MODER &= ~(0x03 << (11 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (11 * 2));   //Set for push pull
    GPIOE->OTYPER &= ~(0x01 << 11);   //Set for push pull

    GPIOB->MODER &= ~(0x03 << (14 * 2));  //clear bits
    GPIOB->MODER |= (0x01 << (14 * 2));   //Set for push pull
    GPIOB->OTYPER &= ~(0x01 << 14);   //Set for push pull

    GPIOF->MODER &= ~(0x03 << (15 * 2));  //clear bits
    GPIOF->MODER |= (0x01 << (15 * 2));   //Set for push pull
    GPIOF->OTYPER &= ~(0x01 << 15);   //Set for push pull
    
    // PWM
    GPIOE->OSPEEDR |= (GPIO_SPEED_FAST << (9 * 2));		//Set fast speed.
	GPIOE->PUPDR &= ~(0x03 << (9 * 2));				//Clear bits for no push/pull
	GPIOE->MODER &= ~(0x03 << (9 * 2));				//Clear bits
	GPIOE->MODER |= (GPIO_MODE_AF_PP << (9 * 2));  	//Set Alternate Function Push Pull Mode
	
	GPIOE->AFR[1] &= ~((0x0F) << ((9-8) * 4));			//Clear Alternate Function for pin (lower ARF register)
	GPIOE->AFR[1] |= (GPIO_AF1_TIM1 << ((9-8) * 4));	//Set Alternate Function for pin (lower ARF register)
    
    // Timer 3 clock enable
	__TIM1_CLK_ENABLE();

	/* Compute the prescaler value
	   Set the clock prescaler to 50kHz
	   SystemCoreClock is the system clock frequency */
	TIM1->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM1->CR1 &= ~TIM_CR1_DIR; 

	TIM1->ARR = PWM_PULSE_WIDTH_TICKS; 		//Set pulse period to ~1s 
	TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(50);	//Set pulse width to 0% duty cycle.

	TIM1->CCMR1 &= ~(TIM_CCMR1_OC2M); 	// Clear OC2M (Channel 2) 
	TIM1->CCMR1 |= (0x6 << 4); 		// Enable PWM Mode 1, upcounting, on Channel 2 
	TIM1->CCMR1 |= (TIM_CCMR1_OC2PE); 	// Enable output preload bit for channel 2 
	
	TIM1->CR1  |= (TIM_CR1_ARPE); 	// Set Auto-Reload Preload Enable 
	TIM1->CCER |= TIM_CCER_CC1E; 	// Set CC1E Bit
	TIM1->CCER &= ~TIM_CCER_CC1NE; 	// Clear CC1NE Bit for active high output

	TIM1->CCER &= ~TIM_CCER_CC1P; 	// Polarity of PWM output waveform is active high. 
									// If set, reverses polarity (e.g. high becomes low).
									// See p576 of the STM32F429 reference manual
	
	/* Set Main Output Enable (MOE) bit
	   Set Off-State Selection for Run mode (OSSR) bit
	   Set Off-State Selection for Idle mode (OSSI) bit */
	TIM1->BDTR |= TIM_BDTR_MOE | TIM_BDTR_OSSR | TIM_BDTR_OSSI; 

	TIM1->CR1 |= TIM_CR1_CEN;	// Enable the counter
}

/**
 * Change how bright the RGB LED glows.
 *
 * Brightness is controlled with PWM on the V pin of the rgb led. The 'level' argument
 * isn't a plain percentage, but a bitmask where each bit means 10% brighter.
 *
 */
void rcm_reg_rgb_brightness_write(unsigned short level) {
	int percent = 0;
    
	// Find which bit is set (10% increments)
    switch (level) {
		case (1 << 0):
			percent = 10;
			break;
		case (1 << 1):
			percent = 20;
			break;
		case (1 << 2):
			percent = 30;
			break;
		case (1 << 3):
			percent = 40;
			break;
		case (1 << 4):
			percent = 50;
			break;
		case (1 << 5):
			percent = 60;
			break;	
        case (1 << 6):
			percent = 70;
			break;
		case (1 << 7):
			percent = 80;
			break;
		case (1 << 8):
			percent = 90;
			break;
		case (1 << 9):
			percent = 100;
			break;
		default:
			percent = 0;
			break;
	}

	// Convert % into CCR2 ticks
    TIM1->CCR1 = PWM_PERCENT2TICKS_DUTYCYCLE(percent);
}

/**
 * Get the current brightness setting.
 *
 * This looks at the PWM duty cycle and converts it back into the
 * bitmask style used by rcm_reg_rgb_brightness_write().
 * Opposite of what rcm_reg_rgb_brightness_write() do.
 *
 * @return A brightness mask.
 */
uint16_t rcm_reg_rgb_brightness_read() {
    int dutyPercent = ((TIM1->CCR1) * 100) / PWM_PULSE_WIDTH_TICKS;
    // Convert duty back to bitmask
	int steps = (dutyPercent + 5) / 10;
	
	if (steps < 1) {
		return 0;
	}
	
	if (steps > 10) {
		steps = 10;
	}

	return (1 << (steps - 1)); 
}

/**
 * Pick the LED colour by turning on red, green or blue.
 *
 * According to Table 5:
 * 0x07 = Black (all off)
 * 0x06 = Blue
 * 0x05 = Green
 * 0x04 = Cyan
 * 0x03 = Red
 * 0x02 = Magenta
 * 0x01 = Yellow
 * 0x00 = White (all on)
 */
void rcm_reg_rgb_colour_set(unsigned char rgb_mask) {
	if (rgb_mask & 0x4) {
        GPIOE->ODR |= (0x01 << 11); // red on
    } else {
        GPIOE->ODR &= ~(0x01 << 11); // red off
    }

    if (rgb_mask & 0x2) {
        GPIOB->ODR |= (0x01 << 14); // green on
    } else {
        GPIOB->ODR &= ~(0x01 << 14); // green off
    }

    if (rgb_mask & 0x1) {
        GPIOF->ODR |= (0x01 << 15); // blue on
    } else {
        GPIOF->ODR &= ~(0x01 << 15); // blue off
    }
}

void rcm_task_rgb_init(void) {
    xTaskCreate((void *) &rcm_task_rgb, "rgb", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void rcm_task_rgb(void *arg) {
	rgbQueue = xQueueCreate(8, sizeof(rgb_t));

	rcm_reg_rgb_init();
	rcm_reg_rgb_brightness_write(BRIGHTNESS); // default brightness (max)
	rcm_reg_rgb_colour_set(YELLOW); // default colour

	rgb_t msg;
    for (;;) {
        if (xQueueReceive(rgbQueue, &msg, portMAX_DELAY) == pdTRUE) {
            rcm_reg_rgb_colour_set(msg.colour);
        }
    }
}