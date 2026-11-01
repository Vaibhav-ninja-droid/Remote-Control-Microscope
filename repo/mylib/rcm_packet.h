/** 
 **************************************************************
 * File: repo/mylib/rcm_packet.h
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: Build 16-byte RCM packets, Hamming-encode to 32 bytes,
 *        and push to the TX queue.
 * REFERENCE: 
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * rcm_hamming_packet_encoding(const uint8_t input[RCM_PACKET_UNENC_SIZE], uint8_t output[RCM_PACKET_ENC_SIZE])
 * rcm_rcm_packet_send(uint8_t type, rcm_packet_t* pkts)
 * rcm_rcm_packet_join(rcm_packet_t* pkts)
 * rcm_rcm_packet_xyz(uint16_t x, uint16_t y, uint8_t z, rcm_packet_t* pkts)
 * rcm_rcm_packet_zoom(uint16_t zoom, rcm_packet_t* pkts)
 * rcm_rcm_packet_april(bool enable, rcm_packet_t* pkts)
 *************************************************************** 
 */

#ifndef RCM_PACKET_H
#define RCM_PACKET_H

#include <string.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>
#include "FreeRTOS.h"
#include "queue.h"

#include "rcm_hamming.h"
#include "rcm_tx.h"

#define MASK 0xFF

#define RCM_PACKET_TYPE_JOIN 0x20
#define RCM_PACKET_TYPE_XYZ 0x22
#define RCM_PACKET_TYPE_ZOOM 0x26
#define RCM_PACKET_TYPE_APRIL 0x28

#define RCM_PACKET_UNENC_SIZE 16
#define RCM_PACKET_ENC_SIZE 32
#define RCM_PAYLOAD_SIZE 11

typedef struct {
    uint8_t type;
    uint8_t sender[4]; // Four-byte sender identifier configured in myconfig.h
    char payload[RCM_PAYLOAD_SIZE];
} rcm_packet_t;

void rcm_hamming_packet_encoding(const uint8_t input[RCM_PACKET_UNENC_SIZE], uint8_t output[RCM_PACKET_ENC_SIZE]);

bool rcm_rcm_packet_send(uint8_t type, rcm_packet_t* pkts);

bool rcm_rcm_packet_join(rcm_packet_t* pkts);

bool rcm_rcm_packet_xyz(uint16_t x, uint16_t y, uint8_t z, rcm_packet_t* pkts);

bool rcm_rcm_packet_zoom(uint16_t zoom, rcm_packet_t* pkts);

bool rcm_rcm_packet_april(bool enable, rcm_packet_t* pkts);

#endif
