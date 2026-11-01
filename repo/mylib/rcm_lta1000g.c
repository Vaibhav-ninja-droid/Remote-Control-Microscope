 /** 
 **************************************************************
 * File: repo/mylib/rcm_lta1000g.c
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: The LED Bar Display Register Driver controls the 10 Segment LED Bar.
 * REFERENCE: LTA1000G LED Bar datasheet
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_lta1000g_init()   - Initialise LED Bar GPIO pins as outputs
 * rcm_reg_lta1000g_write()  - Write the LED Bar segments high or low.
 * rcm_reg_lta1000g_brotate()- Perform a left or right bitwise rotate on the value (using only the 10 lower bits).
 *************************************************************** 
 */

#include "rcm_lta1000g.h"

QueueHandle_t ltaQueue = NULL;

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
void rcm_reg_lta1000g_init() {
    // Enable GPIO clock
    __GPIOE_CLK_ENABLE();
    __GPIOG_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();

    // CN11
    GPIOE->MODER &= ~(0x03 << (6 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (6 * 2));   //Set for push pull
    GPIOE->OTYPER &= ~(1 << 6);   //Set for push pull
    GPIOE->ODR &= ~(1 << 6);

    GPIOG->MODER &= ~(0x03 << (15 * 2) | 0x03 << (10 * 2) | 0x03 << (13 * 2) | 0x03 << (11 * 2));  //clear bits
    GPIOG->MODER |= (0x01 << (15 * 2) | 0x01 << (10 * 2) | 0x01 << (13 * 2) | 0x01 << (11 * 2));   //Set for push pull
    GPIOG->OTYPER &= ~(1 <<15 | 1 << 10 | 1 << 13 | 1 << 11);   //Set for push pull
    GPIOG->ODR &= ~(1 << 15 | 1 << 10 | 1 << 13 | 1 << 11);

    // CN12
    GPIOF->MODER &= ~(0x03 << (11 * 2));  //clear bits
    GPIOF->MODER |= (0x01 << (11 * 2));   //Set for push pull
    GPIOF->OTYPER &= ~(1 << 11);   //Set for push pull
    GPIOF->ODR &= ~(1 << 11);

    GPIOE->MODER &= ~(0x03 << (0 * 2));  //clear bits
    GPIOE->MODER |= (0x01 << (0 * 2));   //Set for push pull
    GPIOE->OTYPER &= ~(1 << 0);   //Set for push pull
    GPIOE->ODR &= ~(1 << 0);

    GPIOG->MODER &= ~(0x03 << (8 * 2) | 0x03 << (5 * 2) | 0x03 << (6 * 2));  //clear bits
    GPIOG->MODER |= (0x01 << (8 * 2) | 0x01 << (5 * 2) | 0x01 << (6 * 2));   //Set for push pull
    GPIOG->OTYPER &= ~(1 << 8 | 1 << 5 | 1 << 6);   //Set for push pull
    GPIOG->ODR &= ~(1 << 8 | 1 << 5 | 1 << 6);
    
}

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
void lta1000g_seg_set (int segment, unsigned char value) {
    switch (segment) {
        case 0: // PE6
            if (value) {
                GPIOE->ODR |= (1 << 6);
            }
            else {
                GPIOE->ODR &= ~(1 << 6);
            }
            break;

        case 1: // PG15
            if (value) {
                GPIOG->ODR |= (1 << 15);
            }
            else {
                GPIOG->ODR &= ~(1 << 15);
            }
            break;

        case 2: // PG10
            if (value) {
                GPIOG->ODR |= (1 << 10);
            }
            else {
                GPIOG->ODR &= ~(1 << 10);
            }
            break;

        case 3: // PG13
            if (value) {
                GPIOG->ODR |= (1 << 13);
            }
            else {
                GPIOG->ODR &= ~(1 << 13);
            }
            break;

        case 4: // PG11
            if (value) {
                GPIOG->ODR |= (1 << 11);
            }
            else {
                GPIOG->ODR &= ~(1 << 11);
            }
            break;

        case 5: // PF11
            if (value) {
                GPIOF->ODR |= (1 << 11);
            }
            else {
                GPIOF->ODR &= ~(1 << 11);
            }
            break;

        case 6: // PE0
            if (value) {
                GPIOE->ODR |= (1 << 0);
            }
            else {
                GPIOE->ODR &= ~(1 << 0);
            }
            break;

        case 7: // PG8
            if (value) {
                GPIOG->ODR |= (1 << 8);
            }
            else {
                GPIOG->ODR &= ~(1 << 8);
            }
            break;

        case 8: // PG5
            if (value) {
                GPIOG->ODR |= (1 << 5);
            }
            else {
                GPIOG->ODR &= ~(1 << 5);
            }
            break;

        case 9: // PG6
            if (value) {
                GPIOG->ODR |= (1 << 6);
            }
            else {
                GPIOG->ODR &= ~(1 << 6);
            }
            break;

        default:
            // Invalid segment index — do nothing
            break;
    }
}

/**
 * @brief Write a 10-bit value to the LED Bar segments.
 *
 * Sets each LED Bar segment high or low based on the corresponding bit
 * in @p value. Bit 0 controls segment 0, bit 1 controls segment 1, etc.
 * 
 * Uses the lta1000g_seg_set() function internally to control each segment.
 *
 * @note Only the lower 10 bits of @p value are used.
 *
 * @param value The 10-bit value to write to the LED Bar segments.
 *
 * @return None.
 */
void rcm_reg_lta1000g_write (unsigned short value) {
    for (int i = 0; i < 10; i++) {
        lta1000g_seg_set(i, (value >> i) & 0x01);
    }
}

/**
 * @brief Perform a left or right bitwise rotation on the given value 
 *        (using only the lower 10 bits) and update the LED Bar display.
 *
 * This function rotates the bits of @p value either to the left or right
 * by one position. The rotation is restricted to the 10 least significant bits.
 * After rotation, the updated value is written to the LED Bar display.
 *
 * @note 0 = left rotate, 1 = right rotate.
 *
 * @param value     The 10-bit value to be rotated.
 * @param direction Rotation direction: 0 for left, 1 for right.
 *
 * @return The newly rotated 10-bit value.
 */
unsigned short rcm_reg_lta1000g_brotate (unsigned short value, int direction) {
    value &= VALUE; // Keep only lower 10 bits

    if (direction == 0) {
        // Left rotate
        unsigned short msb = (value & 0x0200) >> 9;
        value = ((value << 1) & 0x03FF) | msb;
    } else {
        // Right rotate
        unsigned short lsb = value & 0x0001;
        value = (value >> 1) | (lsb << 9);
    }

    rcm_reg_lta1000g_write(value);
    return value;
}

void rcm_task_lta1000g_init(void) {
    xTaskCreate( (void *) &rcm_task_lta1000g, "lta1000g led bar", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void rcm_task_lta1000g(void *arg) {
    rcm_reg_lta1000g_init();
    ltaQueue = xQueueCreate(1, sizeof(lta_t));

    if (ltaQueue) {
        lta_t init_msg = { .ltaValue = (uint8_t)(0x01 & MASK1) }; // default = IDLE
        (void)xQueueOverwrite(ltaQueue, &init_msg);
    }

    lta_t msg;
    for (;;) {
        if (xQueueReceive(ltaQueue, &msg, portMAX_DELAY) == pdTRUE) {
            rcm_reg_lta1000g_write((unsigned short)(msg.ltaValue & MASK1));
        }
    }
}