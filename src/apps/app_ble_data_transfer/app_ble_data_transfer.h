/**
 * @file app_ble_data_transfer.h
 * @brief BLE Data Transfer App - sends inspection/yard data to phone
 * @version 1.0
 * @date 2026-03-19
 */
#pragma once
#include "../app.h"
#include "../../hal/hal.h"
#include "gui/gui_ble_data_transfer.h"

namespace MOONCAKE
{
    namespace USER_APP
    {
        namespace BLE_DATA_TRANSFER
        {
            struct Data_t
            {
                HAL::HAL* hal = nullptr;

                // Update intervals
                uint32_t gui_update_interval = 500;
                uint32_t gui_update_time_count = 0;

                // Data counts for display
                uint16_t inspection_count = 0;
                uint16_t yard_count = 0;
                uint16_t hive_count = 0;
            };
        }

        class BleDataTransfer : public APP_BASE
        {
            private:
                const char* _tag = "BLE_Data";
                BLE_DATA_TRANSFER::Data_t _data;
                GUI_BLE_DataTransfer _gui;

                void _ble_init();
                void _update_counts();
                void _process_data_requests();

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
