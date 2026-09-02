#pragma once

#include <stdint.h>
#include <stdbool.h>


#define CONTACT_SENSOR_NB_INPUTS 6

// SAMPLING RATE (MICRO SECONDS) 
#define CONTACT_SENSOR_SAMPLING_RATE_US 744

// NOISE CANCELLATION 
#define CONTACT_SENSOR_SWITCH_THRESHOLD_OFFSET_PERC  5    // number between 1 and 49
                                           // larger value protects better against noise oscillations, but makes it harder to press and release
                                           // recommended values are between 2 and 20
                                           // default value is 5

#define CONTACT_SENSOR_SWITCH_THRESHOLD_CENTER_BIAS 55   // number between 1 and 99
                                          // larger value makes it easier to "release" keys, but harder to "press"
                                          // smaller value makes it easier to "press" keys, but harder to "release"
                                          // recommended values are between 30 and 70
                                          // 50 is "middle" GPIO volt center ( 1/2 * 5V for ATMega32 VS 1/2 * 3.3V for ESP32)
										  // Default MaKeyMaKey was 55
                                          // 100 = 5V / 3.3V (never use this high)
                                          // 0 = 0 V (never use this low
                                          

#define CONTACT_SENSOR_BUFFER_LENGTH    3     // 3 bytes gives us 24 samples






typedef struct {
	bool oldestMeasurement;
	uint8_t measurementBuffer[CONTACT_SENSOR_BUFFER_LENGTH]; 
	uint8_t bufferSum;
	bool pressed;
	gpio_num_t gpio_num;
	uint8_t on_midi_msg[3];
	uint8_t off_midi_msg[3];
} contact_sensor_t;

void contact_sensor_cycle(void* arg);
void init_contact_sensors();