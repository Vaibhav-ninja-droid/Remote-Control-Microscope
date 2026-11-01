/** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_trimpot.h
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: 
 * REFERENCE: CSSE3010 Blackboard
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_mfs_trimpot_init()
 * rcm_reg_mfs_trimpot_read()
 *************************************************************** 
 */

#ifndef RCM_MFS_TRIMPOT_H
#define RCM_MFS_TRIMPOT_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>
#include <stdbool.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define TRIM_FULL_TURN_DELTA 240
#define TRIM_DEADBAND        64
#define TRIM_POLL_MS         10

typedef struct {
    int8_t  dir;    // +1 clockwise turn, -1 anticlockwise turn
    uint8_t turns;  // number of full turns since last event
} trimpot_t;

extern QueueHandle_t trimpotQueue;

/**
 * Initialise the Trimpot input driver (GPIO and ADC).
 *
 * Configures the GPIO pin connected to the trimpot as an analog input.
 * Sets up the ADC module for single-ended conversion of the trimpot voltage.
 */
void rcm_reg_mfs_trimpot_init(void);

/**
 * Read the current trimpot input value.
 *
 * Performs an ADC conversion of the trimpot analog voltage and returns
 * the raw ADC value. The range depends on the ADC resolution 
 * Example: 0-4095 for a 12-bit ADC
 *
 * @return uint16_t Raw ADC value representing the trimpot input voltage.
 */
uint16_t rcm_reg_mfs_trimpot_read(void);

void rcm_trimpot_task_init(void);

void rcm_trimpot_task(void *arg);

#endif