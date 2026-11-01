/**
  ******************************************************************************
  * File:    pf/main.c
  * Author:  Vaibhav Rana
  * Date:    06/10/2025
  * Brief:   The RCM is a microscope that can view targets with great magnification. The RCM is a
  *          computer numerical controller machine that can move in 3 dimensions. The project has a
  *          microscope that can view, zoom, and detect an April tag.
  * REFERENCE: 
  ******************************************************************************
  */

#include "board.h"
#include "processor_hal.h"

#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"

#include "rcm_rcmsys.h"
#include "rcm_mfs_pb.h"
#include "rcm_tx.h"
#include "rcm_cmdin.h"
#include "rcm_packet.h"
#include "rcm_cli_radio.h"
#include "rcm_cli_task.h"

/*
 * Main program
 */
int main(void)  
{
    HAL_Init();			//Initalise Board

    rcm_rcmsys_init();

    // Start the scheduler
    vTaskStartScheduler();

    return 0;
}
