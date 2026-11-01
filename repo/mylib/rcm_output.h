/** 
 **************************************************************
 * File: repo/mylib/rcm_output.h
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: SSD and RGB feedback
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_output_task_init(void)
 * rcm_output_task(void *arg)
 *************************************************************** 
 */

#ifndef RCM_OUTPUT_H
#define RCM_OUTPUT_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include "FreeRTOS.h"
#include "queue.h"
#include "task.h"

#include "rcm_rgb.h"
#include "rcm_ssd.h"

typedef enum {
    OUTPUT_X,
    OUTPUT_Y,
    OUTPUT_Z,
    OUTPUT_ZOOM,
    OUTPUT_APRIL
} RcmOutput;

typedef struct {
    RcmOutput kind;
    uint16_t ssdvalue;
    int zoom; 
} rcm_output_t;

extern QueueHandle_t outputQueue;

void rcm_output_task_init(void);

void rcm_output_task(void *arg);


#endif