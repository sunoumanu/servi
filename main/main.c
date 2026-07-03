#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "driver/ledc.h"

static const char *TAG = "SERVO_CONTROL";

// Pinout mapping for Seed Studio XIAO ESP32-S3 (D0 is GPIO 1)
#define SERVO_MIN_DEGREE        0
#define SERVO_MAX_DEGREE        180

// 50Hz frequency means a 20ms total period
#define SERVO_PWM_FREQ_HZ       50   
// 14-bit resolution gives us a range of 0 to 16383 positions for precise control
#define SERVO_PWM_RESOLUTION    LEDC_TIMER_14_BIT 

/* 
 * Math mapping for duty cycles on a 14-bit timer (2^14 = 16384 total steps):
 * - 20ms period total = 16384 steps
 * - 1ms pulse (Min)  = (1ms / 20ms) * 16384 = ~819 steps
 * - 2ms pulse (Max)  = (2ms / 20ms) * 16384 = ~1638 steps
 */
#define SERVO_MIN_PULSE_DUTY    410   // Approx 0.5ms pulse width (0 degrees)
#define SERVO_MAX_PULSE_DUTY    2048  // Approx 2.5ms pulse width (180 degrees)

/**
 * @brief Simple helper function to convert target degrees to the exact duty calculation
 */
static uint32_t degree_to_duty(int degree) {
    if (degree < SERVO_MIN_DEGREE) degree = SERVO_MIN_DEGREE;
    if (degree > SERVO_MAX_DEGREE) degree = SERVO_MAX_DEGREE;

    return SERVO_MIN_PULSE_DUTY + 
           ((SERVO_MAX_PULSE_DUTY - SERVO_MIN_PULSE_DUTY) * degree) / (SERVO_MAX_DEGREE - SERVO_MIN_DEGREE);
}

void app_main(void)
{
    ESP_LOGI(TAG, "Initializing PWM hardware for SG90 Servo...");

    // 1. Configure the LEDC Hardware Timer
    ledc_timer_config_t ledc_timer = {
        .speed_mode       = LEDC_LOW_SPEED_MODE,
        .timer_num        = LEDC_TIMER_0,
        .duty_resolution  = SERVO_PWM_RESOLUTION,
        .freq_hz          = SERVO_PWM_FREQ_HZ,
        .clk_cfg          = LEDC_AUTO_CLK
    };
    ESP_ERROR_CHECK(ledc_timer_config(&ledc_timer));

    // 2. Configure the LEDC PWM Channel pointing to Pin D0 (GPIO 1)
    ledc_channel_config_t ledc_channel = {
        .speed_mode     = LEDC_LOW_SPEED_MODE,
        .channel        = LEDC_CHANNEL_0,
        .timer_sel      = LEDC_TIMER_0,
        .intr_type      = LEDC_INTR_DISABLE,
        .gpio_num       = 1, // Labeled D0 on your Seeed Studio board back
        .duty           = degree_to_duty(0), // Start at position 0
        .hpoint         = 0
    };
    ESP_ERROR_CHECK(ledc_channel_config(&ledc_channel));

    ESP_LOGI(TAG, "Starting Sweep Loop...");

    while (1) {
        // Sweep from 0 to 180 degrees
        ESP_LOGI(TAG, "Sweeping Forward...");
        for (int angle = 0; angle <= 180; angle += 10) {
            uint32_t duty = degree_to_duty(angle);
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
            vTaskDelay(pdMS_TO_TICKS(50)); // Controls rotation speed
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Pause 1 second at full rotation

        // Sweep back from 180 to 0 degrees
        ESP_LOGI(TAG, "Sweeping Backward...");
        for (int angle = 180; angle >= 0; angle -= 10) {
            uint32_t duty = degree_to_duty(angle);
            ESP_ERROR_CHECK(ledc_set_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0, duty));
            ESP_ERROR_CHECK(ledc_update_duty(LEDC_LOW_SPEED_MODE, LEDC_CHANNEL_0));
            vTaskDelay(pdMS_TO_TICKS(50));
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Pause 1 second at start position
    }
}