/**
 * @file hal_rtc.hpp
 * @author Forairaaaaa
 * @brief Thanks to https://github.com/nopnop2002/esp-idf-pcf8563
 * @version 0.2
 * @date 2023-05-21
 *
 * @copyright Copyright (c) 2023
 *
 * Updated: 2026-03 - Use new I2C driver API for ESP-IDF 5.4+
 */
#pragma once
#include <cstdint>
#include <driver/i2c_master.h>
#include <driver/gpio.h>
#include <esp_log.h>
#include <ctime>


/**
 * @brief Modify from "hal_tp.hpp"
 *
 */
namespace PCF8563 {


    static const char* TAG = "PCF8563";


    struct Config_t {
        int pin_scl     = -1;
        int pin_sda     = -1;
        int pin_int     = -1;

        uint8_t dev_addr = 0x51;
    };


    class PCF8563 {
        private:
            Config_t _cfg;
            uint8_t _data_buffer[8];
            i2c_master_dev_handle_t _i2c_dev_handle = nullptr;
            i2c_master_bus_handle_t _i2c_bus_handle = nullptr;  // Shared bus handle
            bool _initialized = false;

            inline esp_err_t _write_reg(uint8_t reg, uint8_t data)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;
                uint8_t write_buf[2] = {reg, data};
                return i2c_master_transmit(_i2c_dev_handle, write_buf, 2, portMAX_DELAY);
            }

