/**
 * @file hal.h
 * @author Forairaaaaa
 * @brief
 * @version 0.1
 * @date 2023-06-28
 *
 * @copyright Copyright (c) 2023
 *
 */
#pragma once
#define LVGL_ENABLE 0
#define RFID_ENABLE 0  // Disabled: RC522 uses legacy I2C driver incompatible with ESP-IDF 5.4+

#include "hal_common_define.h"
#include "display/hal_display.hpp"
#include "buzzer/hal_buzzer.hpp"
#include "utils/ESP32Encoder/src/ESP32Encoder.h"
#include "utils/Button/Button.h"
#if LVGL_ENABLE
#include "lvgl/hal_lvgl.hpp"
#endif
#include "tp/hal_tp.hpp"
#include "rtc/hal_rtc.hpp"
#if RFID_ENABLE
#include "rfid/hal_rfid.h"
#endif

namespace HAL
{
    class HAL
    {
    private:
        bool _is_low_current;

        void _display_init();
        void _encoder_init();
        void _buzz_init();

    public:
        HAL() = default;
        ~HAL() = default;

        /* Display */
        LGFX_StampRing display;
        LGFX_Sprite* canvas;

        /* Buzzer */
        BUZZER::BUZZER buzz;

/* Lvgl */
#if LVGL_ENABLE
        LVGL_PORT lvgl;
#endif

        /* Encoder */
        ESP32Encoder encoder;

        /* Touchpad */
        FT3267::TP_FT3267 tp;

        /* RTC */
        PCF8563::PCF8563 rtc;

        /* RFID Reader */
#if RFID_ENABLE
        RFID::RfidReader rfid;
#endif

        /**
         * @brief Init
         *
         */
        void init();

        void powerOn();
        void powerOff();

        static void _encoder_moved_callback(ESP32Encoder* encoder, void* userData);
        static void _encoder_button_pressed_callback(Button* button, void* userData);
    };

    // Legacy I2C functions removed - using new I2C driver
    void encoder_test(HAL& hal);
    void tp_test(HAL& hal);
    void rtc_test(HAL& hal);
} // namespace HAL
