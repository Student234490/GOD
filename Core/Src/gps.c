/*
 * gps.c
 *
 *  Created on: Jun 6, 2025
 *      Author: theis
 */

#include "gps.h"
#include <stdint.h>
#include <stdio.h> // used for printing
#include <string.h>
#include <fixp.h>
#define DELIM ","
#define ASCII0 48

void RingBuffer_Write(RingBuffer *rb, uint8_t byte) {
    uint16_t next = (rb->head + 1) % RING_BUF_SIZE;
    if (next != rb->tail) {  // Avoid overwriting unread data
        rb->buffer[rb->head] = byte;
        rb->head = next;
    } else {
    	printf("Error! Overwriting ringbuffer.");
    }
}

int32_t bredconv(char* string) {
	/*
	 * DEPRECATED FUNCTION
	 * Input1: A string containing a latitude (breddegrad) format DDmm.mmmm
	 * Output: 8.24 decimal degrees latitude
	 */
	printf("  :%s:  ", string);
	int64_t powten[7] = {1, 10, 100, 1000, 10000, 100000, 1000000};
	int32_t degrees = 0;
	int i;
	for (i = 0; i <= 1; i++) {
		degrees += ((string[i] - ASCII0) * powten[1-i]);
	}
	degrees = degrees << 24;
	int64_t raw_minutes = 0;
	int k = 0;
	for (i = 0; i <= 6; i++) {
	    if (i == 2) k++;  // skip point character
	    raw_minutes += (string[2 + k] - ASCII0) * powten[5 - i];
	    k++;
	}

	int32_t minutes = (raw_minutes << 24 / powten[6]);
	return degrees + minutes;
}

int32_t convert_DDmm_mmmm_to_fixed(char* str) {
	printf("   input=%s    ", str);
    // Parse degrees (first two digits)
    int32_t degrees = (str[0] - '0') * 10 + (str[1] - '0');

    // Parse minutes (everything after the degrees)
    int32_t minutes_int = (str[2] - '0') * 10 + (str[3] - '0');

    // Parse fractional minutes (4 digits after decimal)
    int32_t frac_minutes = 0;
    int i = 5; // Skip the dot (str[4] should be '.')
    for (int d = 0; d < 4; d++) {
        frac_minutes = frac_minutes * 10 + (str[i++] - '0');
    }
    printf("deg=%ld min=%ld fracmin=%ld    ", degrees, minutes_int, frac_minutes);

    // Total minutes = minutes_int.frac_minutes
    // Convert to micro-minutes to preserve precision: 24.7777 becomes 247777
    int32_t total_micro_minutes = minutes_int * 10000 + frac_minutes;

    // Convert micro-minutes to degrees:
    // degrees_fraction = total_micro_minutes / 60.0 / 10000.0
    // Use fixed-point math:
    // scaled_fraction = (total_micro_minutes * 65536) / (60 * 10000)
    int32_t scaled_fraction = ((int64_t)total_micro_minutes * 65536) / 600000;

    // Integer part shifted to 16.16 format
    int32_t fixed_degrees = degrees << 16;

    return fixed_degrees + scaled_fraction;
}

int string_to_int(const char *str) {
    int result = 0;
    while (*str) {
        if (*str >= '0' && *str <= '9') {
            result = result * 10 + (*str - '0');
        }
        str++;
    }
    return result;
}

int powten(int x) {
	int i = 1;
	if (x > 0) {
		for (int j = 0; j < x; j++) {
			i *= 10;
		}
	}
	return i;
}

int32_t Altxx(char *data) {
	// format: x<integer>x.x<integer>x, where there can be any amount of integers and decimals (meters)
	char* tokptr = strtok(data, ".");
	int ints[2] = {0,0};
	int len2 = 0;
	int j = 0;
	while (tokptr != NULL) {
        ints[j] = string_to_int(tokptr);
        if (j == 1) {len2 = strlen(tokptr);}
        // next iteration
        j++;
        tokptr = strtok(NULL, ".");
	}
    // Convert to fixed-point representation
    //printf("%i", ints[1]);
	int32_t result = convert(ints[0]);
	result += Rational(ints[1], powten(len2));
    //printFix(result);
    return result;
}

int32_t DDmmmmmm(char *data) {
    // Split
    char DD_str[3] = {0};
    char mm_str[8] = {0};

    strncpy(DD_str, data, 2);
    strcpy(mm_str, data + 2);

    // Convert
    int32_t DD = string_to_int(DD_str) << FIX16_SHIFT;
    int64_t mm_int = string_to_int(mm_str);            //64 cuz no worki with 32
    int32_t mm = ((mm_int << FIX16_SHIFT) / 600000);
    int32_t cords = mm + DD;

    return cords;
}

