
#include "driver/gpio.h"
#include "esp_log.h"

#include "contact_sensor.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "tinyusb.h"
#include "usb.h"
#include "midi.h"





#define TAG "MAIN"


void app_main(){

	init_usbmidi();
	init_contact_sensors();


	
}


