/**
 * @file gui_ble_data_transfer.h
 * @brief GUI for BLE Data Transfer app
 * @version 1.0
 * @date 2026-03-19
 */
#pragma once
#include "../../utilities/gui_base/gui_base.h"

class GUI_BLE_DataTransfer : public GUI_Base
{
    public:
        enum ConnectionStatus_t
        {
            Status_Advertising = 0,
            Status_Connected,
            Status_Transferring,
        };

        void init() override;

        void renderPage(
            ConnectionStatus_t status,
            const char* deviceName,
            uint16_t inspectionCount,
            uint16_t yardCount,
            uint16_t hiveCount,
            uint8_t transferProgress = 0
        );
};