int32_t DDDmmmmmm(char *data) {
	char DD_str[4] = {0};
	char mm_str[10] = {0};

	strncpy(DD_str, data, 3);
	strcpy(mm_str, data + 3);

	// Convert
	int32_t DD = string_to_int(DD_str) << FIX16_SHIFT;
	int64_t mm_int = string_to_int(mm_str);
	int32_t mm = ((mm_int << FIX16_SHIFT) / 600000);

	int32_t cords = mm + DD;
	return cords;
}

int RingBuffer_Read(RingBuffer *rb, uint8_t *byte) {
    if (rb->head == rb->tail) {
        return 0;  // No data
    }
    *byte = rb->buffer[rb->tail];
    rb->tail = (rb->tail + 1) % RING_BUF_SIZE;
    return 1;
}

void getGPGGA(char sentence[LINEBUFFERSIZE], GPSRead_t *gps) {
	if (gps->active) {
		char filter[] = "$GPGGA";
		char *senPtr = strstr(sentence, filter); // filters the string to contain
		if (senPtr != NULL) {
			if (strlen(senPtr) > 40) { // if no target lock, GPGGA is only 30 long
				char *tokPtr = strtok(senPtr, DELIM);
				int i = 1; // index for going through GPGGA values
				while (tokPtr != NULL) {
					//printf("%i %s \r\n", i, tokPtr);
					switch (i) { // https://docs.novatel.com/OEM7/Content/Logs/GPGGA.htm
						case 3: {  // latitude / breddegrad [DDmm.mmmm]
							gps->latitude = DDmmmmmm(tokPtr);
							break;
						}
						case 4: {
							if (!strcmp(tokPtr, "N")) {
								// north direction
								gps->latitude = abs(gps->latitude);
							}
							else {
								// south direction
								gps->latitude = -1 * abs(gps->latitude);
							}
							break;
						}
						case 5: {
							gps->longitude = DDDmmmmmm(tokPtr);
							break;
						}
						case 6: {
							if (!strcmp(tokPtr, "E")) {
								// east direction
								gps->longitude = abs(gps->longitude);
							}
							else {
								// west direction
								gps->longitude = -1 * abs(gps->longitude);
							}
							break;
						}
						case 10: { // altitude
							gps->altitude = Altxx(tokPtr);
							break;
						}
						default: {
							break;
						}
					}
					i++;
					tokPtr = strtok(NULL, DELIM); // required to progress strtok tokens
				}
			}
			else {
				printf("Error: GPGGA too short \r\n");
				gps->active = 0;
			}
		}
		else {
			//printf("Line skip, no %s: %s \r\n", filter, sentence);
		}
	}
	else {
		char filter[] = "$GPRMC";
		char *senPtr = strstr(sentence, filter); // filters the string to contain filter
		if (senPtr != NULL) {
			char *tokPtr = strtok(senPtr, DELIM);
			int i = 1; // index for going through GPGGA values
			while (tokPtr != NULL) {
				//printf("%i: %s\r\n", i, tokPtr);
				i++;
				tokPtr = strtok(NULL, DELIM); // required to progress strtok tokens
				switch (i) {
					case 3: {
						if (!strcmp(tokPtr, "A")) {
							gps->active = 1;
							printf("Set GPS active \r\n");
						}
						else {
								printf("Error: No GPS lock %s \r\n", sentence);
						}
						break;
					}
					default: {}
				}
			}
		}
		else {
			//printf("Line skip, no %s \r\n", filter);
		}
	}
}

char sentence[LINEBUFFERSIZE];  // Temporary line buffer
int indx = 0;

void process_uart_data(RingBuffer *rb, GPSRead_t *gps) {
    uint8_t c;

    while (RingBuffer_Read(rb, &c)) {
        if (indx < sizeof(sentence) - 1) {
            sentence[indx++] = c;
        }

        if (c == '\n') {
            sentence[indx] = '\0';  // Null-terminate the string
            printf("Log:   UART Sentence received \r\n");
            getGPGGA(sentence, gps); // den her linje er lidt cray-cray @rasmus ladegaard
            indx = 0;  // Reset for next line
        }
    }
}

void printGPS(GPSRead_t GPS) {
	if (GPS.active) {
		printf("Active: %i ~ Latitude: ", GPS.active);
		printFix(GPS.latitude);
		printf("deg ~ Longitude: ");
		printFix(GPS.longitude);
		printf("deg ~ Altitude: ");
		printFix(GPS.altitude);
		printf("m. \r\n");
	} else {
		printf("Error: GPS inactive \r\n");
	}
}
