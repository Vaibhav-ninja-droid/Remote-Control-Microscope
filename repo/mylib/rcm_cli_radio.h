/** 
 **************************************************************
 * File: repo/mylib/rcm_cli_radio.h
 * Author: Vaibhav Rana
 * Date: 07/10/2025
 * Brief: CLI peripheral driver for the nRF24L01+ radio
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_cli_radio_init(void)
 *************************************************************** 
 */

#ifndef RCM_CLI_RADIO_H
#define RCM_CLI_RADIO_H

#include "board.h"
#include "processor_hal.h"

void rcm_cli_radio_init(void);

#endif