            inline esp_err_t _read_reg(uint8_t reg, uint8_t readSize)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;
                return i2c_master_transmit_receive(_i2c_dev_handle, &reg, 1, _data_buffer, readSize, portMAX_DELAY);
            }

            inline uint8_t bcd2dec(uint8_t val)
            {
                return (val >> 4) * 10 + (val & 0x0f);
            }

            inline uint8_t dec2bcd(uint8_t val)
            {
                return ((val / 10) << 4) + (val % 10);
            }


        public:

            /* Config */
            inline Config_t config() { return _cfg; }
            inline void config(const Config_t& cfg) { _cfg = cfg; }
            inline void setPin(const int& sda, const int& scl, const int& intr)
            {
                _cfg.pin_sda = sda;
                _cfg.pin_scl = scl;
                _cfg.pin_int = intr;
            }


            inline bool init(const int& sda, const int& scl, const int& intr = -1)
            {
                setPin(sda, scl, intr);
                return init();
            }

            /**
             * @brief Initialize using a shared I2C bus handle
             * @param bus_handle Existing I2C master bus handle (from touchpad)
             */
            inline bool init(i2c_master_bus_handle_t bus_handle)
            {
                if (bus_handle == nullptr) {
                    ESP_LOGE(TAG, "Invalid bus handle");
                    return false;
                }

                _i2c_bus_handle = bus_handle;

                /* Setup interrupt pin if configured */
                if (_cfg.pin_int > 0) {
                    gpio_reset_pin((gpio_num_t)_cfg.pin_int);
                    gpio_set_direction((gpio_num_t)_cfg.pin_int, GPIO_MODE_INPUT);
                }

                /* Add RTC device to existing bus */
                i2c_device_config_t dev_config = {};
                dev_config.dev_addr_length = I2C_ADDR_BIT_LEN_7;
                dev_config.device_address = _cfg.dev_addr;
                dev_config.scl_speed_hz = 100000;

                esp_err_t ret = i2c_master_bus_add_device(_i2c_bus_handle, &dev_config, &_i2c_dev_handle);
                if (ret != ESP_OK) {
                    ESP_LOGE(TAG, "Failed to add RTC device to I2C bus: %s", esp_err_to_name(ret));
                    return false;
                }

                _initialized = true;

                // Initialize RTC registers
                _write_reg(0x00, 0x00);
                _write_reg(0x00, 0x00);
                _write_reg(0x0E, 0x03);

                ESP_LOGI(TAG, "PCF8563 RTC initialized");
                return true;
            }

            inline bool init()
            {
                gpioInit();

                // Note: This standalone init is deprecated - use init(bus_handle) instead
                ESP_LOGW(TAG, "Standalone RTC init not supported with new I2C driver - use shared bus");
                return false;
            }


            /* Setup gpio and reset */
            inline void gpioInit()
            {
                ESP_LOGD(TAG, "setup gpio");

                if (_cfg.pin_int > 0) {
                    gpio_reset_pin((gpio_num_t)_cfg.pin_int);
                    gpio_set_direction((gpio_num_t)_cfg.pin_int, GPIO_MODE_INPUT);
                }
            }


            inline esp_err_t getTime(tm& time)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;

                /* Time and date registers */
                esp_err_t res = _read_reg(0x02, 7);
                if (res != ESP_OK) {
                    return res;
                }

                ESP_LOGD(TAG, "data=%02x %02x %02x %02x %02x %02x %02x\n",
                    _data_buffer[0],_data_buffer[1],_data_buffer[2],_data_buffer[3],_data_buffer[4],_data_buffer[5],_data_buffer[6]);

                /* convert to unix time structure */
                time.tm_sec = bcd2dec(_data_buffer[0] & 0x7F);
                time.tm_min = bcd2dec(_data_buffer[1] & 0x7F);
                time.tm_hour = bcd2dec(_data_buffer[2] & 0x3F);
                time.tm_mday = bcd2dec(_data_buffer[3] & 0x3F);
                time.tm_wday = bcd2dec(_data_buffer[4] & 0x07);		    // tm_wday is 0 to 6
                time.tm_mon  = bcd2dec(_data_buffer[5] & 0x1F) - 1;	    // tm_mon is 0 to 11
                time.tm_year = bcd2dec(_data_buffer[6]) + 2000;
                time.tm_isdst = 0;

                ESP_LOGD(TAG, "%02d:%02d:%02d %d-%d-%d-%d\n",
                    time.tm_hour, time.tm_min, time.tm_sec, time.tm_year, time.tm_mon, time.tm_mday, time.tm_wday);

                return ESP_OK;
            }


            inline esp_err_t setTime(const tm& time)
            {
                if (!_initialized) return ESP_ERR_INVALID_STATE;

                /* Time and date registers */
                _data_buffer[0] = 0x02;

                _data_buffer[1] = dec2bcd(time.tm_sec);
                _data_buffer[2] = dec2bcd(time.tm_min);
                _data_buffer[3] = dec2bcd(time.tm_hour);
                _data_buffer[4] = dec2bcd(time.tm_mday);
                _data_buffer[5] = dec2bcd(time.tm_wday);		        // tm_wday is 0 to 6
                _data_buffer[6] = dec2bcd(time.tm_mon + 1);	            // tm_mon is 0 to 11
                _data_buffer[7] = dec2bcd(time.tm_year - 2000);

                return i2c_master_transmit(_i2c_dev_handle, _data_buffer, 8, portMAX_DELAY);
            }


            /// Set timer IRQ
            /// @param afterSeconds 1 - 15,300. If 256 or more, 1-minute cycle.  (max 255 minute.)
            /// @return the set number of seconds.
            inline int setAlarmIRQ(int afterSeconds)
            {
                if (!_initialized) return -1;

                _read_reg(0x01, 1);
                std::uint8_t reg_value = _data_buffer[0] & ~0x0C;


                if (afterSeconds < 0)
                { // disable timer
                    _write_reg(0x01, reg_value & ~0x01);
                    _write_reg(0x0E, 0x03);

                    return -1;
                }

                std::size_t div = 1;
                std::uint8_t type_value = 0x82;
                if (afterSeconds < 270)
                {
                    if (afterSeconds > 255)
                    {
                        afterSeconds = 255;
                    }
                }
                else
                {
                    div = 60;
                    afterSeconds = (afterSeconds + 30) / div;
                    if (afterSeconds > 255)
                    {
                        afterSeconds = 255;
                    }
                    type_value = 0x83;
                }

                _write_reg(0x0E, type_value);
                _write_reg(0x0F, afterSeconds);
                _write_reg(0x01, (reg_value | 0x01) & ~0x80);

                return afterSeconds * div;
            }


            inline void clearIRQ(void)
            {
                if (!_initialized) return;

                _read_reg(0x01, 1);
                _data_buffer[0] = _data_buffer[0] & 0B11110011;
                _write_reg(0x01, _data_buffer[0]);
            }


            inline void disableIRQ(void)
            {
                if (!_initialized) return;

                _data_buffer[0] = 0x09;

                _data_buffer[1] = 0x80;
                _data_buffer[2] = 0x80;
                _data_buffer[3] = 0x80;
                _data_buffer[4] = 0x80;

                i2c_master_transmit(_i2c_dev_handle, _data_buffer, 5, portMAX_DELAY);

                _write_reg(0x0E, 0);
                _write_reg(0x01, 0x00);
            }


    };


}
