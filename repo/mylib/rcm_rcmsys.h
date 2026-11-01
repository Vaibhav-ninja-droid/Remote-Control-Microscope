/** 
 **************************************************************
 * File: repo/mylib/rcm_rcmsys.h
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

#ifndef RCM_RCMSYS_H
#define RCM_RCMSYS_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>

#include "rcm_mfs_led.h"
#include "rcm_lta1000g.h"
#include "rcm_mfs_pb.h"
#include "rcm_tx.h"
#include "rcm_cmdin.h"
#include "rcm_packet.h"
#include "rcm_mfs_trimpot.h"
#include "rcm_output.h"
#include "rcm_ssd.h"
#include "rcm_rgb.h"
#include "rcm_mfs_pb.h"
#include "rcm_switchbank.h"

void rcm_rcmsys_init(void);

void rcm_rcmsys_on_radio_tx(void);

void rcm_rcmsys_on_any_pb(void);

void rcm_rcmsys_opcode(uint8_t opcode);

#endif