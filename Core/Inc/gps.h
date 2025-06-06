/*
 * gps.h
 *
 *  Created on: Jun 6, 2025
 *      Author: theis
 */
#include "stdint.h"

#ifndef INC_GPS_H_
#define INC_GPS_H_
#define RING_BUF_SIZE 256

typedef struct {
    uint8_t buffer[RING_BUF_SIZE];
    volatile uint16_t head;  // write index
    volatile uint16_t tail;  // read index
} RingBuffer;

void RingBuffer_Write(RingBuffer *rb, uint8_t byte);
int RingBuffer_Read(RingBuffer *rb, uint8_t *byte);
void process_uart_data(RingBuffer *rb);

#endif /* INC_GPS_H_ */
