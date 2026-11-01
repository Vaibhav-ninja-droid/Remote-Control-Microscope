 /** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_led.h
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

#ifndef RCM_MFS_LED_H
#define RCM_MFS_LED_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>

#define LED_A5_PIN 5
#define LED_A6_PIN 6
#define LED_A7_PIN 7
#define LED_D14_PIN 14

#define RCM_REG_MFS_LED_D1_ON()   (GPIOA->ODR &= ~(0x01 << LED_A5_PIN))
#define RCM_REG_MFS_LED_D1_OFF()  (GPIOA->ODR |= (0x01 << LED_A5_PIN))
#define RCM_REG_MFS_LED_D2_ON()   (GPIOA->ODR &= ~(0x01 << LED_A6_PIN))
#define RCM_REG_MFS_LED_D2_OFF()  (GPIOA->ODR |= (0x01 << LED_A6_PIN))
#define RCM_REG_MFS_LED_D3_ON()   (GPIOA->ODR &= ~(0x01 << LED_A7_PIN))
#define RCM_REG_MFS_LED_D3_OFF()  (GPIOA->ODR |= (0x01 << LED_A7_PIN))
#define RCM_REG_MFS_LED_D4_ON()   (GPIOD->ODR &= ~(0x01 << LED_D14_PIN))
#define RCM_REG_MFS_LED_D4_OFF()  (GPIOD->ODR |= (0x01 << LED_D14_PIN))

void rcm_reg_mfs_led_init();

#endif