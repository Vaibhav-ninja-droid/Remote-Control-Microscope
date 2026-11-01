 /** 
 **************************************************************
 * File: repo/mylib/rcm_switchbank.c
 * Author: Vaibhav Rana
 * Date: 6/09/2025
 * Brief: The switchbank Register Driver enables the eight sliding switch inputs. The switch bank is
 * polled and does not use interrupts. You must directly access and manipulate the GPIO registers.
 * REFERENCE: https://freertos.org/Documentation/02-Kernel/04-API-references/06-Queues/11-xQueueOverwrite
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_switchbank_init() 
 * rcm_reg_switchbank_read() 
 *************************************************************** 
 */

#include "rcm_switchbank.h"

#define SW0_PIN  8
#define SW1_PIN  7
#define SW2_PIN  10
#define SW3_PIN  12
#define SW4_PIN  14
#define SW5_PIN  15
#define SW6_PIN  10
#define SW7_PIN  11
#define SW_ALT_PIN 15

#define SWITCHBANK_MASK  0xFF

typedef enum {
    SWITCH_0 = 0,
    SWITCH_1 = 1,
    SWITCH_2 = 2,
    SWITCH_3 = 3,
    SWITCH_4 = 4,
    SWITCH_5 = 5,
    SWITCH_6 = 6,
    SWITCH_7 = 7
} SwitchBank_t;

QueueHandle_t switchQueue = NULL;

/**
 * Initialise all switchbank pins as inputs.
 * Configures the GPIO for the 8 slide switches.
 */
void rcm_reg_switchbank_init() {
    // Enable GPIO clock
    __GPIOE_CLK_ENABLE();
    __GPIOB_CLK_ENABLE();

    GPIOE->MODER &= ~(0x3 << (SW0_PIN * 2) | 0x3 << (SW1_PIN * 2) | 0x3 << (SW2_PIN * 2) | 0x3 << (SW3_PIN * 2) | 0x3 << (SW4_PIN * 2) | 0x3 << (SW5_PIN * 2));
    GPIOE->PUPDR &= ~(0x3 << (SW0_PIN * 2) | 0x3 << (SW1_PIN * 2) | 0x3 << (SW2_PIN * 2) | 0x3 << (SW3_PIN * 2) | 0x3 << (SW4_PIN * 2) | 0x3 << (SW5_PIN * 2));
    GPIOE->PUPDR |=  (0x1 << (SW0_PIN * 2) | 0x1 << (SW1_PIN * 2) | 0x1 << (SW2_PIN * 2) | 0x1 << (SW3_PIN * 2) | 0x1 << (SW4_PIN * 2) | 0x1 << (SW5_PIN * 2));

    // PB10 will conflict with SCK pin of the SPI
    // using PB15 as a replacement
    GPIOB->MODER &= ~(0x3 << (SW_ALT_PIN * 2) | 0x3 << (SW7_PIN * 2));
    GPIOB->PUPDR &= ~(0x3 << (SW_ALT_PIN * 2) | 0x3 << (SW7_PIN * 2));  
    GPIOB->PUPDR |=  (0x1 << (SW_ALT_PIN* 2) | 0x1 << (SW7_PIN * 2));

}

/**
 * Read the current values of the 8 slide switches.
 * Returns a bit mask (LSB = Switch[0], MSB = Switch[7]).
 */
uint16_t rcm_reg_switchbank_read() {
    uint16_t value = 0;

    value |= ((GPIOE->IDR >> SW0_PIN) & 0x01) << SWITCH_0;
    value |= ((GPIOE->IDR >> SW1_PIN) & 0x01) << SWITCH_1;
    value |= ((GPIOE->IDR >> SW2_PIN) & 0x01) << SWITCH_2;
    value |= ((GPIOE->IDR >> SW3_PIN) & 0x01) << SWITCH_3;
    value |= ((GPIOE->IDR >> SW4_PIN) & 0x01) << SWITCH_4;
    value |= ((GPIOE->IDR >> SW5_PIN) & 0x01) << SWITCH_5;
    value |= ((GPIOB->IDR >> SW_ALT_PIN) & 0x01) << SWITCH_6;
    value |= ((GPIOB->IDR >> SW7_PIN) & 0x01) << SWITCH_7;
    
    return value & SWITCHBANK_MASK;
}

void rcm_task_switch_init(void) {
    xTaskCreate( (void *) &rcm_task_switch, "switchbank", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void rcm_task_switch(void * arg) {
    switchQueue = xQueueCreate(1, sizeof(switch_t));
    rcm_reg_switchbank_init();
    for (;;) {
        uint8_t v = (uint8_t)(rcm_reg_switchbank_read() & MASKFORSWITCH);
        if (switchQueue) {
            switch_t m = { .switchValue = v };
            (void)xQueueOverwrite(switchQueue, &m); // keep only the newest value
        }
        vTaskDelay(SWITCH_POLL_MS);
    }
}