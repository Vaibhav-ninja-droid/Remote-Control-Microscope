/** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_pb.c
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

#include "rcm_mfs_pb.h"

static SemaphoreHandle_t s_semS1 = NULL;
static SemaphoreHandle_t s_semS3 = NULL;

static void rcm_MfsTask(void *arg) {
    rcm_reg_mfs_pb_init();   // CALL REGISTER INIT FROM TASK

    // Route PC0 -> EXTI0 (S1, active-low)
    __SYSCFG_CLK_ENABLE();
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0;
    SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI0_PC;
    EXTI->IMR  |= EXTI_IMR_IM0;
    EXTI->FTSR |= EXTI_FTSR_TR0;
    EXTI->RTSR &= ~EXTI_RTSR_TR0;

    // PF3 -> EXTI3 
    SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI3;
    SYSCFG->EXTICR[0] |=  SYSCFG_EXTICR1_EXTI3_PF;
    EXTI->IMR  |= EXTI_IMR_IM3;
    EXTI->FTSR |= EXTI_FTSR_TR3;
    EXTI->RTSR &= ~EXTI_RTSR_TR3;

    NVIC_SetPriority(EXTI0_IRQn, 5);  // at or below LIBRARY_MAX_SYSCALL_PRIORITY
    NVIC_EnableIRQ(EXTI0_IRQn);

    NVIC_SetPriority(EXTI3_IRQn, 5);
    NVIC_EnableIRQ(EXTI3_IRQn);

    for(;;) {
        vTaskDelay(portMAX_DELAY);
    }
}

void rcmTaskMfsInit(void) {
    if (!s_semS1) {
        s_semS1 = xSemaphoreCreateBinary();
    }
    if (!s_semS3) {
        s_semS3 = xSemaphoreCreateBinary();
    }
    xTaskCreate(rcm_MfsTask, "mfs_pb", configMINIMAL_STACK_SIZE*2, NULL, tskIDLE_PRIORITY+2, NULL);
}

SemaphoreHandle_t rcmMfsS1Semaphore(void) { 
    return s_semS1; 
}

SemaphoreHandle_t rcmMfsS3Semaphore(void) {
    return s_semS3; 
}

void EXTI0_IRQHandler(void) {
    BaseType_t hpw = pdFALSE;
    if (EXTI->PR & EXTI_PR_PR0) {
        EXTI->PR = EXTI_PR_PR0;                 // clear pending
        if (s_semS1) {
            xSemaphoreGiveFromISR(s_semS1, &hpw);
        }
        portYIELD_FROM_ISR(hpw);
    }
}

void EXTI3_IRQHandler(void) {
    BaseType_t hpw = pdFALSE;
    if (EXTI->PR & EXTI_PR_PR3) {
        EXTI->PR = EXTI_PR_PR3;
        if (s_semS3) {
            xSemaphoreGiveFromISR(s_semS3, &hpw);
        }
        portYIELD_FROM_ISR(hpw);
    }
}

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
void rcm_reg_mfs_pb_init () {
    __GPIOC_CLK_ENABLE();
    __GPIOF_CLK_ENABLE();

    GPIOC->MODER &= ~(0x3 << (0 * 2));
    GPIOC->PUPDR &= ~(0x3 << (0 * 2));
    GPIOC->PUPDR |=  (0x1 << (0 * 2));

    GPIOC->MODER &= ~(0x3 << (3 * 2));
    GPIOC->PUPDR &= ~(0x3 << (3 * 2));
    GPIOC->PUPDR |=  (0x1 << (3 * 2));

    GPIOF->MODER &= ~(0x3 << (3 * 2));
    GPIOF->PUPDR &= ~(0x3 << (3 * 2));
    GPIOF->PUPDR |=  (0x1 << (3 * 2));
}

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
unsigned int rcm_reg_mfs_pb_signal_get (int pb_select) {
    unsigned int state = 0;
    
    if (pb_select & RCM_REG_MFA_PB_S1) {
        if ((GPIOC->IDR & (1 << 0)) == 0) {  // active low
            state |= 0x01;
        }
    }

    if (pb_select & RCM_REG_MFA_PB_S2) {
        if ((GPIOC->IDR & (1 << 3)) == 0) {  // active low
            state |= 0x02;
        }
    }

    if (pb_select & RCM_REG_MFA_PB_S3) {
        if ((GPIOF->IDR & (1 << 3)) == 0) {  // active low
            state |= 0x04;
        }
    }
    
    return state;
}
