/** 
 **************************************************************
 * File: repo/mylib/rcm_ssd.c
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: 7-Segment Display(SSD)
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_ssd_init(void)
 * rcm_reg_ssd_set(uint8_t digitNum, uint8_t number)
 * rcm_task_ssd_init(void)
 * rcm_task_ssd(void)
 *************************************************************** 
 */

#include "rcm_ssd.h"

// digits from 0 to 9
static uint16_t ssd_digits[10] = {0x03, 0x9F, 0x25, 0x0D, 0x99, 0x49, 0x41, 0x1B, 0x01, 0x09};
uint16_t arr[4] = {0, 0, 0, 0};

QueueHandle_t MessageQueue = NULL;

void rcm_reg_ssd_init(void) {
    __GPIOF_CLK_ENABLE();

    GPIOF->MODER &= ~((0x03 << (SERIAL_DATA_PIN * 2)) | (0x03 << (SRCLK_PIN * 2)) | (0x03 << (RCLK_PIN * 2)));
    GPIOF->MODER |= ((0x01 << (SERIAL_DATA_PIN * 2)) | (0x01 << (SRCLK_PIN * 2)) | (0x01 << (RCLK_PIN * 2)));
    GPIOF->OTYPER &= ~((0x01 << (SERIAL_DATA_PIN * 2)) | (0x01 << (SRCLK_PIN * 2)) | (0x01 << (RCLK_PIN * 2)));

    // Timer 2 clock enable
	__TIM2_CLK_ENABLE();

	// Compute the prescaler value to set the timer counting frequency to 50kHz
	// SystemCoreClock is the system clock frequency
	TIM2->PSC = ((SystemCoreClock / 2) / TIMER_COUNTER_FREQ_2) - 1;

	// Counting direction: 0 = up-counting, 1 = down-counting (Timer Control Register 1)
	TIM2->CR1 |= TIM_CR1_DIR; 

	TIM2->CNT = TIMER_1SECOND_PERIOD;	//Load counter register with 1s period value
	TIM2->ARR = TIMER_1SECOND_PERIOD;	//Load auto reload register with 1s period value

	// Enable related interrupts
	TIM2->DIER |= TIM_DIER_UIE;			// Enable update interrupt to occur on timer counter overflow or underflow.
	
	//Enable priority (10) and interrupt callback. Do not set a priority lower than 5.
	HAL_NVIC_SetPriority(TIM2_IRQn, 10, 0);
	HAL_NVIC_EnableIRQ(TIM2_IRQn);

	TIM2->CR1 |= TIM_CR1_CEN;				// Enable the counter
}

/*
 * Timer update interrupt callback
 */
void tui_callback(void) {
    for(int i = 3; i >= 0; i--) {
        rcm_reg_ssd_set(i, arr[3 - i]);
    }
}

/*
 * Interrupt handler for Timer 2 Interrupt Handler
 */ 
void TIM2_IRQHandler(void) {

	//Check and clear overflow flag.
	if((TIM2->SR & TIM_SR_UIF) == TIM_SR_UIF) {	// Check if overflow has taken place 

		TIM2->SR &= ~TIM_SR_UIF; 					// Clear the UIF Flag

		tui_callback();   // Callback for timer update interrupt
	}
}

/**
 * Shift out a 16-bit frame for one digit selection + segments.
 * @param digitNum 0..3 (digit select)
 * @param number 16-bit segment word
 */
void rcm_reg_ssd_set(uint8_t digitNum, uint8_t number) {
    uint16_t send_val = 0;
    uint16_t dig = (1 << digitNum);
    dig = dig << 12;
    dig |= 0x0F00;

    send_val = (dig | number);
        
    for (int i = 0; i < 16; i++) {
        GPIOF->ODR &= ~(0x01 << SRCLK_PIN); // Clear bit on port E   
        GPIOF->ODR &= ~(0x01 << SERIAL_DATA_PIN); // Clear bit on port E
        GPIOF->ODR |=  (send_val & 1) << SERIAL_DATA_PIN; // Set bit on port E
        send_val = send_val >> 1;
        GPIOF->ODR |= 1 << SRCLK_PIN; // Set bit on port E 
    }
    // clock twice since there are two registers
    GPIOF->ODR &= ~(0x01 << RCLK_PIN); 
    GPIOF->ODR |= 1 << RCLK_PIN; 
    GPIOF->ODR &= ~(0x01 << RCLK_PIN); 
    GPIOF->ODR |= 1 << RCLK_PIN;
}

/**
 * Create the SSD task.
 */
void rcm_task_ssd_init(void) {
    xTaskCreate((void *) &rcm_task_ssd, (const signed char *) "7-Segment Display(SSD)", configMINIMAL_STACK_SIZE * 10, NULL, tskIDLE_PRIORITY + 2, NULL);
}

/**
 * SSD task: waits for a number and updates the 4 digit images.
 */
void rcm_task_ssd(void) {
    MessageQueue = xQueueCreate(4, sizeof(struct Message));
    
    taskENTER_CRITICAL(); //Stop any interrupts
    rcm_reg_ssd_init();
    taskEXIT_CRITICAL();
	struct Message RecvMessage;
    for (;;) {
		if (MessageQueue != NULL) {
			if (xQueueReceive(MessageQueue, &RecvMessage, 10 )) {
                uint16_t num = RecvMessage.s;
                arr[0] = ssd_digits[(int)(num / 1000) % 10];
                arr[1] = ssd_digits[(int)(num / 100) % 10];
                arr[2] = ssd_digits[(int)(num / 10) % 10];
                arr[3] = ssd_digits[num % 10];
        	}
        }
        vTaskDelay(1);
    }
}
