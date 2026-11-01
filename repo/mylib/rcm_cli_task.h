/** 
 **************************************************************
 * File: repo/mylib/rcm_cli_task.h
 * Author: Vaibhav Rana
 * Date: 07/10/2025
 * Brief: Conforms to CLI file conventions. This file only declares an init() that
 * creates the CLI task.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_cli_init(void)
 *************************************************************** 
 */

#ifndef RCM_CLI_TASK_H
#define RCM_CLI_TASK_H

#include "board.h"
#include "processor_hal.h"

void rcm_cli_init(void);

#endif