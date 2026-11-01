 /** 
 **************************************************************
 * File: repo/mylib/rcm_lta1000g.h
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: The LED Bar Display Register Driver controls the 10 Segment LED Bar.
 * REFERENCE: LTA1000G LED Bar datasheet, CSSE3010 Blackboard
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************
 * lta1000g_seg_set()             - Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin.  
 * rcm_reg_lta1000g_init()   - Initialise LED Bar GPIO pins as outputs
 * rcm_reg_lta1000g_write()  - Write the LED Bar segments high or low.
 * rcm_reg_lta1000g_brotate()- Perform a left or right bitwise rotate on the value (using only the 10 lower bits).
 *************************************************************** 
 */
#ifndef RCM_LTA1000G_H
#define RCM_LTA1000G_H

#include "board.h"
#include "processor_hal.h"
#include <stm32f429xx.h>
#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#define VALUE 0x03FF
#define MASK1 0x0F

typedef enum {
    SIX = 6,
    FIFTEEN = 15,
    TEN = 10,
    THIRTEEN = 13,
    ELEVEN = 11,
    ZERO = 0,
    EIGHT = 8,
    FIVE = 5,
} LedPin;

typedef struct {
    uint8_t ltaValue;
} lta_t;

extern QueueHandle_t ltaQueue;

/**
 * @brief Set the value (0 or 1) for the corresponding LED Bar segment GPIO pin.
 *
 * This is an internal helper function that directly controls a specific segment
 * on the LTA1000G LED Bar by setting or clearing its corresponding GPIO pin.
 *
 * @param segment The index of the segment to be set or cleared (0-9).
 * @param value   The state to set for the segment: 0 for low (off), 1 for high (on).
 *
 * @return None.
 */
void lta1000g_seg_set (int segment, unsigned char value);

/**
 * @brief Perform a left or right bitwise rotation on the given value 
 *        (using only the lower 10 bits) and update the LED Bar display.
 *
 * This function rotates the bits of @p value either to the left or right
 * by one position. The rotation is restricted to the 10 least significant bits.
 * After rotation, the updated value is written to the LED Bar display.
 *
 * @param value     The 10-bit value to be rotated.
 * @param direction Rotation direction: 0 for left, 1 for right.
 *
 * @return The newly rotated 10-bit value.
 */
unsigned short rcm_reg_lta1000g_brotate (unsigned short value, int direction);

/**
 * @brief Initialise all LED Bar GPIO pins as outputs.
 *
 * Configures the GPIO pins connected to the LTA1000G LED Bar so they can
 * be controlled as outputs. This function must be called before attempting
 * to write to the LED Bar.
 *
 * @param None.
 * @return None.
 */
void rcm_reg_lta1000g_init ();

/**
 * @brief Write a 10-bit value to the LED Bar segments.
 *
 * Sets each LED Bar segment high or low based on the corresponding bit
 * in @p value. Bit 0 controls segment 0, bit 1 controls segment 1, etc.
 * 
 * Uses the lta1000g_seg_set() function internally to control each segment.
 *
 * @param value The 10-bit value to write to the LED Bar segments.
 *
 * @return None.
 */
void rcm_reg_lta1000g_write (unsigned short value);

void rcm_task_lta1000g_init(void);

void rcm_task_lta1000g(void *arg);

#endif