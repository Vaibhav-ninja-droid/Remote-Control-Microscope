 /** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_led.c
 * Author: Vaibhav Rana
 * Date: 6/09/2025
 * Brief: The MFS LED register driver controls the MFS LEDs D1 to D4 (See the MFS Lab on black-
 * board). There should only be one external function implemented for initialisation. No other
 * external or internal functions should be implemented.
 * REFERENCE:
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_mfs_led_init() 
 *************************************************************** 
 */

#include "rcm_mfs_led.h"

/**
 * Set up the GPIO pins for the MFS LEDs (D1-D4).
 * Must be called once before using the LEDs in main.
 */
void rcm_reg_mfs_led_init() {
    // Enable GPIO clock
    __GPIOA_CLK_ENABLE();
    __GPIOD_CLK_ENABLE();

    // MFS led
    GPIOA->MODER &= ~(0x03 << (LED_A5_PIN * 2) | 0x03 << (LED_A6_PIN * 2) | 0x03 << (LED_A7_PIN * 2));
    GPIOA->MODER |= (0x01 << (LED_A5_PIN * 2) | 0x01 << (LED_A6_PIN * 2) | 0x01 << (LED_A7_PIN * 2));
    GPIOA->OTYPER &= ~(0x01 << LED_A5_PIN | 0x01 << LED_A6_PIN | 0x01 << LED_A7_PIN);
    GPIOA->PUPDR  &= ~(0x03 << (LED_A5_PIN * 2) | 0x03 << (LED_A6_PIN * 2) | 0x03 << (LED_A7_PIN * 2));
    GPIOA->ODR |= (0x01 << LED_A5_PIN | 0x01 << LED_A6_PIN | 0x01 << LED_A7_PIN);

    GPIOD->MODER &= ~(0x03 << (LED_D14_PIN * 2));
    GPIOD->MODER |= (0x01 << (LED_D14_PIN * 2));
    GPIOD->OTYPER &= ~(0x01 << LED_D14_PIN);
    GPIOD->PUPDR  &= ~(0x03 << (LED_D14_PIN * 2));
    GPIOD->ODR |= (0x01 << LED_D14_PIN);

}