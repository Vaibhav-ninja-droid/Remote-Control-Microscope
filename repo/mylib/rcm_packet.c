/** 
 **************************************************************
 * File: repo/mylib/rcm_packet.c
 * Author: Vaibhav Rana
 * Date: 26/10/2025
 * Brief: Build 16-byte RCM packets, Hamming-encode to 32 bytes,
 *        and push to the TX queue.
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

#include "rcm_packet.h"
#include "myconfig.h"

/** 
 * Fill the configured four-byte sender address. 
 */
static void sender_addr(uint8_t out[4]) {
    out[0] = RCM_SENDER_ID_0;
    out[1] = RCM_SENDER_ID_1;
    out[2] = RCM_SENDER_ID_2;
    out[3] = RCM_SENDER_ID_3;
}

/**
 * Write decimal digits into a fixed width field (zero-padded).
 * @param dst    Where to write.
 * @param value  Non-negative number.
 * @param digits Field width.
 */
static void fmt_digits(char *dst, unsigned value, int digits) {
    for (int i = digits - 1; i >= 0; i--) {
        dst[i] = '0' + (value % 10);  // take last digit
        value /= 10;                  // shift right
    }
}

/**
 * Hamming encode 16 raw bytes into 32 encoded bytes.
 * Each input byte -> 2 bytes via rcm_lib_hamming_byte_encode in rcm_hamming.c.
 */
void rcm_hamming_packet_encoding(const uint8_t input[RCM_PACKET_UNENC_SIZE], uint8_t output[RCM_PACKET_ENC_SIZE]) {
    for (int i = 0; i < RCM_PACKET_UNENC_SIZE; i++) {
        uint16_t encoded = rcm_lib_hamming_byte_encode(input[i]);
        output[(2 * i) + 0] = (uint8_t)(encoded & MASK);
        output[(2 * i) + 1] = (uint8_t)(encoded >> 8);
    }
}

/**
 * Build a 16-byte unencoded packet with type, sender and payload.
 */
void build_unencoded(uint8_t type, const char payload[RCM_PAYLOAD_SIZE], uint8_t output[RCM_PACKET_UNENC_SIZE]) {
    memset(output, 0, RCM_PACKET_UNENC_SIZE);
    output[0] = type;

    uint8_t id[4];
    sender_addr(id);
    output[1] = id[0];
    output[2] = id[1];
    output[3] = id[2];
    output[4] = id[3];

    memcpy(&output[5], payload, RCM_PAYLOAD_SIZE);  // remaining bytes already 0
}

/**
 * Encode and enqueue a packet for TX.
 * @return false if TX queue not ready.
 */
bool rcm_rcm_packet_send(uint8_t type, rcm_packet_t* pkts) {
    uint8_t raw[RCM_PACKET_UNENC_SIZE];
    uint8_t encoded[RCM_PACKET_ENC_SIZE];

    build_unencoded(type, pkts->payload, raw);
    rcm_hamming_packet_encoding(raw, encoded);

    if (!txQueue) {
        return false; // TX task not up yet
    }
    return (xQueueSend(txQueue, encoded, 50) == pdTRUE);
}

/** 
 * Send a JOIN packet. 
 */
bool rcm_rcm_packet_join(rcm_packet_t* pkts) {
    memset(pkts, 0, sizeof(*pkts));
    
    pkts->type = RCM_PACKET_TYPE_JOIN;
    sender_addr(pkts->sender);
    memcpy(pkts->payload, "JOIN", 4);

    return rcm_rcm_packet_send(RCM_PACKET_TYPE_JOIN, pkts);
}

/** 
 * Send an XYZ packet: "XYZxxxyyyzz". 
 */
bool rcm_rcm_packet_xyz(uint16_t x, uint16_t y, uint8_t z, rcm_packet_t* pkts) {
    memset(pkts, 0, sizeof(*pkts));
    pkts->type = RCM_PACKET_TYPE_XYZ;
    sender_addr(pkts->sender);

    char *p = pkts->payload;
    *p++ = 'X';
    *p++ = 'Y'; 
    *p++ = 'Z';

    fmt_digits(p, x, 3);
    p += 3;
    fmt_digits(p, y, 3);
    p += 3;
    fmt_digits(p, z, 2);
    p += 2;

    return rcm_rcm_packet_send(RCM_PACKET_TYPE_XYZ, pkts);
}

/** 
 * Send a ZOOM packet: "ZOOMnnn". 
 */
bool rcm_rcm_packet_zoom(uint16_t zoom, rcm_packet_t* pkts) {
    memset(pkts, 0, sizeof(*pkts));
    pkts->type = RCM_PACKET_TYPE_ZOOM;
    sender_addr(pkts->sender);

    memcpy(pkts->payload, "ZOOM", 4);
    fmt_digits(&pkts->payload[4], zoom, 3);

    return rcm_rcm_packet_send(RCM_PACKET_TYPE_ZOOM, pkts);
}

/** 
 * Send an APRIL packet: "APRILON" or "APRILOFF". 
 */
bool rcm_rcm_packet_april(bool enable, rcm_packet_t* pkts) {
    memset(pkts, 0, sizeof(*pkts));
    pkts->type = RCM_PACKET_TYPE_APRIL;
    sender_addr(pkts->sender);

    memcpy(pkts->payload, "APRIL", 5);
    
    if (enable) {
        memcpy(&pkts->payload[5], "ON", 2);
    } else {
        memcpy(&pkts->payload[5], "OFF", 3);
    }

    return rcm_rcm_packet_send(RCM_PACKET_TYPE_APRIL, pkts);
}

