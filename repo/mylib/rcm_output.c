/** 
 **************************************************************
 * File: repo/mylib/rcm_output.c
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

#include "rcm_output.h"

QueueHandle_t outputQueue = NULL;

/**
 * Create the output task.
 */
void rcm_output_task_init(void) {
    xTaskCreate( (void *) &rcm_output_task, "RCM output", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, NULL);
}

/**
 * Output task. Shows numbers on SSD and flashes RGB on increase or decrease of zoom.
 */
void rcm_output_task(void *arg) {
    outputQueue = xQueueCreate(8, sizeof(rcm_output_t));

    rcm_output_t m;
    for (;;) {
        // 1) SSD updates
        if (xQueueReceive(outputQueue, &m, portMAX_DELAY) == pdTRUE) {
            if (MessageQueue) {
                struct Message msg;

                if (m.kind == OUTPUT_ZOOM) {
                    if (m.ssdvalue > 200) {
                        m.ssdvalue = 200;
                    }
                } else {
                    if (m.ssdvalue > 255) {
                        m.ssdvalue = 255;
                    }
                }
                msg.s = (uint8_t)m.ssdvalue;
                (void)xQueueSend(MessageQueue, &msg, 10);
            }
        }

        // 2) RGB colour
        if (m.kind == OUTPUT_ZOOM && rgbQueue) {
            rgb_t c;
            if (m.zoom > 0) {
                c.colour = RED;     // increasing
            } else if (m.zoom < 0) {
                c.colour = GREEN;   // decreasing
            } else {
                c.colour = YELLOW;  // defualt
            }
            (void)xQueueSend(rgbQueue, &c, 0);
        }
    }
}