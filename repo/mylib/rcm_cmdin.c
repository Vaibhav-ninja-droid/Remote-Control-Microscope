/** 
 **************************************************************
 * File: repo/mylib/rcm_cmdin.c
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: Command input task. S1 cycles fields, S3 applies current
 *        switch value, trimpot adjusts zoom.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_cmdin_task_init(void)
 * rcm_cmdin_task(void *arg)
 *************************************************************** 
 */

#include "rcm_cmdin.h"

cmdState CurrentState = S_IDLE;

uint16_t X = 0;
uint16_t Y = 0;
uint16_t Z = 0;

bool April = false;

static uint16_t zoomValue = 0; // start default

/**
 * Simple state machine that changes the currect state to the next one.
 */
cmdState next_state(cmdState s) { 
    switch (s) {
        case S_IDLE:  
            return S_X;
        case S_X:     
            return S_Y;
        case S_Y:     
            return S_Z;
        case S_Z:     
            return S_ZOOM;
        case S_ZOOM:  
            return S_APRIL;
        case S_APRIL: 
            return S_ORG;
        case S_ORG:   
            return S_JOIN;
        case S_JOIN:  
            return S_IDLE;
        default:      
            return S_IDLE;
    }
}

/**
 * Map state to its display opcode (for lta1000g LED bar).
 */
OpCode state_opcode(cmdState s) {
    switch (s) {
        case S_IDLE:  
            return OPCODE_IDLE;
        case S_X:     
            return OPCODE_X;
        case S_Y:     
            return OPCODE_Y;
        case S_Z:     
            return OPCODE_Z;
        case S_ZOOM:  
            return OPCODE_ZOOM;
        case S_APRIL: 
            return OPCODE_APRIL;
        case S_ORG:   
            return OPCODE_ORG;
        case S_JOIN:  
            return OPCODE_JOIN;
        default:      
            return OPCODE_IDLE;
    }
}

/**
 * Execute the action for the current state.
 * @param value Latest 8-bit switchbank value (0 to 255).
 */
