 /** 
 **************************************************************
 * File: repo/mylib/rcm_hamming.h
 * Author: Vaibhav Rana
 * Date: 6/09/2025
 * Brief: The Hamming LIB provides hamming byte encode, byte decode and half byte encode functions.
 * REFERENCE:
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_lib_hamming_byte_encode(unsigned char value)
 * rcm_lib_hamming_byte_decode(unsigned char value)
 * hamming_hbyte_encode(unsigned char value)
 * rcm_lib_hamming_parity_error(unsigned char value)
 *************************************************************** 
 */

#ifndef RCM_HAMMING_H
#define RCM_HAMMING_H

#include "board.h"
#include "processor_hal.h"
#include <stdint.h>

unsigned short rcm_lib_hamming_byte_encode(unsigned char value);

unsigned char rcm_lib_hamming_byte_decode(unsigned char value);

unsigned char hamming_hbyte_encode(unsigned char value);

int rcm_lib_hamming_parity_error(unsigned char value);

#endif
