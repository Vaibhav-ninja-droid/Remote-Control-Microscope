 /** 
 **************************************************************
 * File: repo/mylib/rcm_hamming.c
 * Author: Vaibhav Rana
 * Date: 6/09/2025
 * Brief: The Hamming LIB provides hamming byte encode, byte decode and half byte encode functions.
 * REFERENCE:
 ***************************************************************
 * EXTERNAL FUNCTIONS 
 ***************************************************************  
 * unsigned short rcm_lib_hamming_byte_encode(unsigned char value)
 * unsigned char rcm_lib_hamming_byte_decode(unsigned char value)
 * unsigned char hamming_hbyte_encode(unsigned char value)
 * int rcm_lib_hamming_parity_error(unsigned char value)
 *************************************************************** 
 */

#include "rcm_hamming.h"

typedef enum {
    BIT_P0 = 0,
    BIT_H0 = 1,
    BIT_H1 = 2,
    BIT_H2 = 3,
    BIT_D0 = 4,
    BIT_D1 = 5,
    BIT_D2 = 6,
    BIT_D3 = 7
} HammingBitPos;


/**
 * Encode a byte into a 16-bit Hamming code.
 */
unsigned short rcm_lib_hamming_byte_encode(unsigned char value) {
    uint16_t out;

	/* first encode D0..D3 (first 4 bits),
	 * then D4..D7 (second 4 bits).
	 */
	out = hamming_hbyte_encode(value & 0xF) |
		(hamming_hbyte_encode(value >> 4) << 8);

	return(out);
}

/**
 * Decode a Hamming-encoded byte back to 4 bits.
 * Also checks and fixes single-bit errors.
 */
unsigned char rcm_lib_hamming_byte_decode(unsigned char value) {
    uint8_t encoded = value;

    uint8_t b1 = (encoded >> BIT_H0) & 1;
    uint8_t b2 = (encoded >> BIT_H1) & 1;
    uint8_t b3 = (encoded >> BIT_H2) & 1;
    uint8_t d0 = (encoded >> BIT_D0) & 1;
    uint8_t d1 = (encoded >> BIT_D1) & 1;
    uint8_t d2 = (encoded >> BIT_D2) & 1;
    uint8_t d3 = (encoded >> BIT_D3) & 1;

    uint8_t s0 = b1 ^ d1 ^ d2 ^ d3;
    uint8_t s1 = b2 ^ d0 ^ d2 ^ d3;
    uint8_t s2 = b3 ^ d0 ^ d1 ^ d3;
    uint8_t syndrome = (s0 << 2) | (s1 << 1) | (s2 << 0);

    int parityError = rcm_lib_hamming_parity_error(encoded);

    if (syndrome == 0) {
        if (parityError) {
            encoded ^= 0x01;
        }
    } else {
        if (parityError) {
            uint8_t flip = 0;
            switch (syndrome) {
                case 0b100:
                    flip = BIT_H0; // h0
                    break;
                case 0b010:
                    flip = BIT_H1; // h1
                    break;
                case 0b001:
                    flip = BIT_H2; // h2
                    break;
                case 0b011:
                    flip = BIT_D0; // d0
                    break;
                case 0b101:
                    flip = BIT_D1; // d1
                    break;
                case 0b110:
                    flip = BIT_D2; // d2
                    break;
                case 0b111:
                    flip = BIT_D3; // d3
                    break;
                default:
                    flip = BIT_P0;
                    break;
            }

            if (flip) {
                encoded ^= (1u << flip);
            }
        }
    }

    // extract corrected data
    uint8_t out0 = (encoded >> BIT_D0) & 1;
    uint8_t out1 = (encoded >> BIT_D1) & 1;
    uint8_t out2 = (encoded >> BIT_D2) & 1;
    uint8_t out3 = (encoded >> BIT_D3) & 1;

    uint8_t out = out0 | (out1 << 1) | (out2 << 2) | (out3 << 3);
    
    return out;
}

/**
 * Encode a 4-bit half-byte using Hamming code.
 * (Used internally by the library.)
 */
unsigned char hamming_hbyte_encode(unsigned char value) {
    uint8_t d0, d1, d2, d3;
	uint8_t p0 = 0, h0, h1, h2;
	uint8_t z;
	uint8_t out;

	/* extract bits */
	d0 = !!(value & 0x1);
	d1 = !!(value & 0x2);
	d2 = !!(value & 0x4);
	d3 = !!(value & 0x8);

	/* calculate hamming parity bits */
	h0 = d1 ^ d2 ^ d3;
	h1 = d0 ^ d2 ^ d3;
	h2 = d0 ^ d1 ^ d3;

	/* generate out byte without parity bit P0 */
	out = (h0 << 1) | (h1 << 2) | (h2 << 3) |
		(d0 << 4) | (d1 << 5) | (d2 << 6) | (d3 << 7);

	/* calculate even parity bit */
	for (z = 1; z < 8; z++)
		p0 = p0 ^ !!(out & (1 << z));

	out |= p0;

	return(out);
}

/**
 * Check if a parity error is present.
 * Returns 1 if error found, 0 otherwise.
 */
int rcm_lib_hamming_parity_error(unsigned char value) {
    uint8_t p0 = value & 0x01;
    uint8_t parity = 0;
    for (uint8_t i = 1; i <= 7; i++) {
        parity ^= !!(value & (1u << i));
    }

    if (p0 ^ parity) {
        return 1;
    } else {
        return 0;
    }
}