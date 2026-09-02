

#include "midi.h"




void sendCC(uint8_t cc_num, uint8_t value, uint8_t channel, uint8_t cable_num) {
    if (tud_midi_mounted()) {
        uint8_t cc_msg[3] = {CC | channel, cc_num, value};
        tud_midi_stream_write(cable_num, cc_msg, 3);
    }
}


void noteOn( uint8_t note, uint8_t velocity, uint8_t channel, uint8_t cable_num ){
	if (tud_midi_mounted()) {
		uint8_t note_on[3] = {NOTE_ON | channel, note, velocity};
		tud_midi_stream_write(cable_num, note_on, 3);
	}
}
void noteOff(uint8_t note, uint8_t channel, uint8_t cable_num ){
	if (tud_midi_mounted()) {
		uint8_t note_off[3] = {NOTE_OFF | channel, note, 0};
		tud_midi_stream_write(cable_num, note_off, 3);
	}
}
