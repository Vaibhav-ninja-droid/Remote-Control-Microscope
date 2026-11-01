 /** 
 **************************************************************
 * File: repo/mylib/rcm_switchback.h
 * Author: Vaibhav Rana
 * Date: 6/09/2025
 * Brief: The switchbank Register Driver enables the eight sliding switch inputs. The switch bank is
 * polled and does not use interrupts. You must directly access and manipulate the GPIO registers.
 * REFERENCE:
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_switchbank_init() 
 * rcm_reg_switchbank_read() 
 *************************************************************** 
 */

#ifndef RCM_SWITCHBANK_H
#define RCM_SWITCHBANK_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>
#include "FreeRTOS.h"
#include "queue.h"

#define SWITCH_POLL_MS 10
#define MASKFORSWITCH 0xFF

typedef struct {
    uint8_t switchValue;
} switch_t;

extern QueueHandle_t switchQueue;

void rcm_reg_switchbank_init(void);

uint16_t rcm_reg_switchbank_read(void);

void rcm_task_switch_init(void);

void rcm_task_switch(void * arg);

#endif