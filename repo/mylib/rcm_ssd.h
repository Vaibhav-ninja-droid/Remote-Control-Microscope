/** 
 **************************************************************
 * File: repo/mylib/rcm_ssd.h
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: 7-Segment Display(SSD)
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_ssd_init(void)
 * rcm_reg_ssd_set(uint8_t digitNum, uint8_t number)
 * rcm_task_ssd_init(void)
 * rcm_task_ssd(void)
 *************************************************************** 
 */

#ifndef RCM_SSD_H
#define RCM_SSD_H

#include "board.h"
#include "processor_hal.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

#define SERIAL_DATA_PIN 12
#define SRCLK_PIN 13
#define RCLK_PIN 14

#define TIMER_COUNTER_FREQ_2 50000           //Frequency (in Hz)
#define TIMER_1SECOND_PERIOD 50       //Period for 1s (in count ticks)

struct Message {
	uint8_t s;
};

extern QueueHandle_t MessageQueue;

void rcm_reg_ssd_init(void);

void rcm_reg_ssd_set(uint8_t digitNum, uint8_t number);

void rcm_task_ssd_init(void);

void rcm_task_ssd(void);

#endif