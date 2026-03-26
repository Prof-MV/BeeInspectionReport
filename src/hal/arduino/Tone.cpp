#include "Arduino.h"
#include "driver/ledc.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"

#define delay(ms) vTaskDelay(pdMS_TO_TICKS(ms))

static const char* TAG = "Tone";

// Use LEDC timer 3 and channel 6 to avoid conflicts with display (backlight uses channel 7)
#define BUZZER_LEDC_TIMER      LEDC_TIMER_3
#define BUZZER_LEDC_CHANNEL    LEDC_CHANNEL_6
#define BUZZER_LEDC_MODE       LEDC_LOW_SPEED_MODE
#define BUZZER_LEDC_DUTY_RES   LEDC_TIMER_10_BIT
#define BUZZER_DUTY_50_PERCENT 512

static TaskHandle_t _tone_task = NULL;
static QueueHandle_t _tone_queue = NULL;
static int8_t _pin = -1;
static bool _initialized = false;

typedef enum {
    TONE_START,
    TONE_END
} tone_cmd_t;

typedef struct {
    tone_cmd_t tone_cmd;
    uint8_t pin;
    unsigned int frequency;
    unsigned long duration;
} tone_msg_t;

static bool buzzer_init(uint8_t pin, uint32_t frequency) {
    // Configure timer
    ledc_timer_config_t timer_conf = {
        .speed_mode = BUZZER_LEDC_MODE,
        .duty_resolution = BUZZER_LEDC_DUTY_RES,
        .timer_num = BUZZER_LEDC_TIMER,
        .freq_hz = frequency,
        .clk_cfg = LEDC_AUTO_CLK,
        .deconfigure = false
    };

    esp_err_t err = ledc_timer_config(&timer_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Timer config failed: %s", esp_err_to_name(err));
        return false;
    }

    // Configure channel
    ledc_channel_config_t channel_conf = {
        .gpio_num = pin,
        .speed_mode = BUZZER_LEDC_MODE,
        .channel = BUZZER_LEDC_CHANNEL,
        .intr_type = LEDC_INTR_DISABLE,
        .timer_sel = BUZZER_LEDC_TIMER,
        .duty = 0,
        .hpoint = 0,
        .sleep_mode = LEDC_SLEEP_MODE_NO_ALIVE_NO_PD,
        .flags = {.output_invert = 0}
    };

    err = ledc_channel_config(&channel_conf);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Channel config failed: %s", esp_err_to_name(err));
        return false;
    }

    return true;
}

static bool buzzer_set_frequency(uint32_t frequency) {
    esp_err_t err = ledc_set_freq(BUZZER_LEDC_MODE, BUZZER_LEDC_TIMER, frequency);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Set freq failed: %s", esp_err_to_name(err));
        return false;
    }
    return true;
}

static void buzzer_on() {
    ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, BUZZER_DUTY_50_PERCENT);
    ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
}

static void buzzer_off() {
    ledc_set_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL, 0);
    ledc_update_duty(BUZZER_LEDC_MODE, BUZZER_LEDC_CHANNEL);
}

static void tone_task(void*) {
    tone_msg_t tone_msg;
    while (1) {
        xQueueReceive(_tone_queue, &tone_msg, portMAX_DELAY);
        switch (tone_msg.tone_cmd) {
            case TONE_START:
                ESP_LOGD(TAG, "TONE_START: pin=%d, freq=%u, dur=%lu",
                         tone_msg.pin, tone_msg.frequency, tone_msg.duration);

                // Initialize on first use or pin change
                if (!_initialized || _pin != tone_msg.pin) {
                    if (!buzzer_init(tone_msg.pin, tone_msg.frequency)) {
                        break;
                    }
                    _pin = tone_msg.pin;
                    _initialized = true;
                } else {
                    // Just update frequency
                    buzzer_set_frequency(tone_msg.frequency);
                }

                // Turn on
                buzzer_on();

                // Wait for duration then turn off
                if (tone_msg.duration) {
                    delay(tone_msg.duration);
                    buzzer_off();
                }
                break;

            case TONE_END:
                ESP_LOGD(TAG, "TONE_END");
                buzzer_off();
                break;

            default:
                break;
        }
    }
}

static int tone_init() {
    if (_tone_queue == NULL) {
        _tone_queue = xQueueCreate(128, sizeof(tone_msg_t));
        if (_tone_queue == NULL) {
            ESP_LOGE(TAG, "Could not create tone queue");
            return 0;
        }
    }

    if (_tone_task == NULL) {
        xTaskCreate(tone_task, "toneTask", 3500, NULL, 1, &_tone_task);
        if (_tone_task == NULL) {
            ESP_LOGE(TAG, "Could not create tone task");
            return 0;
        }
    }
    return 1;
}

namespace ARDUINO {

void noTone(uint8_t pin) {
    if (_pin == pin) {
        if (tone_init()) {
            tone_msg_t tone_msg = {
                .tone_cmd = TONE_END,
                .pin = pin,
                .frequency = 0,
                .duration = 0,
            };
            xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
        }
    }
}

void tone(uint8_t pin, unsigned int frequency, unsigned long duration) {
    ESP_LOGD(TAG, "tone: pin=%d, freq=%u, dur=%lu", pin, frequency, duration);
    if (_pin == -1 || _pin == pin) {
        if (tone_init()) {
            tone_msg_t tone_msg = {
                .tone_cmd = TONE_START,
                .pin = pin,
                .frequency = frequency,
                .duration = duration,
            };
            xQueueSend(_tone_queue, &tone_msg, portMAX_DELAY);
        }
    } else {
        ESP_LOGE(TAG, "Tone still running on pin %d", _pin);
    }
}

}
