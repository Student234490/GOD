/*
 * gps.c
 *
 *  Created on: Jun 6, 2025
 *      Author: theis
 */

#include "gps.h"
#include <stdint.h>
#include <stdio.h> // used for printing

void RingBuffer_Write(RingBuffer *rb, uint8_t byte) {
    uint16_t next = (rb->head + 1) % RING_BUF_SIZE;
    if (next != rb->tail) {  // Avoid overwriting unread data
        rb->buffer[rb->head] = byte;
        rb->head = next;
    }
}

int RingBuffer_Read(RingBuffer *rb, uint8_t *byte) {
    if (rb->head == rb->tail) {
        return 0;  // No data
    }
    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUF_SIZE;
    return 1;
}

char sentence[100];  // Temporary line buffer
int indx = 0;

void process_uart_data(RingBuffer*rb) {
    uint8_t c;

    while (RingBuffer_Read(rb, &c)) {
        if (indx < sizeof(sentence) - 1) {
            sentence[indx++] = c;
        }

        if (c == '\n') {
            sentence[indx] = '\0';  // Null-terminate the string
            printf("Got line: %s", sentence);
            indx = 0;  // Reset for next line
        }
    }
}
