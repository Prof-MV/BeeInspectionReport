/**
 * @file hal.cpp
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#include "hal.h"
#include <esp_log.h>
#include <driver/gpio.h>
#include <string.h>
#include "file_system/wear_levelling.h"

#define delay(ms) vTaskDelay(pdMS_TO_TICKS(ms))

namespace HAL
{
    const char* TAG = "hal";

    void HAL::_display_init()
    {
        ESP_LOGI(TAG, "display init");
        display.init();

        /* Init tp right after lcd (sharing rst pin) */
        tp.init();

        /* Init RTC using shared I2C bus from touchpad */
        if (tp.getBusHandle() != nullptr) {
            if (!rtc.init(tp.getBusHandle())) {
                ESP_LOGW(TAG, "RTC init failed");
            }
        } else {
            ESP_LOGW(TAG, "Cannot init RTC - no I2C bus handle");
        }

        // display.setColorDepth(16);
        // display.setTextSize(3);
        // // display.setTextSize(1);
        // // display.setTextColor((uint32_t)0xF3E9D2);
        // // display.setFont(&fonts::efontCN_24);
        // display.setTextColor(TFT_WHITE);
        // display.drawCenterString("StampRing", display.width() / 2, display.height() / 2 - 32);
        // for (int i = 0; i < 128; i++)
        // {
        //     display.setBrightness(i);
        //     delay(4);
        // }
        // delay(200);

        /* Create canvas */
        canvas = new LGFX_Sprite(&display);
        canvas->createSprite(display.width(), display.height());
        // printf("free block: %d\n", heap_caps_get_largest_free_block(MALLOC_CAP_INTERNAL));

        /* Simple start up anim */
        display.setBrightness(0);
        delay(100);

        canvas->setTextSize(1.5);
        canvas->setTextColor((uint32_t)0xF3E9D2);
        canvas->setFont(&fonts::efontCN_24);
        canvas->drawCenterString("M5-DIAL", display.width() / 2, display.height() / 2 - 24);
        canvas->pushSprite(0, 0);
        canvas->setTextSize(1);
        for (int i = 0; i < 128; i++)
        {
            display.setBrightness(i);
            delay(4);
        }
        delay(200);

        // display.setBrightness(128);
    }

    static int _last_count = 0;
    void HAL::_encoder_moved_callback(ESP32Encoder* encoder, void* userData)
    {
        int new_shit = static_cast<int>(encoder->getCount()) / 2;
        // Encoder feedback tones disabled to avoid LEDC timer conflicts
        // Apps can provide their own feedback via buzzNavigate() calls
        _last_count = new_shit;
    }

    void HAL::_encoder_button_pressed_callback(Button* button, void* userData)
    {
        // Button feedback tone disabled - apps provide their own feedback
        (void)button;
        (void)userData;
    }

    void HAL::_encoder_init()
    {
        /* Encoder */
        // encoder.init(41, 40, 42);
        // encoder.init(HAL_PIN_ENCODER_A, HAL_PIN_ENCODER_B, HAL_PIN_PWR_WAKE_UP);
        encoder.attachHalfQuad(HAL_PIN_ENCODER_A, HAL_PIN_ENCODER_B);

        /* Set moved callback */
        // encoder.setMovedCallback(_encoder_moved_callback, this);
        encoder.setMovedCallback(_encoder_moved_callback, this);

        /* Set button pressed callback */
        // encoder.btn.setPressedCallback(_encoder_button_pressed_callback, this);
        encoder.btn.setPin(HAL_PIN_PWR_WAKE_UP);
        encoder.btn.setPressedCallback(_encoder_button_pressed_callback, this);
    }

    void HAL::_buzz_init()
    {
        /* Buzz */
        // buzz.init(3);
        buzz.init(HAL_PIN_BUZZER);

        // buzz.tone(4000, 50);
        // delay(100);
        // buzz.tone(4000, 50);
    }

    void HAL::init()
    {
        ESP_LOGI(TAG, "init");

        /* Start power holding */
        powerOn();

        _encoder_init();

        /* Init i2c port 0 (for Tp) */
        // i2c_init(I2C_NUM_0, 11, 12, 100000, true);
        // i2c_init(I2C_NUM_0, HAL_PIN_TP_I2C_SDA, HAL_PIN_TP_I2C_SCL, 100000, true);  // DISABLED: Legacy I2C conflicts with driver_ng

        /* Display init */
        _display_init();

        _buzz_init();

// /* Init file system */
// wear_levelling_init();

/* Init lvgl */
#if LVGL_ENABLE
        lvgl.init(&display, &encoder, &tp);
#endif

#if RFID_ENABLE
        /* Init RFID reader (uses I2C_NUM_0, same bus as touchscreen) */
        if (!rfid.init(I2C_NUM_0)) {
            ESP_LOGW(TAG, "RFID reader not detected - may not be connected");
        }
#endif

        // _encoder_init();
    }

    void HAL::powerOn()
    {
        gpio_num_t pin_pwr_holding = (gpio_num_t)HAL_PIN_PWR_HOLDING;

        gpio_reset_pin(pin_pwr_holding);
        gpio_set_direction(pin_pwr_holding, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(pin_pwr_holding, GPIO_PULLUP_PULLDOWN);

        /* Keep power on */
        gpio_set_level(pin_pwr_holding, 1);
    }

    void HAL::powerOff()
    {
        gpio_num_t pin_pwr_holding = (gpio_num_t)HAL_PIN_PWR_HOLDING;

        gpio_reset_pin(pin_pwr_holding);
        gpio_set_direction(pin_pwr_holding, GPIO_MODE_OUTPUT);
        gpio_set_pull_mode(pin_pwr_holding, GPIO_PULLUP_PULLDOWN);

        /* Release power holding */
        gpio_set_level(pin_pwr_holding, 0);
    }

    // Legacy i2c_init removed - using new I2C driver in hal_tp.hpp

    void encoder_test(HAL& hal)
    {
        // int shit = 0;
        // int old_shit = 0;
        // hal.encoder.resetPosition();

        // while (1)
        // {
        //     if (hal.encoder.wasMoved())
        //     {
        //         // printf("%d %d\n", hal.encoder.btn.read(), hal.encoder.getPosition());
        //         if (hal.encoder.getPosition() > old_shit)
        //         {
        //             shit++;
        //         }
        //         else
        //         {
        //             shit--;
        //         }
        //         printf("%d\n", shit);
        //         old_shit = hal.encoder.getPosition();
        //     }

        //     if (hal.encoder.btn.pressed())
        //     {
        //         // hal.encoder.resetPosition();
        //         // old_shit = 0;
        //         // printf("%d %d\n", hal.encoder.btn.read(), hal.encoder.getPosition());

        //         shit = 0;
        //         printf("%d\n", shit);

        //     }

        //     delay(1);
        // }
    }

    void tp_test(HAL& hal)
    {
        // // i2c_scan(I2C_NUM_0);
        // // while (1)
        // // {
        // //     i2c_scan(I2C_NUM_0);
        // //     delay(500);
        // // }

        // hell:
        // hal.display.fillScreen(TFT_WHITE);
        // hal.display.fillCircle(hal.display.width() / 2, hal.display.height() / 2, 2, TFT_WHITE);
        // hal.display.drawCircle(hal.display.width() / 2, hal.display.height() / 2, hal.display.width() / 2 - 1, TFT_GREEN);
        // hal.display.drawCircle(hal.display.width() / 2, hal.display.height() / 2, hal.display.width() / 4, TFT_GREEN);
        // hal.display.drawLine(0, hal.display.height() / 2, hal.display.width(), hal.display.height() / 2, TFT_GREEN);
        // hal.display.drawLine(hal.display.width() / 2, 0, hal.display.width() / 2, hal.display.height(), TFT_GREEN);

        // // uint8_t shit = 0;
        // while (1)
        // {
        //     // printf("%d %d %d\n", hal.tp.readPos().touch_num, hal.tp.readPos().x, hal.tp.readPos().y);
        //     // delay(5);

        //     if (hal.tp.isTouched())
        //     {
        //         hal.tp.update();
        //         printf("%d %d %d\n", hal.tp.getTouchPointBuffer().touch_num, hal.tp.getTouchPointBuffer().x,
        //         hal.tp.getTouchPointBuffer().y); hal.display.fillCircle(hal.tp.getTouchPointBuffer().x,
        //         hal.tp.getTouchPointBuffer().y, 2, TFT_BLUE);
        //     }

        //     if (hal.btn.pressed())
        //     {
        //         goto hell;
        //     }

        //     // shit++;
        //     // printf("%d\n", shit);
        //     delay(5);
        // }
    }

    // Legacy i2c_scan removed - using new I2C driver

    void rtc_test(HAL& hal)
    {
        // while (1)
        // {
        //     if (hal.encoder.btn.pressed())
        //         break;

        //     delay(50);
        // }

        // tm my_time;
        // while (1)
        // {
        //     hal.rtc.getTime(my_time);
        //     printf("%d:%d:%d\n", my_time.tm_hour, my_time.tm_min, my_time.tm_sec);

        //     delay(1000);
        // }
    }
} // namespace HAL
