/**
 * @file app_settings.cpp
 * @brief Settings App implementation
 * @version 1.0
 * @date 2026-03-20
 */
#include "app_settings.h"
#include "settings_storage.h"
#include "../common_define.h"
#include "../app_yard_management/yard_storage.h"
#include "../app_beehive_inspection/inspection_storage.h"
#include "../../hal/hal_common_define.h"
#include <cstdio>
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/rtc_io.h"


using namespace MOONCAKE::USER_APP;

void Settings::_load_settings()
{
    _data.timezoneOffset = SETTINGS::getTimezoneOffset();
    printf("[Settings] Loaded timezone: GMT%+d\n", _data.timezoneOffset);
}

void Settings::_save_timezone()
{
    SETTINGS::setTimezoneOffset(_data.timezoneOffset);
}

bool Settings::_clear_all_data()
{
    printf("[Settings] Clearing all data...\n");

    bool success = true;

    // Clear yard management data (yards and hives)
    if (!YARD_MANAGEMENT::clearAllYardManagementData()) {
        printf("[Settings] WARNING: Failed to clear yard management data\n");
        success = false;
    }

    // Clear inspection data
    if (!BEEHIVE_INSPECTION::clearAllInspections()) {
        printf("[Settings] WARNING: Failed to clear inspection data\n");
        success = false;
    }

    if (success) {
        printf("[Settings] All data cleared successfully\n");
    }

    return success;
}

void Settings::_handle_main_menu()
{
    // Handle rotation
    if (_data.hal->encoder.wasMoved(true)) {
        int direction = (_data.hal->encoder.getDirection() < 1) ? 1 : -1;
        _data.selectedOption += direction;

        // Wrap around
        int optionCount = static_cast<int>(SETTINGS_APP::MenuOption::COUNT);
        if (_data.selectedOption < 0) {
            _data.selectedOption = optionCount - 1;
        } else if (_data.selectedOption >= optionCount) {
            _data.selectedOption = 0;
        }

        _data.hal->buzz.tone(4000, 20);
        _render();
    }

    // Handle button press
    if (!_data.hal->encoder.btn.read()) {
        // Wait for release
        while (!_data.hal->encoder.btn.read()) {
            delay(5);
        }

        _data.hal->buzz.tone(2000, 50);

        switch (static_cast<SETTINGS_APP::MenuOption>(_data.selectedOption)) {
            case SETTINGS_APP::MenuOption::TIMEZONE:
                _data.inTimezoneEdit = true;
                break;
            case SETTINGS_APP::MenuOption::CLEAR_DATA:
                _data.inClearConfirm = true;
                _data.clearConfirmSelected = false;  // Default to No
                _data.clearing = false;
                _data.cleared = false;
                _data.clearFailed = false;
                break;
            case SETTINGS_APP::MenuOption::SLEEP:
                _data.inSleepConfirm = true;
                _data.sleepConfirmSelected = false;  // Default to No
                break;
            case SETTINGS_APP::MenuOption::BEEP:
                _data.inBeepTest = true;
                break;
            case SETTINGS_APP::MenuOption::DONE:
                destroyApp();
                return;
            default:
                break;
        }

        _render();
    }
}

void Settings::_handle_timezone_edit()
{
    // Handle rotation
    if (_data.hal->encoder.wasMoved(true)) {
        int direction = (_data.hal->encoder.getDirection() < 1) ? 1 : -1;
        _data.timezoneOffset += direction;

        // Clamp to valid range
        if (_data.timezoneOffset < SETTINGS_APP::TIMEZONE_MIN) {
            _data.timezoneOffset = SETTINGS_APP::TIMEZONE_MIN;
        } else if (_data.timezoneOffset > SETTINGS_APP::TIMEZONE_MAX) {
            _data.timezoneOffset = SETTINGS_APP::TIMEZONE_MAX;
        }

        _data.hal->buzz.tone(4000, 20);
        _render();
    }

    // Handle button press - save and go back
    if (!_data.hal->encoder.btn.read()) {
        // Wait for release
        while (!_data.hal->encoder.btn.read()) {
            delay(5);
        }

        _save_timezone();
        _data.hal->buzz.tone(2000, 100);
        _data.inTimezoneEdit = false;
        _render();
    }
}

void Settings::_handle_clear_confirm()
{
    if (_data.cleared || _data.clearFailed) {
        // Handle button press to go back
        if (!_data.hal->encoder.btn.read()) {
            while (!_data.hal->encoder.btn.read()) {
                delay(5);
            }

            _data.hal->buzz.tone(2000, 50);
            _data.inClearConfirm = false;
            _data.cleared = false;
            _data.clearFailed = false;
            _render();
        }
        return;
    }

    if (_data.clearing) {
        return;  // Do nothing while clearing
    }

    // Handle rotation to toggle Yes/No
    if (_data.hal->encoder.wasMoved(true)) {
        _data.clearConfirmSelected = !_data.clearConfirmSelected;
        _data.hal->buzz.tone(4000, 20);
        _render();
    }

    // Handle button press
    if (!_data.hal->encoder.btn.read()) {
        // Wait for release
        while (!_data.hal->encoder.btn.read()) {
            delay(5);
        }

        if (_data.clearConfirmSelected) {
            // User selected Yes - clear data
            _data.clearing = true;
            _render();

            if (_clear_all_data()) {
                _data.cleared = true;
                _data.hal->buzz.tone(2000, 100);
            } else {
                _data.clearFailed = true;
                _data.hal->buzz.tone(500, 200);
            }

            _data.clearing = false;
            _render();
        } else {
            // User selected No - go back
            _data.hal->buzz.tone(2000, 50);
            _data.inClearConfirm = false;
            _render();
        }
    }
}

