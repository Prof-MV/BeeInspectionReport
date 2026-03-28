/**
 * @file hal_tp.hpp
 * @author Forairaaaaa
 * @brief
 * @version 0.2
 * @date 2023-05-20
 *
 * @copyright Copyright (c) 2023
 *
 * Updated: 2026-03 - Use new I2C driver API for ESP-IDF 5.4+
 */
#pragma once
#include <driver/i2c_master.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <cstring>


/** @brief FT5x06 register map and function codes */
#define FT5x06_ADDR                    (0x38)

#define FT5x06_DEVICE_MODE             (0x00)
#define FT5x06_GESTURE_ID              (0x01)
#define FT5x06_TOUCH_POINTS            (0x02)

#define FT5x06_TOUCH1_EV_FLAG          (0x03)
#define FT5x06_TOUCH1_XH               (0x03)
#define FT5x06_TOUCH1_XL               (0x04)
#define FT5x06_TOUCH1_YH               (0x05)
#define FT5x06_TOUCH1_YL               (0x06)

#define FT5x06_TOUCH2_EV_FLAG          (0x09)
#define FT5x06_TOUCH2_XH               (0x09)
#define FT5x06_TOUCH2_XL               (0x0A)
#define FT5x06_TOUCH2_YH               (0x0B)
#define FT5x06_TOUCH2_YL               (0x0C)

#define FT5x06_TOUCH3_EV_FLAG          (0x0F)
#define FT5x06_TOUCH3_XH               (0x0F)
#define FT5x06_TOUCH3_XL               (0x10)
#define FT5x06_TOUCH3_YH               (0x11)
#define FT5x06_TOUCH3_YL               (0x12)

#define FT5x06_TOUCH4_EV_FLAG          (0x15)
#define FT5x06_TOUCH4_XH               (0x15)
#define FT5x06_TOUCH4_XL               (0x16)
#define FT5x06_TOUCH4_YH               (0x17)
#define FT5x06_TOUCH4_YL               (0x18)

#define FT5x06_TOUCH5_EV_FLAG          (0x1B)
#define FT5x06_TOUCH5_XH               (0x1B)
#define FT5x06_TOUCH5_XL               (0x1C)
#define FT5x06_TOUCH5_YH               (0x1D)
#define FT5x06_TOUCH5_YL               (0x1E)

#define FT5x06_ID_G_THGROUP            (0x80)
#define FT5x06_ID_G_THPEAK             (0x81)
#define FT5x06_ID_G_THCAL              (0x82)
#define FT5x06_ID_G_THWATER            (0x83)
#define FT5x06_ID_G_THTEMP             (0x84)
#define FT5x06_ID_G_THDIFF             (0x85)
#define FT5x06_ID_G_CTRL               (0x86)
#define FT5x06_ID_G_TIME_ENTER_MONITOR (0x87)
#define FT5x06_ID_G_PERIODACTIVE       (0x88)
#define FT5x06_ID_G_PERIODMONITOR      (0x89)
#define FT5x06_ID_G_AUTO_CLB_MODE      (0xA0)
#define FT5x06_ID_G_LIB_VERSION_H      (0xA1)
#define FT5x06_ID_G_LIB_VERSION_L      (0xA2)
#define FT5x06_ID_G_CIPHER             (0xA3)
#define FT5x06_ID_G_MODE               (0xA4)
#define FT5x06_ID_G_PMODE              (0xA5)
#define FT5x06_ID_G_FIRMID             (0xA6)
#define FT5x06_ID_G_STATE              (0xA7)
#define FT5x06_ID_G_FT5201ID           (0xA8)
#define FT5x06_ID_G_ERR                (0xA9)

// I2C pins for M5Dial touchscreen
#define TP_I2C_SDA_PIN  11
#define TP_I2C_SCL_PIN  12


namespace FT3267
{
    static const char* TAG = "ft3267";


    enum ft3267_gesture_t
    {
        ft3267_gesture_none         = 0x00,
        ft3267_gesture_move_up      = 0x10,
        ft3267_gesture_move_left    = 0x14,
        ft3267_gesture_move_down    = 0x18,
        ft3267_gesture_move_right   = 0x1c,
        ft3267_gesture_zoom_in      = 0x48,
        ft3267_gesture_zoom_out     = 0x49,
    };


    struct TouchPoint_t
    {
        uint8_t touch_num = 0;
        int x = -1;
        int y = -1;
    };


    struct Config_t
    {
        int pin_sda = TP_I2C_SDA_PIN;
        int pin_scl = TP_I2C_SCL_PIN;
        uint8_t dev_addr = FT5x06_ADDR;
        uint32_t i2c_freq = 100000;
    };


    class TP_FT3267
    {
        private:
            Config_t _cfg;
            uint8_t _data_buffer[7];
            TouchPoint_t _touch_point_buffer;
            i2c_master_bus_handle_t _i2c_bus_handle = nullptr;
            i2c_master_dev_handle_t _i2c_dev_handle = nullptr;
            bool _initialized = false;


            inline esp_err_t _write_reg(uint8_t reg, uint8_t data)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;
                uint8_t write_buf[2] = {reg, data};
                return i2c_master_transmit(_i2c_dev_handle, write_buf, 2, pdMS_TO_TICKS(200));
            }


