/*
 * gps.h
 *
 *  Created on: Jun 6, 2025
 *      Author: theis
 */
#include "stdint.h"
#include "vector.h"
#include "igrf16.h"

#ifndef INC_GPS_H_
#define INC_GPS_H_
#define RING_BUF_SIZE 4000
#define LINEBUFFERSIZE 500

typedef struct {
    uint8_t buffer[RING_BUF_SIZE];
    volatile uint16_t head;  // write index
    volatile uint16_t tail;  // read index
} RingBuffer;

typedef struct {
	int32_t latitude;
	int32_t longitude;
	int32_t altitude;
	igrf_time_t gpstime;
	char active;
} GPSRead_t;

void RingBuffer_Write(RingBuffer *rb, uint8_t byte); // runs in the interrupt
int RingBuffer_Read(RingBuffer *rb, uint8_t *byte); // runs in process_uart_data
int process_uart_data(RingBuffer *rb, GPSRead_t *gps); // runs in main script
void getGPGGA(char sentence[LINEBUFFERSIZE], GPSRead_t *gps);
void printGPS(GPSRead_t GPS);
Vector3D BfromGPS(GPSRead_t GPS, igrf_frame_t frame);
Vector3D BfromGPS_gcc(GPSRead_t GPS);
Vector3D BfromGPS_gdt(GPSRead_t GPS);

#endif /* INC_GPS_H_ */
