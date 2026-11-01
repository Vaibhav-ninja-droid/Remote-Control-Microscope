/**
 **************************************************************
 * File: repo/mylib/rcm_tx.h
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

#ifndef RCM_TX_H
#define RCM_TX_H

#include "board.h"
#include "processor_hal.h"
#include "nrf24l01plus_hal_tx.h"
#include "myconfig.h"
#include "FreeRTOS.h"
#include "queue.h"
#include <string.h>
#include "rcm_rcmsys.h"

typedef enum {
    TXIDLE,
    TXCONFIG,
    TXREADY,
    TXSEND,
    TXERROR
} txState;

extern QueueHandle_t txQueue; 
extern txState currentState;

void rcm_tx_task_init(void);

void rcm_tx_task(void *arg);

#endif