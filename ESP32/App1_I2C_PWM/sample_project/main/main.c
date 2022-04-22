#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/ledc.h"
#include "esp_err.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"
#include "driver/i2c.h"

/* PWM Configurations - timer, channel and pins */
#define LEDC_HS_TIMER          LEDC_TIMER_0
#define LEDC_HS_MODE           LEDC_HIGH_SPEED_MODE
#define LEDC_HS_CH0_GPIO       (32)
#define LEDC_HS_CH0_CHANNEL    LEDC_CHANNEL_0
#define LEDC_HS_CH1_GPIO       (33)
#define LEDC_HS_CH1_CHANNEL    LEDC_CHANNEL_1
#define LEDC_HS_CH2_GPIO       (25)
#define LEDC_HS_CH2_CHANNEL    LEDC_CHANNEL_2

#define LEDC_TEST_CH_NUM       (3)
#define LEDC_TEST_DUTY         (4095)
#define LEDC_TEST_FADE_TIME    (500)

/* I2C Configuration */
#define I2C_MASTER_SCL_IO           12                         /*!< GPIO number used for I2C master clock */
#define I2C_MASTER_SDA_IO           14                         /*!< GPIO number used for I2C master data  */
#define I2C_MASTER_NUM              0                          /*!< I2C master i2c port number, the number of i2c peripheral interfaces available will depend on the chip */
#define I2C_MASTER_FREQ_HZ          400000                     /*!< I2C master clock frequency */
#define I2C_MASTER_TX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_RX_BUF_DISABLE   0                          /*!< I2C master doesn't need buffer */
#define I2C_MASTER_TIMEOUT_MS       1000
#define SAMPLE_RATE_MS              500

#define SENSOR_ADDR                 0x4D                       /*!< Slave address of the sensor */


static const char* TAG = "App1";

static esp_err_t sensor_read(uint8_t reg_addr, uint8_t *data, size_t len)
{
    return i2c_master_read_from_device(I2C_MASTER_NUM, SENSOR_ADDR, data, \
    len, I2C_MASTER_TIMEOUT_MS / portTICK_RATE_MS);
}

static esp_err_t i2c_master_init(void) {

    int i2c_master_port = I2C_MASTER_NUM;

    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };

    i2c_param_config(i2c_master_port, &conf);

    return i2c_driver_install(i2c_master_port, conf.mode, \
     I2C_MASTER_RX_BUF_DISABLE, I2C_MASTER_TX_BUF_DISABLE, 0);
}

void app_main(void) {
    
    int ch;
    uint8_t data;
    int duty[3];

    ledc_timer_config_t ledc_timer = {
        .duty_resolution = LEDC_TIMER_12_BIT,  // resolution of PWM duty
        .freq_hz = 10000,                      // frequency of PWM signal
        .speed_mode = LEDC_HS_MODE,            // timer mode
        .timer_num = LEDC_HS_TIMER,            // timer index
        .clk_cfg = LEDC_USE_APB_CLK,           
    };
    
    ledc_timer_config(&ledc_timer);

    ledc_channel_config_t ledc_channel[LEDC_TEST_CH_NUM] = {
        {
            .channel    = LEDC_HS_CH0_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH0_GPIO,             
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
        {
            .channel    = LEDC_HS_CH1_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH1_GPIO,             
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
        {
            .channel    = LEDC_HS_CH2_CHANNEL,
            .duty       = 0,
            .gpio_num   = LEDC_HS_CH2_GPIO,
            .speed_mode = LEDC_HS_MODE,
            .hpoint     = 0,
            .timer_sel  = LEDC_HS_TIMER,
            .flags.output_invert = 0
        },
    };

    // Set LED Controller with previously prepared configuration
    for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
        ledc_channel_config(&ledc_channel[ch]);
    }

    ESP_ERROR_CHECK(i2c_master_init());

    while (1) {
        ESP_ERROR_CHECK(sensor_read(SENSOR_ADDR, &data, 1));
        ESP_LOGI(TAG, "Val = %d", data);

        duty[0] = 0;
        duty[1] = 0;
        duty[2] = 0;
        if (data >= 27) {
            if (data >= 34) duty[2] = 4094;
            else duty[2] = ((26 - data) * -1) * 511; 
        } else if (data >= 22 && data <= 26) {
            duty[1] = ((21 - data) * -1) * 818;
        } else if (data <= 21) {
            if (data <= 14)  duty[0] = 0;
            else duty[0] = ((13 - data) * -1) * 511;
        }

        ESP_LOGI(TAG, "duty = %d, %d, %d", duty[0], duty[1], duty[2]);

        for (ch = 0; ch < LEDC_TEST_CH_NUM; ch++) {
            ledc_set_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel, duty[ch]);
            ledc_update_duty(ledc_channel[ch].speed_mode, ledc_channel[ch].channel);
        }
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}
