/**
 * @file app_ble_data_transfer.cpp
 * @brief BLE Data Transfer App implementation
 * @version 1.0
 * @date 2026-03-19
 */
#include "app_ble_data_transfer.h"
#include "../common_define.h"
#include "../app_beehive_inspection/inspection_storage.h"
#include "../app_beehive_inspection/inspection_data.h"
#include "../app_yard_management/yard_storage.h"
#include "../app_yard_management/yard_data.h"
#include "ble_data_service.h"
#include <cstdio>
#include <cstring>

using namespace MOONCAKE::USER_APP;

// External C functions from ble_data_service.c
extern "C" {
    void ble_data_service_send_data(const uint8_t* data, uint16_t len);
    void ble_data_service_transfer_complete(void);
}

void BleDataTransfer::_ble_init()
{
    printf("[BLE_App] Initializing BLE Data Transfer...\n");
    printf("[BLE_App] Free heap before BLE init: %lu bytes\n",
           (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));

    // Start the BLE service
    ble_data_service_start();

    printf("[BLE_App] Free heap after BLE init: %lu bytes\n",
           (unsigned long)heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
}

void BleDataTransfer::_update_counts()
{
    // Get counts from storage
    _data.inspection_count = BEEHIVE_INSPECTION::getInspectionCount();
    _data.yard_count = YARD_MANAGEMENT::getYardCount();
    _data.hive_count = YARD_MANAGEMENT::getHiveCount();

    printf("[BLE_App] Data counts - Inspections: %d, Yards: %d, Hives: %d\n",
           _data.inspection_count, _data.yard_count, _data.hive_count);
}

void BleDataTransfer::_process_data_requests()
{
    BleDataServiceInfo_t* info = ble_data_service_get_info();

    // Only process if we're connected, subscribed, and have a command pending
    if (!info->is_connected || !info->is_subscribed) {
        return;
    }

    uint8_t cmd = info->last_command;
    if (cmd == 0) {
        return;
    }

    // Clear the command so we don't process it again
    info->last_command = 0;

    uint8_t response[256];
    uint16_t len = 0;

    switch (cmd) {
        case CMD_GET_INSPECTION_COUNT:
        {
            size_t count = BEEHIVE_INSPECTION::getInspectionCount();
            printf("[BLE_App] Sending inspection count: %d\n", (int)count);

            response[0] = RESP_OK;
            response[1] = CMD_GET_INSPECTION_COUNT;
            response[2] = (count >> 0) & 0xFF;
            response[3] = (count >> 8) & 0xFF;
            len = 4;
            ble_data_service_send_data(response, len);
            break;
        }

        case CMD_GET_YARD_COUNT:
        {
            size_t count = YARD_MANAGEMENT::getYardCount();
            printf("[BLE_App] Sending yard count: %d\n", (int)count);

            response[0] = RESP_OK;
            response[1] = CMD_GET_YARD_COUNT;
            response[2] = (count >> 0) & 0xFF;
            response[3] = (count >> 8) & 0xFF;
            len = 4;
            ble_data_service_send_data(response, len);
            break;
        }

        case CMD_GET_HIVE_COUNT:
        {
            size_t count = YARD_MANAGEMENT::getHiveCount();
            printf("[BLE_App] Sending hive count: %d\n", (int)count);

            response[0] = RESP_OK;
            response[1] = CMD_GET_HIVE_COUNT;
            response[2] = (count >> 0) & 0xFF;
            response[3] = (count >> 8) & 0xFF;
            len = 4;
            ble_data_service_send_data(response, len);
            break;
        }

        case CMD_GET_INSPECTION_IDS:
        {
            std::vector<uint32_t> ids = BEEHIVE_INSPECTION::getAllInspectionIDs();
            printf("[BLE_App] Sending %d inspection IDs\n", (int)ids.size());

            // Send count first
            response[0] = RESP_DATA_START;
            response[1] = CMD_GET_INSPECTION_IDS;
            response[2] = (ids.size() >> 0) & 0xFF;
            response[3] = (ids.size() >> 8) & 0xFF;
            len = 4;

            // Add IDs (up to ~60 per packet to stay under MTU)
            for (size_t i = 0; i < ids.size() && len < 240; i++) {
                response[len++] = (ids[i] >> 0) & 0xFF;
                response[len++] = (ids[i] >> 8) & 0xFF;
                response[len++] = (ids[i] >> 16) & 0xFF;
                response[len++] = (ids[i] >> 24) & 0xFF;
            }

            ble_data_service_send_data(response, len);
            ble_data_service_transfer_complete();
            break;
        }

        case CMD_GET_YARD_LIST:
        {
            std::vector<uint32_t> yards = YARD_MANAGEMENT::getAllYardNumbers();
            printf("[BLE_App] Sending %d yard numbers\n", (int)yards.size());

            response[0] = RESP_DATA_START;
            response[1] = CMD_GET_YARD_LIST;
            response[2] = (yards.size() >> 0) & 0xFF;
            response[3] = (yards.size() >> 8) & 0xFF;
            len = 4;

            for (size_t i = 0; i < yards.size() && len < 240; i++) {
                response[len++] = (yards[i] >> 0) & 0xFF;
                response[len++] = (yards[i] >> 8) & 0xFF;
                response[len++] = (yards[i] >> 16) & 0xFF;
                response[len++] = (yards[i] >> 24) & 0xFF;
            }

            ble_data_service_send_data(response, len);
            ble_data_service_transfer_complete();
            break;
        }

        case CMD_GET_ALL_DATA:
        {
            printf("[BLE_App] Starting full data transfer...\n");

            // Get all data
            std::vector<uint32_t> yardNums = YARD_MANAGEMENT::getAllYardNumbers();
            std::vector<uint32_t> hiveNums = YARD_MANAGEMENT::getAllHiveNumbers();
            std::vector<uint32_t> inspectionIds = BEEHIVE_INSPECTION::getAllInspectionIDs();

            // Calculate total for progress tracking
            info->total_bytes = (yardNums.size() * sizeof(YARD_MANAGEMENT::YardRecord)) +
                               (hiveNums.size() * sizeof(YARD_MANAGEMENT::HiveRecord)) +
                               (inspectionIds.size() * sizeof(BEEHIVE_INSPECTION::InspectionRecord));
            info->bytes_sent = 0;

            printf("[BLE_App] Total data to send: ~%lu bytes\n", (unsigned long)info->total_bytes);
            printf("[BLE_App] Yards: %d, Hives: %d, Inspections: %d\n",
                   (int)yardNums.size(), (int)hiveNums.size(), (int)inspectionIds.size());

            // Send summary header
            response[0] = RESP_DATA_START;
            response[1] = CMD_GET_ALL_DATA;
            response[2] = yardNums.size() & 0xFF;
            response[3] = (yardNums.size() >> 8) & 0xFF;
            response[4] = hiveNums.size() & 0xFF;
            response[5] = (hiveNums.size() >> 8) & 0xFF;
            response[6] = inspectionIds.size() & 0xFF;
            response[7] = (inspectionIds.size() >> 8) & 0xFF;
            ble_data_service_send_data(response, 8);

            // Send yards as JSON-ish format for easy phone parsing
            for (uint32_t yardNum : yardNums) {
                YARD_MANAGEMENT::YardRecord yard;
                if (YARD_MANAGEMENT::loadYard(yardNum, yard)) {
                    char jsonBuf[200];
                    int jsonLen = snprintf(jsonBuf, sizeof(jsonBuf),
                        "{\"type\":\"yard\",\"num\":%lu,\"name\":\"%s\",\"rfid\":\"%s\",\"status\":\"%c\",\"hives\":%d}",
                        (unsigned long)yard.yardNumber,
                        yard.nickname,
                        yard.rfidTagId,
                        yard.status,
                        yard.activeHiveCount);

                    printf("[BLE_App] Sending yard: %s\n", jsonBuf);

                    response[0] = RESP_DATA_CONT;
                    response[1] = 'Y';  // Yard marker
                    memcpy(&response[2], jsonBuf, jsonLen);
                    ble_data_service_send_data(response, 2 + jsonLen);

                    vTaskDelay(pdMS_TO_TICKS(20));  // Small delay between packets
                }
            }

            // Send hives
            for (uint32_t hiveNum : hiveNums) {
                YARD_MANAGEMENT::HiveRecord hive;
                if (YARD_MANAGEMENT::loadHive(hiveNum, hive)) {
                    char jsonBuf[200];
                    int jsonLen = snprintf(jsonBuf, sizeof(jsonBuf),
                        "{\"type\":\"hive\",\"num\":%lu,\"yard\":%lu,\"rfid\":\"%s\",\"status\":\"%c\",\"origin\":%d}",
                        (unsigned long)hive.hiveNumber,
                        (unsigned long)hive.yardNumber,
                        hive.rfidTagId,
                        hive.status,
                        (int)hive.originType);

                    printf("[BLE_App] Sending hive: %s\n", jsonBuf);

                    response[0] = RESP_DATA_CONT;
                    response[1] = 'H';  // Hive marker
                    memcpy(&response[2], jsonBuf, jsonLen);
                    ble_data_service_send_data(response, 2 + jsonLen);

                    vTaskDelay(pdMS_TO_TICKS(20));
                }
            }

            // Send inspections
            for (uint32_t id : inspectionIds) {
                BEEHIVE_INSPECTION::InspectionRecord insp;
                if (BEEHIVE_INSPECTION::loadInspectionFromNVS(id, insp)) {
                    char jsonBuf[256];
                    int jsonLen = snprintf(jsonBuf, sizeof(jsonBuf),
                        "{\"type\":\"inspection\",\"id\":%lu,\"hive\":%lu,\"ts\":%lu,"
                        "\"qr\":%d,\"sc\":%d,\"ssc\":%d,\"supers\":%d,\"temp\":%d,\"brood\":%d,\"treat\":%d,\"pests\":%d}",
                        (unsigned long)insp.recordId,
                        (unsigned long)insp.hiveId,
                        (unsigned long)insp.timestamp,
                        insp.queenRight ? 1 : 0,
                        (int)insp.swarmCells,
                        (int)insp.supersedureCells,
                        insp.superCount,
                        (int)insp.temperament,
                        insp.broodFrames,
                        (int)insp.treatment,
                        insp.pests);

                    printf("[BLE_App] Sending inspection: %s\n", jsonBuf);

                    response[0] = RESP_DATA_CONT;
                    response[1] = 'I';  // Inspection marker
                    memcpy(&response[2], jsonBuf, jsonLen);
                    ble_data_service_send_data(response, 2 + jsonLen);

                    vTaskDelay(pdMS_TO_TICKS(20));
                }
            }

            // Send end marker
            response[0] = RESP_DATA_END;
            response[1] = CMD_GET_ALL_DATA;
            ble_data_service_send_data(response, 2);

            ble_data_service_transfer_complete();
            printf("[BLE_App] Full data transfer complete!\n");
            break;
        }

        default:
            // Other commands handled by C layer
            break;
    }
}

void BleDataTransfer::onSetup()
{
    setAppName("BLE_Data");
    setAllowBgRunning(false);

    BLE_DATA_TRANSFER::Data_t default_data;
    _data = default_data;

    _data.hal = (HAL::HAL*)getUserData();
}

void BleDataTransfer::onCreate()
{
    _log("onCreate");
    printf("[BLE_App] =========================================\n");
    printf("[BLE_App] BLE Data Transfer App Started\n");
    printf("[BLE_App] =========================================\n");

    _ble_init();
    _update_counts();

    // Initial GUI render
    _gui.renderPage(
        GUI_BLE_DataTransfer::Status_Advertising,
        ble_data_service_get_info()->device_name,
        _data.inspection_count,
        _data.yard_count,
        _data.hive_count
    );
}

void BleDataTransfer::onRunning()
{
    // Process any pending data requests
    _process_data_requests();

    // Update GUI periodically
    if ((millis() - _data.gui_update_time_count) > _data.gui_update_interval)
    {
        BleDataServiceInfo_t* info = ble_data_service_get_info();

        GUI_BLE_DataTransfer::ConnectionStatus_t status;
        if (info->transfer_in_progress) {
            status = GUI_BLE_DataTransfer::Status_Transferring;
        } else if (info->is_connected) {
            status = GUI_BLE_DataTransfer::Status_Connected;
        } else {
            status = GUI_BLE_DataTransfer::Status_Advertising;
        }

        _gui.renderPage(
            status,
            info->device_name,
            _data.inspection_count,
            _data.yard_count,
            _data.hive_count,
            ble_data_service_get_progress()
        );

        _data.gui_update_time_count = millis();
    }

    // Check for button press to exit
    if (!_data.hal->encoder.btn.read())
    {
        printf("[BLE_App] Button pressed - exiting\n");

        // Wait for release
        while (!_data.hal->encoder.btn.read())
            delay(5);

        destroyApp();
    }
}

void BleDataTransfer::onDestroy()
{
    _log("onDestroy");
    printf("[BLE_App] Shutting down BLE service...\n");

    ble_data_service_stop();

    printf("[BLE_App] BLE Data Transfer App Stopped\n");
    printf("[BLE_App] =========================================\n");
}
