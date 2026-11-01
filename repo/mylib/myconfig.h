#ifndef MYCONFIG_H
#define MYCONFIG_H

#include <stdint.h>

/* Example sender ID. Set these bytes to an ID accepted by the receiver. */
#define RCM_SENDER_ID_0 0x00
#define RCM_SENDER_ID_1 0x00
#define RCM_SENDER_ID_2 0x00
#define RCM_SENDER_ID_3 0x01

#define MYRADIOCHAN 52  // RF channel for RCM ID 6

static uint8_t myradiotxaddr[5] = {0x30, 0x10, 0x00, 0x00, 0x52};

#endif