/**
 * @file app_settings.h
 * @brief Settings App - Timezone and Clear Data options
 * @version 1.0
 * @date 2026-03-20
 */
#pragma once
#include "../app.h"
#include "../../hal/hal.h"
#include "gui/gui_settings.h"

namespace MOONCAKE
{
    namespace USER_APP
    {
        namespace SETTINGS_APP
        {
            // Settings menu options
            enum class MenuOption {
                TIMEZONE = 0,
                CLEAR_DATA,
                DONE,
                COUNT
            };

            struct Data_t
            {
                HAL::HAL* hal = nullptr;

                // Current menu selection
                int selectedOption = 0;

                // Sub-menu states
                bool inTimezoneEdit = false;
                bool inClearConfirm = false;

                // Timezone offset from UTC (in hours, can be negative)
                int8_t timezoneOffset = -4;  // Default GMT-4

                // Clear data confirmation
                bool clearConfirmSelected = false;  // false = No, true = Yes
                bool clearing = false;
                bool cleared = false;
            };

            // Timezone range
            constexpr int8_t TIMEZONE_MIN = -12;
            constexpr int8_t TIMEZONE_MAX = 14;
        }

        class Settings : public APP_BASE
        {
            private:
                const char* _tag = "Settings";
                SETTINGS_APP::Data_t _data;
                GUI_Settings _gui;

                void _handle_input();
                void _handle_main_menu();
                void _handle_timezone_edit();
                void _handle_clear_confirm();
                void _render();

                // Storage
                void _load_settings();
                void _save_timezone();
                bool _clear_all_data();

            public:
                /**
                 * @brief Get gui pointer for basic settings
                 * @return GUI_Base*
                 */
                GUI_Base* getGui() override { return &_gui; }

                /**
                 * @brief Lifecycle callbacks
                 */
                void onSetup();
                void onCreate();
                void onRunning();
                void onDestroy();
        };

    }
}
