/** 
 **************************************************************
 * File: repo/mylib/rcm_cmdin.h
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: Command input task. S1 cycles fields, S3 applies current
 *        switch value, trimpot adjusts zoom.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_cmdin_task_init(void)
 * rcm_cmdin_task(void *arg)
 *************************************************************** 
 */

#ifndef RCM_CMDIN_H
#define RCM_CMDIN_H

#include <stdint.h>
#include "FreeRTOS.h"
#include "task.h"
#include "semphr.h"

#include "rcm_lta1000g.h"
#include "rcm_mfs_pb.h"
#include "rcm_mfs_trimpot.h"
#include "rcm_rcmsys.h"
#include "rcm_switchbank.h"
#include "rcm_packet.h"

#include "rcm_output.h"

#define ZOOM_STEP_PERCENT 20      // 10% per step
#define ZOOM_MIN 0
#define ZOOM_MAX 200

#define POLL_MS 10
#define DELAY 200

typedef enum {
    OPCODE_IDLE = 0x01,
    OPCODE_X = 0x02,
    OPCODE_Y = 0x03,
    OPCODE_Z = 0x04,
    OPCODE_ZOOM = 0x05,
    OPCODE_APRIL = 0x06,
    OPCODE_NEW = 0x07,
    OPCODE_DEL = 0x08,
    OPCODE_SYS = 0x09,
    OPCODE_ORG = 0x0A,
    OPCODE_PAT = 0x0B,
    OPCODE_JOIN = 0x0F,
} OpCode;

typedef enum {
    S_IDLE,
    S_X,
    S_Y,
    S_Z,
    S_ZOOM,
    S_APRIL,
    S_ORG,
    S_JOIN
} cmdState;

extern cmdState CurrentState;

void rcm_cmdin_task_init(void);

void rcm_cmdin_task(void *arg);

#endif
