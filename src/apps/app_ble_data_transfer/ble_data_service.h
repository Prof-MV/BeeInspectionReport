/**
 * @file ble_data_service.h
 * @brief BLE GATT service for transferring inspection and yard data
 * @version 1.0
 * @date 2026-03-19
 */
#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>

// Custom Service UUIDs (128-bit)
// Bee Inspection Service: 12345678-1234-5678-1234-56789ABCDEF0
#define BLE_BEE_SERVICE_UUID_128 \
    0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12, \
    0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12

// Characteristic UUIDs (using 16-bit for simplicity within the custom service)
// Command characteristic - write to request data
#define BLE_CHAR_COMMAND_UUID           0xFF01
// Response characteristic - read/notify for data responses
#define BLE_CHAR_RESPONSE_UUID          0xFF02
// Status characteristic - read connection/transfer status
#define BLE_CHAR_STATUS_UUID            0xFF03

// Command types (written to command characteristic)
typedef enum {
    CMD_GET_DEVICE_INFO     = 0x01,   // Get device name and version
    CMD_GET_INSPECTION_COUNT = 0x02,  // Get count of inspection records
    CMD_GET_INSPECTION_IDS  = 0x03,   // Get list of all inspection IDs
    CMD_GET_INSPECTION      = 0x04,   // Get specific inspection by ID (followed by 4-byte ID)
    CMD_GET_YARD_COUNT      = 0x05,   // Get count of yards
    CMD_GET_YARD_LIST       = 0x06,   // Get list of yard numbers
    CMD_GET_YARD            = 0x07,   // Get specific yard by number (followed by 4-byte number)
    CMD_GET_HIVE_COUNT      = 0x08,   // Get count of hives
    CMD_GET_HIVES_IN_YARD   = 0x09,   // Get hives in yard (followed by 4-byte yard number)
    CMD_GET_HIVE            = 0x0A,   // Get specific hive by number (followed by 4-byte number)
    CMD_GET_ALL_DATA        = 0x0B,   // Stream all data (yards, hives, inspections)
    CMD_PING                = 0xFF,   // Simple ping to test connection
} BleCommand_t;

// Response status codes
typedef enum {
    RESP_OK                 = 0x00,
    RESP_ERROR              = 0x01,
    RESP_NOT_FOUND          = 0x02,
    RESP_BUSY               = 0x03,
    RESP_DATA_START         = 0x10,   // Start of multi-packet data
    RESP_DATA_CONT          = 0x11,   // Continuation of data
    RESP_DATA_END           = 0x12,   // End of data transfer
} BleResponseStatus_t;

// Connection state info
typedef struct {
    char device_name[32];
    uint8_t is_connected;
    uint8_t is_subscribed;
    uint16_t conn_handle;
    uint8_t last_command;
    uint8_t transfer_in_progress;
    uint32_t bytes_sent;
    uint32_t total_bytes;
} BleDataServiceInfo_t;

// Initialize the BLE data service
void ble_data_service_start(void);

// Stop the BLE data service
void ble_data_service_stop(void);

// Get service info (connection status, etc.)
BleDataServiceInfo_t* ble_data_service_get_info(void);

// Check if connected
bool ble_data_service_is_connected(void);

// Check if a transfer is in progress
bool ble_data_service_is_transferring(void);

// Get transfer progress (0-100)
uint8_t ble_data_service_get_progress(void);

#ifdef __cplusplus
}
#endif