void exec_current(uint8_t value) {
    rcm_packet_t pkt;
    switch (CurrentState) {
        case S_X: 
            uint8_t x;
            if (value > 200) {
                x = 200;
            } else {
                x = value;
            }
            X = x;
            rcm_rcmsys_opcode(OPCODE_X);
            rcm_rcm_packet_xyz(X, Y, Z, &pkt);

            if (outputQueue) {
                rcm_output_t m = { .kind = OUTPUT_X, .ssdvalue = X, .zoom = 0 };
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_Y:
            uint8_t y;
            if (value > 200) {
                y = 200;
            } else {
                y = value;
            }
            Y = y;
            rcm_rcmsys_opcode(OPCODE_Y);
            rcm_rcm_packet_xyz(X, Y, Z, &pkt);

            if (outputQueue) {
                rcm_output_t m = { .kind = OUTPUT_Y, .ssdvalue = Y, .zoom = 0 };
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_Z:     
            uint8_t z;
            if (value > 99) {
                z = 99;
            } else {
                z = value;
            }
            Z = z;
            rcm_rcmsys_opcode(OPCODE_Z);
            rcm_rcm_packet_xyz(X, Y, Z, &pkt);

            if (outputQueue) {
                rcm_output_t m = { .kind = OUTPUT_Z, .ssdvalue = Z, .zoom = 0 };
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_ZOOM: 
            rcm_rcmsys_opcode(OPCODE_ZOOM);
            rcm_rcm_packet_zoom(zoomValue, &pkt);
            
            if (outputQueue) {
                rcm_output_t m = { .kind = OUTPUT_ZOOM, .ssdvalue = zoomValue, .zoom = 0 };
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_APRIL: // Toggle APRIL ON/OFF
            April = !April;
            rcm_rcmsys_opcode(OPCODE_APRIL);
            rcm_rcm_packet_april(April, &pkt);

            if (outputQueue) {
                rcm_output_t m = { .kind = OUTPUT_APRIL, .ssdvalue = April, .zoom = 0 };
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_ORG:   // Reset everything to defaults
            rcm_rcmsys_opcode(OPCODE_ORG);
            X = 0;
            Y = 0;
            Z = 0;
            rcm_rcm_packet_xyz(0, 0, 0, &pkt);
            zoomValue = 0;
            rcm_rcm_packet_zoom(0, &pkt);  // default zoom
            April = false;
            rcm_rcm_packet_april(false, &pkt);

            if (outputQueue) {
                rcm_output_t m;
                m = (rcm_output_t){ .kind = OUTPUT_Z, .ssdvalue = 0, .zoom = 0 }; 
                (void)xQueueSend(outputQueue, &m, 10);
                m = (rcm_output_t){ .kind = OUTPUT_Y, .ssdvalue = 0, .zoom = 0 }; 
                (void)xQueueSend(outputQueue, &m, 10);
                m = (rcm_output_t){ .kind = OUTPUT_X, .ssdvalue = 0, .zoom = 0 }; 
                (void)xQueueSend(outputQueue, &m, 10);
                m = (rcm_output_t){ .kind = OUTPUT_ZOOM, .ssdvalue = 0, .zoom = 0 }; 
                (void)xQueueSend(outputQueue, &m, 10);
                m = (rcm_output_t){ .kind = OUTPUT_APRIL, .ssdvalue = 0, .zoom = 0 }; 
                (void)xQueueSend(outputQueue, &m, 10);
            }
            break;
        case S_JOIN:  
            rcm_rcmsys_opcode(OPCODE_JOIN);
            rcm_rcm_packet_join(&pkt);
            break;
        default:      
            break;
    }
}

/**
 * Create the command input task and start PB/switch helpers.
 */
void rcm_cmdin_task_init(void) {
    xTaskCreate( (void *) &rcm_cmdin_task, "cmdin", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 3, NULL);
}

/**
 * Read PBs/Switches/Trimpot and drive packets.
 */
void rcm_cmdin_task(void *arg) {
    static TickType_t lastS1Time = 0;
    static TickType_t lastS3Time = 0;

    uint8_t lastSwitch = 0; // latest switch value

    // present opcode on LED bar at boot
    rcm_rcmsys_opcode(state_opcode(CurrentState));
    for (;;) {
        TickType_t now = xTaskGetTickCount();

        switch_t sm;
        if (switchQueue && xQueueReceive(switchQueue, &sm, 0) == pdTRUE) {
            lastSwitch = sm.switchValue;
        }

        if (xSemaphoreTake(rcmMfsS1Semaphore(), 0) == pdTRUE) {
            if ((now - lastS1Time) > DELAY) { // simple debouncing
                lastS1Time = now;

                rcm_rcmsys_on_any_pb(); // D2 toggle (in rcmsys)
                CurrentState = next_state(CurrentState); // rotate selection
                rcm_rcmsys_opcode(state_opcode(CurrentState));
            }
        }

        if (xSemaphoreTake(rcmMfsS3Semaphore(), 0) == pdTRUE) {
            if ((now - lastS3Time) > DELAY) { // simple debouncing
                lastS3Time = now;

                rcm_rcmsys_on_any_pb(); // D2 toggle (in rcmsys)
                exec_current(lastSwitch);
            }
        }

        // Handle trimpot (only affect ZOOM state)
        trimpot_t event;
        while (trimpotQueue && xQueueReceive(trimpotQueue, &event, 0) == pdTRUE) {
            if (CurrentState == S_ZOOM) {
                int delta;
                if (event.dir > 0) {
                    delta = event.turns * ZOOM_STEP_PERCENT;
                } else {
                    delta = -event.turns * ZOOM_STEP_PERCENT;
                }

                int newZoom = (int)zoomValue + delta;
                if (newZoom < ZOOM_MIN) {
                    newZoom = ZOOM_MIN;
                }
                if (newZoom > ZOOM_MAX) {
                    newZoom = ZOOM_MAX;
                }

                zoomValue = (uint16_t)newZoom;

                rcm_rcmsys_opcode(OPCODE_ZOOM);
                rcm_packet_t pkt;
                rcm_rcm_packet_zoom(zoomValue, &pkt);

                if (outputQueue) {
                    rcm_output_t m;
                    m.kind = OUTPUT_ZOOM;
                    m.ssdvalue = zoomValue;

                    if (event.dir > 0) {
                        m.zoom = +1;
                    } else {
                        m.zoom = -1;
                    }
                    (void)xQueueSend(outputQueue, &m, 10);
                }
            }
        }

        vTaskDelay(POLL_MS);
    }
}