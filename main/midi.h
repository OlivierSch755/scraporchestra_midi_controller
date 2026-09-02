#pragma once

#include "tinyusb.h"


// Basic MIDI Messages
#define NOTE_OFF 0x80
#define NOTE_ON  0x90
#define CC 0xB0



void sendCC(uint8_t cc_num, uint8_t value, uint8_t channel, uint8_t cable_num);
void noteOn( uint8_t note, uint8_t velocity, uint8_t channel, uint8_t cable_num );
void noteOff(uint8_t note, uint8_t channel, uint8_t cable_num );