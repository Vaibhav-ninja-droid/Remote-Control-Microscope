/** 
 **************************************************************
 * File: repo/mylib/rcm_rcmsys.c
 * Author: Vaibhav Rana
 * Date: 12/10/2025
 * Brief: The RCM system is used to manipulate the RCM's location and control the zoom and April
 * tag detection. The RCM system must process the command inputs and control the radio
 * transmitter and display output.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_rcmsys_init(void)
 * rcm_rcmsys_on_radio_tx(void)
 * rcm_rcmsys_on_any_pb(void)
 * rcm_rcmsys_opcode(uint8_t opcode)
 *************************************************************** 
 */

#include "rcm_rcmsys.h"

static uint8_t mfsD1On = 0;
static uint8_t mfsD2On = 0;

/**
 * Drive MFS LED D1 on/off directly. 
 */
void setD1(uint8_t on) {
    if (on) {
        RCM_REG_MFS_LED_D1_ON();
    } else {
        RCM_REG_MFS_LED_D1_OFF();
    }
    mfsD1On = on;
}

/**
 * Drive MFS LED D2 on/off directly. 
 */
void setD2(uint8_t on) {
    if (on) {
        RCM_REG_MFS_LED_D2_ON();
    } else {
        RCM_REG_MFS_LED_D2_OFF();
    }
    mfsD2On = on;
}

/**
 * Bring up drivers and tasks.
 */
void rcm_rcmsys_init(void) {
    // Output
    rcm_task_lta1000g_init();
    rcm_reg_mfs_led_init();
    rcm_output_task_init();
    rcm_task_ssd_init();
    rcm_task_rgb_init();

    setD1(0);
    setD2(0);

    //Input
    rcmTaskMfsInit(); // PB task + S1 semaphore
    rcm_task_switch_init();
    rcm_trimpot_task_init();
    rcm_cmdin_task_init();
    rcm_tx_task_init();
}

/**
 * Called after a radio TX to blink D1. 
 */
void rcm_rcmsys_on_radio_tx(void) {
    setD1(!mfsD1On);
}

/** 
 * Called when any PB is pressed to blink D2. 
 */
void rcm_rcmsys_on_any_pb(void) {
    setD2(!mfsD2On);
}

/**
 * Show the current opcode on the LED bar (lower bits).
 */
void rcm_rcmsys_opcode(uint8_t opcode) {
    if (ltaQueue) { 
        lta_t x = { .ltaValue = (uint8_t)(opcode & MASK1) };
        (void)xQueueOverwrite(ltaQueue, &x);
    }
}