void Settings::_handle_sleep_confirm()
{
    // Handle rotation to toggle Yes/No
    if (_data.hal->encoder.wasMoved(true)) {
        _data.sleepConfirmSelected = !_data.sleepConfirmSelected;
        _data.hal->buzz.tone(4000, 20);
        _render();
    }

    // Handle button press
    if (!_data.hal->encoder.btn.read()) {
        // Wait for release
        while (!_data.hal->encoder.btn.read()) {
            delay(5);
        }

        if (_data.sleepConfirmSelected) {
            // User selected Yes - enter sleep
            _data.hal->buzz.tone(1000, 100);
            delay(150);
            _enter_sleep();
            // After wake-up, reset state
            _data.inSleepConfirm = false;
            _render();
        } else {
            // User selected No - go back
            _data.hal->buzz.tone(2000, 50);
            _data.inSleepConfirm = false;
            _render();
        }
    }
}

void Settings::_enter_sleep()
{
    printf("[Settings] Entering light sleep mode...\n");
    printf("[Settings] Wake sources: Button (GPIO %d), Encoder A (GPIO %d), Encoder B (GPIO %d)\n",
           HAL_PIN_PWR_WAKE_UP, HAL_PIN_ENCODER_A, HAL_PIN_ENCODER_B);

    // Turn off display backlight to save power
    // The display will be reinitialized on wake

    // Configure GPIO wake-up sources
    // Button - GPIO 42 - wake on low level (button pressed)
    gpio_wakeup_enable((gpio_num_t)HAL_PIN_PWR_WAKE_UP, GPIO_INTR_LOW_LEVEL);

    // Encoder pins - wake on any edge (rotation)
    gpio_wakeup_enable((gpio_num_t)HAL_PIN_ENCODER_A, GPIO_INTR_LOW_LEVEL);
    gpio_wakeup_enable((gpio_num_t)HAL_PIN_ENCODER_B, GPIO_INTR_LOW_LEVEL);

    // Enable GPIO wakeup
    esp_sleep_enable_gpio_wakeup();

    // Optional: Add a timer wakeup as backup (30 seconds)
    // esp_sleep_enable_timer_wakeup(30 * 1000000ULL);

    printf("[Settings] Going to sleep now...\n");

    // Enter light sleep
    esp_light_sleep_start();

    // --- Code resumes here after wake-up ---
    printf("[Settings] Woke up from sleep!\n");

    // Get wake-up cause
    esp_sleep_wakeup_cause_t wakeup_cause = esp_sleep_get_wakeup_cause();
    switch (wakeup_cause) {
        case ESP_SLEEP_WAKEUP_GPIO:
            printf("[Settings] Wake-up cause: GPIO\n");
            break;
        case ESP_SLEEP_WAKEUP_TIMER:
            printf("[Settings] Wake-up cause: Timer\n");
            break;
        default:
            printf("[Settings] Wake-up cause: %d\n", wakeup_cause);
            break;
    }

    // Small delay after wake-up
    delay(100);

    // Re-render the display
    _data.hal->buzz.tone(2000, 50);
}

void Settings::_handle_beep_test()
{
    // Play beep when button is pressed
    if (!_data.hal->encoder.btn.read()) {
        // Wait for release
        while (!_data.hal->encoder.btn.read()) {
            delay(5);
        }

        // Play a test beep sequence
        _data.hal->buzz.tone(1000, 100);
        delay(1000);
        _data.hal->buzz.tone(2000, 100);
        delay(1000);
        _data.hal->buzz.tone(3000, 100);

        // Go back to main menu
        _data.inBeepTest = false;
        _render();
    }
}

void Settings::_handle_input()
{
    if (_data.inTimezoneEdit) {
        _handle_timezone_edit();
    } else if (_data.inClearConfirm) {
        _handle_clear_confirm();
    } else if (_data.inSleepConfirm) {
        _handle_sleep_confirm();
    } else if (_data.inBeepTest) {
        _handle_beep_test();
    } else {
        _handle_main_menu();
    }
}

void Settings::_render()
{
    if (_data.inTimezoneEdit) {
        _gui.renderTimezoneEdit(_data.timezoneOffset);
    } else if (_data.inClearConfirm) {
        _gui.renderClearConfirm(_data.clearConfirmSelected, _data.clearing, _data.cleared, _data.clearFailed);
    } else if (_data.inSleepConfirm) {
        _gui.renderSleepConfirm(_data.sleepConfirmSelected);
    } else if (_data.inBeepTest) {
        _gui.renderBeepTest();
    } else {
        _gui.renderMainMenu(_data.selectedOption);
    }
}

void Settings::onSetup()
{
    setAppName("Settings");
    setAllowBgRunning(false);

    SETTINGS_APP::Data_t default_data;
    _data = default_data;

    _data.hal = (HAL::HAL*)getUserData();
}

void Settings::onCreate()
{
    _log("onCreate");
    printf("[Settings] Settings App Started\n");

    _load_settings();
    _render();
}

void Settings::onRunning()
{
    _handle_input();

    // Long press to exit
    static uint32_t buttonPressStart = 0;
    static bool buttonWasPressed = false;

    if (!_data.hal->encoder.btn.read()) {
        if (!buttonWasPressed) {
            buttonPressStart = millis();
            buttonWasPressed = true;
        } else if (millis() - buttonPressStart > 1000) {
            // Long press - exit app
            printf("[Settings] Long press - exiting\n");

            // Wait for release
            while (!_data.hal->encoder.btn.read()) {
                delay(5);
            }

            destroyApp();
        }
    } else {
        buttonWasPressed = false;
    }
}

void Settings::onDestroy()
{
    _log("onDestroy");
    printf("[Settings] Settings App Stopped\n");
}