            inline esp_err_t _read_reg(uint8_t reg, uint8_t readSize)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;
                return i2c_master_transmit_receive(_i2c_dev_handle, &reg, 1, _data_buffer, readSize, pdMS_TO_TICKS(200));
            }


            inline void _tp_init()
            {
                // Valid touching detect threshold
                _write_reg(FT5x06_ID_G_THGROUP, 70);

                // valid touching peak detect threshold
                _write_reg(FT5x06_ID_G_THPEAK, 60);

                // Touch focus threshold
                _write_reg(FT5x06_ID_G_THCAL, 16);

                // threshold when there is surface water
                _write_reg(FT5x06_ID_G_THWATER, 60);

                // threshold of temperature compensation
                _write_reg(FT5x06_ID_G_THTEMP, 10);

                // Touch difference threshold
                _write_reg(FT5x06_ID_G_THDIFF, 20);

                // Delay to enter 'Monitor' status (s)
                _write_reg(FT5x06_ID_G_TIME_ENTER_MONITOR, 2);

                // Period of 'Active' status (ms)
                _write_reg(FT5x06_ID_G_PERIODACTIVE, 12);

                // Timer to enter 'idle' when in 'Monitor' (ms)
                _write_reg(FT5x06_ID_G_PERIODMONITOR, 40);
            }


        public:
            TP_FT3267()
            {
                memset(_data_buffer, 0, sizeof(_data_buffer));
            }

            ~TP_FT3267()
            {
                if (_i2c_dev_handle) {
                    i2c_master_bus_rm_device(_i2c_dev_handle);
                }
                if (_i2c_bus_handle) {
                    i2c_del_master_bus(_i2c_bus_handle);
                }
            }


            /* Config */
            inline Config_t getConfig() { return _cfg; }
            inline void setConfig(const Config_t& cfg) { _cfg = cfg; }


            inline bool init()
            {
                ESP_LOGI(TAG, "init tp ft3267 with new I2C driver");

                /* Interrupt pin */
                gpio_reset_pin(GPIO_NUM_14);
                gpio_set_direction(GPIO_NUM_14, GPIO_MODE_INPUT);
                gpio_set_pull_mode(GPIO_NUM_14, GPIO_PULLUP_ONLY);

                /* Initialize I2C bus with new driver */
                i2c_master_bus_config_t bus_config = {};
                bus_config.clk_source = I2C_CLK_SRC_DEFAULT;
                bus_config.i2c_port = I2C_NUM_0;
                bus_config.scl_io_num = (gpio_num_t)_cfg.pin_scl;
                bus_config.sda_io_num = (gpio_num_t)_cfg.pin_sda;
                bus_config.glitch_ignore_cnt = 7;
                bus_config.flags.enable_internal_pullup = true;

                esp_err_t ret = i2c_new_master_bus(&bus_config, &_i2c_bus_handle);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to create I2C master bus: %s", esp_err_to_name(ret));
                    return false;
                }

                /* Add touchscreen device */
                i2c_device_config_t dev_config = {};
                dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
                dev_config.device_address = _cfg.dev_addr;
                dev_config.scl_speed_hz = _cfg.i2c_freq;

                ret = i2c_master_bus_add_device(_i2c_bus_handle, &dev_config, &_i2c_dev_handle);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to add I2C device: %s", esp_err_to_name(ret));
                    i2c_del_master_bus(_i2c_bus_handle);
                    _i2c_bus_handle = nullptr;
                    return false;
                }

                _initialized = true;
                _tp_init();

                return true;
            }


            inline uint8_t getTouchPointsNum()
            {
                _data_buffer[0] = 0;
                _read_reg(FT5x06_TOUCH_POINTS, 1);
                return _data_buffer[0];
            }


            inline const TouchPoint_t& readPos()
            {
                _touch_point_buffer.touch_num = 0;
                _touch_point_buffer.x = -1;
                _touch_point_buffer.y = -1;

                /* Get touch num */
                _read_reg(FT5x06_TOUCH_POINTS, 1);
                _data_buffer[0] = _data_buffer[0] & 0x0F;
                _touch_point_buffer.touch_num = _data_buffer[0];

                /* Get postion */
                if (_data_buffer[0] != 0)
                {
                    _read_reg(FT5x06_TOUCH1_XH, 4);
                    _touch_point_buffer.x = ((_data_buffer[0] & 0x0f) << 8) + _data_buffer[1];
                    _touch_point_buffer.y = ((_data_buffer[2] & 0x0f) << 8) + _data_buffer[3];
                }

                return _touch_point_buffer;
            }


            inline bool isTouched()
            {
                return (getTouchPointsNum() > 0);
            }


            /**
             * @brief Update internal touch point buffer
             *
             */
            inline void update()
            {
                readPos();
            }


            /**
             * @brief Get internal touch point buffer
             *
             * @return TouchPoint_t
             */
            inline TouchPoint_t getTouchPointBuffer()
            {
                return _touch_point_buffer;
            }

            /**
             * @brief Get the I2C bus handle (for sharing with other devices like RTC)
             */
            inline i2c_master_bus_handle_t getBusHandle() { return _i2c_bus_handle; }
    };


}
