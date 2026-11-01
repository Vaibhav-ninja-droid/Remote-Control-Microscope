 /** 
 **************************************************************
 * File: repo/mylib/rcm_mfs_trimpot.c
 * Author: Vaibhav Rana
 * Date: 11/08/2025
 * Brief: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_reg_mfs_trimpot_init()
 * rcm_reg_mfs_trimpot_read()
 *************************************************************** 
 */

#include "rcm_mfs_trimpot.h"

ADC_HandleTypeDef AdcHandle;
ADC_ChannelConfTypeDef AdcChanConfig;

static uint16_t last = 0;
static uint16_t accum = 0;

QueueHandle_t trimpotQueue = NULL;

/**
 * Initialise the Trimpot input driver (GPIO and ADC).
 *
 * Configures the GPIO pin connected to the trimpot as an analog input.
 * Sets up the ADC module for single-ended conversion of the trimpot voltage.
 */
void rcm_reg_mfs_trimpot_init() {
    // Enable the GPIO Clock
  	__GPIOA_CLK_ENABLE();
  
	// Initalise PA3 as an Analog input.
  	GPIOA->MODER |= (0x03 << (3 * 2));			//Set bits for Analog input mode

  	GPIOA->OSPEEDR &= ~(0x03 << (3 * 2));
	GPIOA->OSPEEDR |= 0x02 << (3 * 2);  			// Fast speed

	GPIOA->PUPDR &= ~(0x03 << (3 * 2));			//Clear bits for no push/pull

	__ADC1_CLK_ENABLE();						//Enable ADC1 clock

	AdcHandle.Instance = (ADC_TypeDef *)(ADC1_BASE);						//Use ADC1 (For PA3)
	AdcHandle.Init.ClockPrescaler        = ADC_CLOCKPRESCALER_PCLK_DIV2;	//Set clock prescaler
	AdcHandle.Init.Resolution            = ADC_RESOLUTION12b;				//Set 12-bit data resolution
	AdcHandle.Init.ScanConvMode          = DISABLE;
	AdcHandle.Init.ContinuousConvMode    = DISABLE;
	AdcHandle.Init.DiscontinuousConvMode = DISABLE;
	AdcHandle.Init.NbrOfDiscConversion   = 0;
	AdcHandle.Init.ExternalTrigConvEdge  = ADC_EXTERNALTRIGCONVEDGE_NONE;	//No Trigger
	AdcHandle.Init.ExternalTrigConv      = ADC_EXTERNALTRIGCONV_T1_CC1;		//No Trigger
	AdcHandle.Init.DataAlign             = ADC_DATAALIGN_RIGHT;				//Right align data
	AdcHandle.Init.NbrOfConversion       = 1;
	AdcHandle.Init.DMAContinuousRequests = DISABLE;
	AdcHandle.Init.EOCSelection          = DISABLE;

	HAL_ADC_Init(&AdcHandle);		//Initialise ADC

	// Configure ADC Channel
	AdcChanConfig.Channel = ADC_CHANNEL_3;					//PA3 has Analog Channel 3 connected
	AdcChanConfig.Rank         = 1;
	AdcChanConfig.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	AdcChanConfig.Offset       = 0;

	HAL_ADC_ConfigChannel(&AdcHandle, &AdcChanConfig);		//Initialise ADC Channel
}

/**
 * Read the current trimpot input value.
 *
 * Performs an ADC conversion of the trimpot analog voltage and returns
 * the raw ADC value. The range depends on the ADC resolution 
 * Example: 0-4095 for a 12-bit ADC
 *
 * @return uint16_t Raw ADC value representing the trimpot input voltage.
 */
uint16_t rcm_reg_mfs_trimpot_read() {
    uint16_t AdcValue = 0;
	
	// start ADC conversion
    if (HAL_ADC_Start(&AdcHandle) != HAL_OK) {
        return 0;
    }

    // poll for end of conversion
    if (HAL_ADC_PollForConversion(&AdcHandle, 10) == HAL_OK) {
        // read the converted value
        AdcValue = (uint16_t)HAL_ADC_GetValue(&AdcHandle);
    }

	// stop ADC
    HAL_ADC_Stop(&AdcHandle);
	
	return AdcValue;
}

void rcm_trimpot_task_init(void) {
    xTaskCreate((void *) &rcm_trimpot_task, "TRIMPOT", configMINIMAL_STACK_SIZE * 2, NULL, tskIDLE_PRIORITY + 1, NULL);
}

void rcm_trimpot_task(void *arg) {
	rcm_reg_mfs_trimpot_init();

	if (!trimpotQueue) {
        trimpotQueue = xQueueCreate(8, sizeof(trimpot_t));
    }
	
	last = rcm_reg_mfs_trimpot_read();

    for (;;) {
        uint16_t now = rcm_reg_mfs_trimpot_read();
        int delta = (int)now - (int)last;

        if (delta > TRIM_DEADBAND || delta < -TRIM_DEADBAND) {
            last = now;

			int dir;
			uint16_t mag;
            
			if (delta > 0) {
				dir = 1;
			} else {
				dir = -1;
			}

			if (delta > 0) {
				mag = (uint16_t)delta;
			} else {
				mag = (uint16_t)(-delta);
			}

            accum += mag;

            while (accum >= TRIM_FULL_TURN_DELTA) {
                accum -= TRIM_FULL_TURN_DELTA;
                if (trimpotQueue) {
                    trimpot_t event = { .dir = (int8_t)dir, .turns = 1 };
                    (void)xQueueSend(trimpotQueue, &event, 0);
                }
            }
        }

        vTaskDelay(TRIM_POLL_MS);
    }
}