#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/spi_master.h"
#include "driver/gpio.h"
#include "driver/dac.h"

#include "sdkconfig.h"
#include "esp_log.h"
#include "eeprom.h"

static const char *TAG = "App2";

#define	EEPROM_MODEL "24LC040A"

void dump(uint8_t *dt, int n)
{
	uint16_t clm = 0;
	uint8_t data;
	uint32_t saddr =0;
	uint32_t eaddr =n-1;

	printf("--------------------------------------------------------\n");
	uint32_t addr;
	for (addr = saddr; addr <= eaddr; addr++) {
		data = dt[addr];
		if (clm == 0) {
			printf("%05x: ",addr);
		}

		printf("%02x ",data);
		clm++;
		if (clm == 16) {
			printf("| \n");
			clm = 0;
		}
	}
	printf("--------------------------------------------------------\n");
}


void app_main(void)
{
    ESP_LOGI(TAG, "EEPROM_MODEL=%s", EEPROM_MODEL);
	EEPROM_t dev;
	esp_err_t ret;
	spi_master_init(&dev);
	int32_t totalBytes = eeprom_TotalBytes(&dev);
	ESP_LOGI(TAG, "totalBytes=%d Bytes",totalBytes);

	//enable dac module
	ret = dac_output_enable(DAC_CHANNEL_2);
	ESP_ERROR_CHECK(ret);

	// Get Status Register
	uint8_t reg;
	ret = eeprom_ReadStatusReg(&dev, &reg);
	if (ret != ESP_OK) {
		ESP_LOGI(TAG, "ReadStatusReg Fail %d",ret);
		while(1) { vTaskDelay(1); }
	} 
	ESP_LOGI(TAG, "readStatusReg : 0x%02x", reg);

	uint8_t wdata[128];
	int len;
	
    // write all bits 1
    for (int i=0; i<128; i++) {
		wdata[i]=0xff;	
	}

    //write increase
	// for (int i=0; i<128; i++) {
	// 	wdata[i]=i;	
	// }  

    //write decrease
	// for (int i=0; i<128; i++) {
	// 	wdata[i]=0xff-i;	
	// }

	
	//write random values 

	// for (int i=0; i<128; i++) {
	// 	wdata[i] = (uint8_t) esp_random() & 0xff;
	// }

	for (int addr=0; addr<128;addr++) {
		len =  eeprom_WriteByte(&dev, addr, wdata[addr]);
		ESP_LOGI(TAG, "WriteByte(addr=%d) len=%d: data=%d", addr, len, wdata[addr]);
		if (len != 1) {
			ESP_LOGI(TAG, "WriteByte Fail addr=%d", addr);
			while(1) { vTaskDelay(1); }
		}
	}

	// Read 128 byte from Address=0
	uint8_t rbuf[128];
	memset(rbuf, 0, 128);
	len =  eeprom_Read(&dev, 0, rbuf, 128);
	if (len != 128) {
		ESP_LOGI(TAG, "Read Fail");
		while(1) { vTaskDelay(1); }
	}
	ESP_LOGI(TAG, "Read Data: len=%d", len);
	dump(rbuf, 128);

	uint8_t d;
	for(int i = 0; i < 128; i++) {
		d = rbuf[i];
		ESP_LOGI(TAG, "value: %d", d);
		dac_output_voltage(DAC_CHANNEL_2, d);
		vTaskDelay(3000 / portTICK_PERIOD_MS);
	}
}