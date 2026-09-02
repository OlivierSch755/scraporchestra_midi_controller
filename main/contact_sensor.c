#include "driver/gpio.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "contact_sensor.h"
#include "midi.h"

#define TAG "CONTACT"

static uint8_t byteCounter = 0;
static uint8_t bitCounter = 0;
static int pressThreshold;
static int releaseThreshold;
static esp_timer_handle_t timer;

gpio_num_t pinNumbers[CONTACT_SENSOR_NB_INPUTS] = {
  GPIO_NUM_7, 
  GPIO_NUM_15, 
  GPIO_NUM_16,
  GPIO_NUM_17,
  GPIO_NUM_18,
  GPIO_NUM_8
};


// should not be hardcoded here
uint8_t on_midi_msg[CONTACT_SENSOR_NB_INPUTS][3] = {
	{36,90,9},
	{38,75,9}, 
	{44,80,9},
	{46,80,9},
	{49,80,9},
	{52,80,9}
};
// should not be hardcoded here
uint8_t off_midi_msg[CONTACT_SENSOR_NB_INPUTS][2] = {
	{36,9},
	{38,9}, 
	{44,9},
	{46,9},
	{49,9},
	{52,9}
};
// should not be hardcoded here
contact_sensor_t contact_sensors[CONTACT_SENSOR_NB_INPUTS];





void init_contact_sensors(){
	
	float thresholdPerc = CONTACT_SENSOR_SWITCH_THRESHOLD_OFFSET_PERC;
	float thresholdCenterBias = CONTACT_SENSOR_SWITCH_THRESHOLD_CENTER_BIAS/50.0;
	float pressThresholdAmount = (CONTACT_SENSOR_BUFFER_LENGTH * 8) * (thresholdPerc / 100.0);
	float thresholdCenter = ( (CONTACT_SENSOR_BUFFER_LENGTH * 8) / 2.0 ) * (thresholdCenterBias);
	
	pressThreshold = thresholdCenter + pressThresholdAmount;
	releaseThreshold = thresholdCenter - pressThresholdAmount;

	ESP_LOGD(TAG, "pressThreshold : %d", pressThreshold );
	ESP_LOGD(TAG, "releaseThreshold : %d", releaseThreshold );
	
	for (int i=0; i<CONTACT_SENSOR_NB_INPUTS; i++) {
		
		contact_sensor_t *contact_sensor = &contact_sensors[i];
		contact_sensor->gpio_num = pinNumbers[i];
		memcpy(contact_sensor->on_midi_msg, on_midi_msg[i], sizeof(contact_sensor->on_midi_msg));
		memcpy(contact_sensor->off_midi_msg, off_midi_msg[i], sizeof(contact_sensor->off_midi_msg));

		for (int j=0; j<CONTACT_SENSOR_BUFFER_LENGTH; j++) {
		  contact_sensor->measurementBuffer[j] = 0;
		}
		
		contact_sensor->oldestMeasurement = 0;
		contact_sensor->bufferSum = 0;
		contact_sensor->pressed = false;
		
		gpio_set_level( contact_sensor->gpio_num, 0 );	
		
		// configure GPIO for this sensor
		gpio_config_t cfg = {
			.pin_bit_mask = 1ULL << contact_sensor->gpio_num,	
			.mode = GPIO_MODE_INPUT,
		};
		gpio_config(&cfg);
	}
	
    const esp_timer_create_args_t timer_args = {
        .callback = &contact_sensor_cycle,
        .name = "sensor_timer"
    };

    esp_timer_create(&timer_args, &timer);
    esp_timer_start_periodic(timer, CONTACT_SENSOR_SAMPLING_RATE_US);

}

static void updateMeasurementBuffers() {
	for (int i=0; i<CONTACT_SENSOR_NB_INPUTS; i++) {
		
		contact_sensor_t *contact_sensor = &contact_sensors[i];
		uint8_t *currentByte_ref = &contact_sensor->measurementBuffer[byteCounter];
		uint8_t currentByte = *currentByte_ref;
		
		contact_sensor->oldestMeasurement = (currentByte >> bitCounter) & 0x01; 
		bool newMeasurement = !gpio_get_level(contact_sensor->gpio_num);
		if (newMeasurement) {
		  currentByte |= (1<<bitCounter);
		} 
		else {
		  currentByte &= ~(1<<bitCounter);
		}
		*currentByte_ref = currentByte;
	}
}


static void updateBufferSums() {
	for (int i=0; i<CONTACT_SENSOR_NB_INPUTS ;i++) {
		contact_sensor_t *contact_sensor = &contact_sensors[i];
		uint8_t *currentByte_ref = &contact_sensor->measurementBuffer[byteCounter];
		uint8_t currentByte = *currentByte_ref;
		bool currentMeasurement = (currentByte >> bitCounter) & 0x01; 
		if (currentMeasurement) {
			contact_sensor->bufferSum++;
		}
		if (contact_sensor->oldestMeasurement) {
			contact_sensor->bufferSum--;
		}
	} 		
}


static void updateBufferIndex() {
	bitCounter++;
	if (bitCounter == 8) {
		bitCounter = 0;
		byteCounter++;
		if (byteCounter == CONTACT_SENSOR_BUFFER_LENGTH) {
			byteCounter = 0;
		}
	}
}

// edge callback should not be hardcoded here
static void updateInputStates() {
	
	for (int i=0; i<CONTACT_SENSOR_NB_INPUTS; i++) {
		contact_sensor_t *contact_sensor = &contact_sensors[i];
		
		if (contact_sensor->pressed) {
			if (contact_sensor->bufferSum < releaseThreshold) {  
				contact_sensor->pressed = false;
				ESP_LOGI(TAG, "OFF %d",contact_sensor->on_midi_msg[0]);
				noteOff(
					contact_sensor->off_midi_msg[0],
					contact_sensor->off_midi_msg[1],
					0
				);
			}
		} 
		else if (!contact_sensor->pressed) {
			if (contact_sensor->bufferSum > pressThreshold){ 
				contact_sensor->pressed = true; 
				ESP_LOGI(TAG, "ON %d",contact_sensor->on_midi_msg[0]);
				noteOn(
					contact_sensor->on_midi_msg[0],
					contact_sensor->on_midi_msg[1],
					contact_sensor->on_midi_msg[2],
					0
				);
			
			}
		}
	}
}


void contact_sensor_cycle(void* arg)
{
	updateMeasurementBuffers();
	updateBufferSums();
	updateBufferIndex();
	updateInputStates();
}




