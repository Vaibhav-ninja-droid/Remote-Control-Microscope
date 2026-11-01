/** 
 **************************************************************
 * File: repo/mylib/rcm_tx.c
 * Author: Vaibhav Rana
 * Date: 13/10/2025
 * Brief: FreeRTOS TX task that pulls encoded packets from a queue
 *        and fires them over the nRF24L01+.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_tx_init(void)
 * rcm_tx_task(void *arg)
 *************************************************************** 
 */

#include "rcm_tx.h"
#include "debug_log.h"


QueueHandle_t txQueue = NULL; 
txState currentState = TXIDLE;

/**
 * Create the TX task.
 */
void rcm_tx_task_init(void) {
    xTaskCreate( (void *) &rcm_tx_task, "TX_TASK", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 2, NULL);
}

/**
 * TX state machine. Waits for a packet, then sends it.
 */
void rcm_tx_task(void *arg) {
    uint8_t packet[NRF24L01P_TX_PLOAD_WIDTH];

    nrf24l01plus_hal_init();
    BRD_debuguart_init(); 
    memset(packet, 0, 32);

    txQueue = xQueueCreate(8, NRF24L01P_TX_PLOAD_WIDTH);
    currentState = TXCONFIG;
    for (;;) {
        switch (currentState) {
            case TXCONFIG:
                currentState = TXREADY;
                break;
            case TXREADY:
                if (xQueueReceive(txQueue, packet, portMAX_DELAY) == pdTRUE) {
                    currentState = TXSEND;
                }
                break;
            case TXSEND:
                nrf24l01plus_hal_send(packet);
                rcm_rcmsys_on_radio_tx();
                currentState = TXREADY;
                break;
            default:
                currentState = TXERROR;
                break;
        }

        vTaskDelay(50);
    }
}

