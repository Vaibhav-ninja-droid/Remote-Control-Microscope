 /** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_pb.h
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: 
 * REFERENCE: CSSE3010 Blackboard
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * rcm_reg_mfs_pb_init ();
 * rcm_reg_mfs_pb_signal_get (int pb_select);
 *************************************************************** 
 */

#ifndef RCM_MFS_PB_H
#define RCM_MFS_PB_H

#include "board.h"
#include "processor_hal.h"
#include <stm32f429xx.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#define RCM_REG_MFA_PB_S1 1 << 0 
#define RCM_REG_MFA_PB_S2 1 << 1 
#define RCM_REG_MFA_PB_S3 1 << 2 

void rcmTaskMfsInit(void);           // create the controlling task

SemaphoreHandle_t rcmMfsS1Semaphore(void); // expose S1 semaphore
SemaphoreHandle_t rcmMfsS3Semaphore(void); // expose S3 semaphore

/**
 * @brief Initialise MFS pushbutton GPIO pins as inputs with pull-up resistors.
 *
 * This function enables the required GPIO clocks and configures the
 * pushbutton pins for the MFS board:
 * - S1: PC0
 * - S2: PC3
 * - S3: PF3
 *
 * Each pin is set as an input and configured with an internal pull-up resistor,
 * allowing the pushbutton to be read as active-low (pressed = 0, released = 1).
 *
 * @return None.
 */
void rcm_reg_mfs_pb_init ();

/**
 * @brief Read the state of one or more MFS pushbuttons.
 *
 * Checks the input data registers for the selected pushbuttons and returns
 * their states in a bitmask. Each pushbutton is active-low, meaning a pressed
 * button returns 0 from the GPIO register, which is then mapped to a bit flag:
 *
 * - Bit 0: S1 (PC0)
 * - Bit 1: S2 (PC3)
 * - Bit 2: S3 (PF3)
 *
 * @param pb_select Bitmask of pushbuttons to read. Use:
 *        - RCM_REG_MFA_PB_S1
 *        - RCM_REG_MFA_PB_S2
 *        - RCM_REG_MFA_PB_S3
 *        Can be OR'd together to read multiple buttons.
 *
 * @return Bitmask indicating pressed buttons (1 = pressed, 0 = not pressed).
 */
unsigned int rcm_reg_mfs_pb_signal_get (int pb_select);

#endif