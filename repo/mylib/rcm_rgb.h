 /** 
 **************************************************************
 * File: repo/mylib/rcm_rgb.h
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

#ifndef RCM_RGB_H
#define RCM_RGB_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#define TIMER_COUNTER_FREQ  				1000000         				//Frequency (in Hz)
#define PWM_PULSE_WIDTH_SECONDS				0.01				    //2s
#define PWM_PULSE_WIDTH_TICKS				20000					//2*((PWM_PULSE_WIDTH_SECONDS)/(1/TIMER_COUNTER_FREQ))		//Period in timer ticks
#define PWM_PERCENT2TICKS_DUTYCYCLE(value)	(uint16_t)(value*PWM_PULSE_WIDTH_TICKS/100)				//Duty Cycle on time in timer ticks
#define PWM_DUTYCYCLE_CHANGE(value) 			TIM1->CCR1=(uint16_t)value									//Change PWM dutycyle

#define YELLOW 0x01
#define RED 0x03
#define GREEN 0x05
#define BRIGHTNESS (1 << 9)

typedef struct {
    uint8_t colour;
} rgb_t;

extern QueueHandle_t rgbQueue;

/**
 * Initialise the RGB LED driver.
 *
 * This sets up the GPIO pins for the red, green, and blue channels,
 * and also configures the PWM module to control the LED's brightness
 * using the common V pin.
 */
void rcm_reg_rgb_init(void);

/**
 * Change how bright the RGB LED glows.
 *
 * Brightness is controlled with PWM on the V pin of the rgb led. The 'level' argument
 * isn't a plain percentage, but a bitmask where each bit means 10% brighter.
 *
 */
void rcm_reg_rgb_brightness_write(unsigned short level);

/**
 * Get the current brightness setting.
 *
 * This looks at the PWM duty cycle and converts it back into the
 * bitmask style used by rcm_reg_rgb_brightness_write().
 * Opposite of what rcm_reg_rgb_brightness_write() do.
 *
 * @return A brightness mask.
 */
uint16_t rcm_reg_rgb_brightness_read(void);

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
void rcm_reg_rgb_colour_set(unsigned char rgb_mask);

void rcm_task_rgb_init(void);

void rcm_task_rgb(void *arg);

#endif