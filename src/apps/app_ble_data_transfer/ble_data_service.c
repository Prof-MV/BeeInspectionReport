/**
 * @file ble_data_service.c
 * @brief BLE GATT service implementation for data transfer
 * @version 1.0
 * @date 2026-03-19
 */

#include "ble_data_service.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/timers.h"
#include "nimble/nimble_port.h"
#include "nimble/nimble_port_freertos.h"
#include "host/ble_hs.h"
#include "host/util/util.h"
#include "host/ble_uuid.h"
#include "services/gap/ble_svc_gap.h"
#include "services/gatt/ble_svc_gatt.h"
#include <esp_mac.h>
#include <string.h>
#include <stdio.h>

static const char *TAG = "BLE_DATA_SVC";

// Service info
static BleDataServiceInfo_t _service_info;
static char* _device_name = NULL;
static uint8_t _addr_type;

// Characteristic handles
static uint16_t _cmd_handle;
static uint16_t _response_handle;
static uint16_t _status_handle;

// Response buffer
#define RESPONSE_BUF_SIZE 512
static uint8_t _response_buf[RESPONSE_BUF_SIZE];
static uint16_t _response_len = 0;

// Forward declarations
static int ble_gap_event(struct ble_gap_event *event, void *arg);
static int gatt_svr_chr_access_cmd(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg);
static int gatt_svr_chr_access_response(uint16_t conn_handle, uint16_t attr_handle,
                                        struct ble_gatt_access_ctxt *ctxt, void *arg);
static int gatt_svr_chr_access_status(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg);

// 128-bit service UUID
static const ble_uuid128_t gatt_svr_svc_uuid =
    BLE_UUID128_INIT(0xF0, 0xDE, 0xBC, 0x9A, 0x78, 0x56, 0x34, 0x12,
                     0x78, 0x56, 0x34, 0x12, 0x78, 0x56, 0x34, 0x12);

// GATT service definition
static const struct ble_gatt_svc_def gatt_svr_svcs[] = {
    {
        // Bee Data Service
        .type = BLE_GATT_SVC_TYPE_PRIMARY,
        .uuid = &gatt_svr_svc_uuid.u,
        .characteristics = (struct ble_gatt_chr_def[]) {
            {
                // Command characteristic (write)
                .uuid = BLE_UUID16_DECLARE(BLE_CHAR_COMMAND_UUID),
                .access_cb = gatt_svr_chr_access_cmd,
                .val_handle = &_cmd_handle,
                .flags = BLE_GATT_CHR_F_WRITE | BLE_GATT_CHR_F_WRITE_NO_RSP,
            },
            {
                // Response characteristic (read/notify)
                .uuid = BLE_UUID16_DECLARE(BLE_CHAR_RESPONSE_UUID),
                .access_cb = gatt_svr_chr_access_response,
                .val_handle = &_response_handle,
                .flags = BLE_GATT_CHR_F_READ | BLE_GATT_CHR_F_NOTIFY,
            },
            {
                // Status characteristic (read)
                .uuid = BLE_UUID16_DECLARE(BLE_CHAR_STATUS_UUID),
                .access_cb = gatt_svr_chr_access_status,
                .val_handle = &_status_handle,
                .flags = BLE_GATT_CHR_F_READ,
            },
            {
                0, // No more characteristics
            },
        }
    },
    {
        0, // No more services
    },
};

// Process received command
static void process_command(const uint8_t* data, uint16_t len)
{
    if (len < 1) {
        printf("[BLE] ERROR: Empty command received\n");
        return;
    }

    uint8_t cmd = data[0];
    _service_info.last_command = cmd;

    printf("[BLE] Command received: 0x%02X (len=%d)\n", cmd, len);

    // Prepare response based on command
    _response_len = 0;
    memset(_response_buf, 0, RESPONSE_BUF_SIZE);

    switch (cmd) {
        case CMD_PING:
            printf("[BLE] PING received, sending PONG\n");
            _response_buf[0] = RESP_OK;
            _response_buf[1] = 'P';
            _response_buf[2] = 'O';
            _response_buf[3] = 'N';
            _response_buf[4] = 'G';
            _response_len = 5;
            break;

        case CMD_GET_DEVICE_INFO:
            printf("[BLE] GET_DEVICE_INFO - sending device name\n");
            _response_buf[0] = RESP_OK;
            _response_buf[1] = CMD_GET_DEVICE_INFO;
            snprintf((char*)&_response_buf[2], RESPONSE_BUF_SIZE - 2,
                     "{\"name\":\"%s\",\"version\":\"1.0\"}",
                     _service_info.device_name);
            _response_len = 2 + strlen((char*)&_response_buf[2]);
            break;

        case CMD_GET_INSPECTION_COUNT:
            printf("[BLE] GET_INSPECTION_COUNT requested\n");
            // This will be filled in by C++ wrapper
            _response_buf[0] = RESP_OK;
            _response_buf[1] = CMD_GET_INSPECTION_COUNT;
            _response_len = 2;
            // Count will be added by C++ handler
            break;

        case CMD_GET_YARD_COUNT:
            printf("[BLE] GET_YARD_COUNT requested\n");
            _response_buf[0] = RESP_OK;
            _response_buf[1] = CMD_GET_YARD_COUNT;
            _response_len = 2;
            break;

        case CMD_GET_HIVE_COUNT:
            printf("[BLE] GET_HIVE_COUNT requested\n");
            _response_buf[0] = RESP_OK;
            _response_buf[1] = CMD_GET_HIVE_COUNT;
            _response_len = 2;
            break;

        case CMD_GET_INSPECTION_IDS:
            printf("[BLE] GET_INSPECTION_IDS requested\n");
            _response_buf[0] = RESP_DATA_START;
            _response_buf[1] = CMD_GET_INSPECTION_IDS;
            _response_len = 2;
            _service_info.transfer_in_progress = 1;
            break;

        case CMD_GET_INSPECTION:
            if (len >= 5) {
                uint32_t record_id = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
                printf("[BLE] GET_INSPECTION ID=%lu requested\n", (unsigned long)record_id);
                _response_buf[0] = RESP_OK;
                _response_buf[1] = CMD_GET_INSPECTION;
                memcpy(&_response_buf[2], &record_id, 4);
                _response_len = 6;
            } else {
                printf("[BLE] ERROR: GET_INSPECTION missing ID parameter\n");
                _response_buf[0] = RESP_ERROR;
                _response_len = 1;
            }
            break;

        case CMD_GET_YARD:
            if (len >= 5) {
                uint32_t yard_num = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
                printf("[BLE] GET_YARD number=%lu requested\n", (unsigned long)yard_num);
                _response_buf[0] = RESP_OK;
                _response_buf[1] = CMD_GET_YARD;
                memcpy(&_response_buf[2], &yard_num, 4);
                _response_len = 6;
            } else {
                printf("[BLE] ERROR: GET_YARD missing number parameter\n");
                _response_buf[0] = RESP_ERROR;
                _response_len = 1;
            }
            break;

        case CMD_GET_HIVE:
            if (len >= 5) {
                uint32_t hive_num = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
                printf("[BLE] GET_HIVE number=%lu requested\n", (unsigned long)hive_num);
                _response_buf[0] = RESP_OK;
                _response_buf[1] = CMD_GET_HIVE;
                memcpy(&_response_buf[2], &hive_num, 4);
                _response_len = 6;
            } else {
                printf("[BLE] ERROR: GET_HIVE missing number parameter\n");
                _response_buf[0] = RESP_ERROR;
                _response_len = 1;
            }
            break;

        case CMD_GET_YARD_LIST:
            printf("[BLE] GET_YARD_LIST requested\n");
            _response_buf[0] = RESP_DATA_START;
            _response_buf[1] = CMD_GET_YARD_LIST;
            _response_len = 2;
            _service_info.transfer_in_progress = 1;
            break;

        case CMD_GET_HIVES_IN_YARD:
            if (len >= 5) {
                uint32_t yard_num = data[1] | (data[2] << 8) | (data[3] << 16) | (data[4] << 24);
                printf("[BLE] GET_HIVES_IN_YARD yard=%lu requested\n", (unsigned long)yard_num);
                _response_buf[0] = RESP_DATA_START;
                _response_buf[1] = CMD_GET_HIVES_IN_YARD;
                memcpy(&_response_buf[2], &yard_num, 4);
                _response_len = 6;
                _service_info.transfer_in_progress = 1;
            } else {
                printf("[BLE] ERROR: GET_HIVES_IN_YARD missing yard number\n");
                _response_buf[0] = RESP_ERROR;
                _response_len = 1;
            }
            break;

        case CMD_GET_ALL_DATA:
            printf("[BLE] GET_ALL_DATA requested - starting full data transfer\n");
            _response_buf[0] = RESP_DATA_START;
            _response_buf[1] = CMD_GET_ALL_DATA;
            _response_len = 2;
            _service_info.transfer_in_progress = 1;
            _service_info.bytes_sent = 0;
            break;

        default:
            printf("[BLE] ERROR: Unknown command 0x%02X\n", cmd);
            _response_buf[0] = RESP_ERROR;
            _response_len = 1;
            break;
    }

    // Send notification if subscribed
    if (_service_info.is_subscribed && _response_len > 0) {
        struct os_mbuf *om = ble_hs_mbuf_from_flat(_response_buf, _response_len);
        int rc = ble_gatts_notify_custom(_service_info.conn_handle, _response_handle, om);
        if (rc == 0) {
            printf("[BLE] Response sent: %d bytes\n", _response_len);
            _service_info.bytes_sent += _response_len;
        } else {
            printf("[BLE] ERROR: Failed to send notification rc=%d\n", rc);
        }
    }
}

// Command characteristic access callback
static int gatt_svr_chr_access_cmd(uint16_t conn_handle, uint16_t attr_handle,
                                   struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("[BLE] Command characteristic accessed, op=%d\n", ctxt->op);

    if (ctxt->op == BLE_GATT_ACCESS_OP_WRITE_CHR) {
        uint16_t len = OS_MBUF_PKTLEN(ctxt->om);
        uint8_t buf[256];

        if (len > sizeof(buf)) {
            len = sizeof(buf);
        }

        int rc = ble_hs_mbuf_to_flat(ctxt->om, buf, len, NULL);
        if (rc != 0) {
            printf("[BLE] ERROR: Failed to read mbuf rc=%d\n", rc);
            return BLE_ATT_ERR_UNLIKELY;
        }

        process_command(buf, len);
        return 0;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

// Response characteristic access callback
static int gatt_svr_chr_access_response(uint16_t conn_handle, uint16_t attr_handle,
                                        struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("[BLE] Response characteristic accessed, op=%d\n", ctxt->op);

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        int rc = os_mbuf_append(ctxt->om, _response_buf, _response_len);
        printf("[BLE] Response read: %d bytes\n", _response_len);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

// Status characteristic access callback
static int gatt_svr_chr_access_status(uint16_t conn_handle, uint16_t attr_handle,
                                      struct ble_gatt_access_ctxt *ctxt, void *arg)
{
    printf("[BLE] Status characteristic accessed, op=%d\n", ctxt->op);

    if (ctxt->op == BLE_GATT_ACCESS_OP_READ_CHR) {
        // Return simple status: connected, subscribed, transferring
        uint8_t status[4];
        status[0] = _service_info.is_connected;
        status[1] = _service_info.is_subscribed;
        status[2] = _service_info.transfer_in_progress;
        status[3] = ble_data_service_get_progress();

        int rc = os_mbuf_append(ctxt->om, status, sizeof(status));
        printf("[BLE] Status read: connected=%d subscribed=%d transferring=%d progress=%d%%\n",
               status[0], status[1], status[2], status[3]);
        return rc == 0 ? 0 : BLE_ATT_ERR_INSUFFICIENT_RES;
    }

    return BLE_ATT_ERR_UNLIKELY;
}

// Start advertising
static void start_advertise(void)
{
    struct ble_gap_adv_params adv_params;
    struct ble_hs_adv_fields fields;
    int rc;

    memset(&fields, 0, sizeof(fields));
    fields.flags = BLE_HS_ADV_F_DISC_GEN | BLE_HS_ADV_F_BREDR_UNSUP;
    fields.tx_pwr_lvl_is_present = 1;
    fields.tx_pwr_lvl = BLE_HS_ADV_TX_PWR_LVL_AUTO;
    fields.name = (uint8_t *)_device_name;
    fields.name_len = strlen(_device_name);
    fields.name_is_complete = 1;

    rc = ble_gap_adv_set_fields(&fields);
    if (rc != 0) {
        printf("[BLE] ERROR: Failed to set adv fields rc=%d\n", rc);
        return;
    }

    memset(&adv_params, 0, sizeof(adv_params));
    adv_params.conn_mode = BLE_GAP_CONN_MODE_UND;
    adv_params.disc_mode = BLE_GAP_DISC_MODE_GEN;

    rc = ble_gap_adv_start(_addr_type, NULL, BLE_HS_FOREVER, &adv_params, ble_gap_event, NULL);
    if (rc != 0) {
        printf("[BLE] ERROR: Failed to start advertising rc=%d\n", rc);
    } else {
        printf("[BLE] Advertising started as '%s'\n", _device_name);
    }
}

// GAP event handler
static int ble_gap_event(struct ble_gap_event *event, void *arg)
{
    switch (event->type) {
        case BLE_GAP_EVENT_CONNECT:
            if (event->connect.status == 0) {
                printf("[BLE] *** CONNECTED *** conn_handle=%d\n", event->connect.conn_handle);
                _service_info.is_connected = 1;
                _service_info.conn_handle = event->connect.conn_handle;
            } else {
                printf("[BLE] Connection failed, status=%d\n", event->connect.status);
                _service_info.is_connected = 0;
                start_advertise();
            }
            break;

        case BLE_GAP_EVENT_DISCONNECT:
            printf("[BLE] *** DISCONNECTED *** reason=%d\n", event->disconnect.reason);
            _service_info.is_connected = 0;
            _service_info.is_subscribed = 0;
            _service_info.transfer_in_progress = 0;
            start_advertise();
            break;

        case BLE_GAP_EVENT_ADV_COMPLETE:
            printf("[BLE] Advertising complete\n");
            start_advertise();
            break;

        case BLE_GAP_EVENT_SUBSCRIBE:
            printf("[BLE] Subscribe event: handle=%d cur_notify=%d\n",
                   event->subscribe.attr_handle, event->subscribe.cur_notify);
            if (event->subscribe.attr_handle == _response_handle) {
                _service_info.is_subscribed = event->subscribe.cur_notify;
                printf("[BLE] Response notifications %s\n",
                       _service_info.is_subscribed ? "ENABLED" : "DISABLED");
            }
            break;

        case BLE_GAP_EVENT_MTU:
            printf("[BLE] MTU update: conn_handle=%d mtu=%d\n",
                   event->mtu.conn_handle, event->mtu.value);
            break;

        default:
            break;
    }

    return 0;
}

// Called when BLE stack is synced
static void on_sync(void)
{
    int rc = ble_hs_id_infer_auto(0, &_addr_type);
    if (rc != 0) {
        printf("[BLE] ERROR: Failed to infer address type rc=%d\n", rc);
        return;
    }

    uint8_t addr[6] = {0};
    rc = ble_hs_id_copy_addr(_addr_type, addr, NULL);
    printf("[BLE] Device address: %02X:%02X:%02X:%02X:%02X:%02X\n",
           addr[5], addr[4], addr[3], addr[2], addr[1], addr[0]);

    start_advertise();
}

// Called on BLE stack reset
static void on_reset(int reason)
{
    printf("[BLE] Stack reset, reason=%d\n", reason);
}

// BLE host task
static void ble_host_task(void *param)
{
    printf("[BLE] Host task started\n");
    nimble_port_run();
    nimble_port_freertos_deinit();
}

// Initialize GATT server
static int gatt_svr_init(void)
{
    int rc;

    ble_svc_gap_init();
    ble_svc_gatt_init();

    rc = ble_gatts_count_cfg(gatt_svr_svcs);
    if (rc != 0) {
        printf("[BLE] ERROR: ble_gatts_count_cfg failed rc=%d\n", rc);
        return rc;
    }

    rc = ble_gatts_add_svcs(gatt_svr_svcs);
    if (rc != 0) {
        printf("[BLE] ERROR: ble_gatts_add_svcs failed rc=%d\n", rc);
        return rc;
    }

    printf("[BLE] GATT server initialized\n");
    return 0;
}

// Public API implementation
void ble_data_service_start(void)
{
    printf("[BLE] ========================================\n");
    printf("[BLE] Starting BLE Data Transfer Service\n");
    printf("[BLE] ========================================\n");

    // Reset service info
    memset(&_service_info, 0, sizeof(_service_info));

    // Generate device name
    _device_name = (char*)malloc(48);
    uint64_t chip_mac = 0;
    esp_efuse_mac_get_default((uint8_t*)&chip_mac);
    snprintf(_device_name, 48, "BeeData-%lX", (uint32_t)(chip_mac >> 32));
    strncpy(_service_info.device_name, _device_name, sizeof(_service_info.device_name) - 1);

    printf("[BLE] Device name: %s\n", _device_name);

    // Initialize NVS
    esp_err_t ret = nvs_flash_init();
    if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_ERROR_CHECK(nvs_flash_erase());
        ret = nvs_flash_init();
    }
    ESP_ERROR_CHECK(ret);

    // Initialize NimBLE
    ret = nimble_port_init();
    if (ret != ESP_OK) {
        printf("[BLE] ERROR: nimble_port_init failed ret=%d\n", ret);
        return;
    }

    // Configure BLE host
    ble_hs_cfg.sync_cb = on_sync;
    ble_hs_cfg.reset_cb = on_reset;

    // Initialize GATT server
    int rc = gatt_svr_init();
    if (rc != 0) {
        printf("[BLE] ERROR: gatt_svr_init failed rc=%d\n", rc);
        return;
    }

    // Set device name
    rc = ble_svc_gap_device_name_set(_device_name);
    if (rc != 0) {
        printf("[BLE] ERROR: Failed to set device name rc=%d\n", rc);
    }

    // Start BLE host task
    nimble_port_freertos_init(ble_host_task);

    printf("[BLE] Service started successfully\n");
    printf("[BLE] Waiting for connections...\n");
    printf("[BLE] Service UUID: 12345678-1234-5678-1234-56789ABCDEF0\n");
    printf("[BLE] Command Char: 0xFF01 (write)\n");
    printf("[BLE] Response Char: 0xFF02 (read/notify)\n");
    printf("[BLE] Status Char: 0xFF03 (read)\n");
    printf("[BLE] ========================================\n");
}

void ble_data_service_stop(void)
{
    printf("[BLE] Stopping BLE Data Transfer Service\n");

    nimble_port_freertos_deinit();
    nimble_port_deinit();
    ESP_ERROR_CHECK(nvs_flash_deinit());

    if (_device_name) {
        free(_device_name);
        _device_name = NULL;
    }

    memset(&_service_info, 0, sizeof(_service_info));

    printf("[BLE] Service stopped\n");
}

BleDataServiceInfo_t* ble_data_service_get_info(void)
{
    return &_service_info;
}

bool ble_data_service_is_connected(void)
{
    return _service_info.is_connected != 0;
}

bool ble_data_service_is_transferring(void)
{
    return _service_info.transfer_in_progress != 0;
}

uint8_t ble_data_service_get_progress(void)
{
    if (_service_info.total_bytes == 0) {
        return 0;
    }
    return (uint8_t)((_service_info.bytes_sent * 100) / _service_info.total_bytes);
}

// Function to send data notification (called from C++)
void ble_data_service_send_data(const uint8_t* data, uint16_t len)
{
    if (!_service_info.is_subscribed || !_service_info.is_connected) {
        printf("[BLE] Cannot send data: not connected/subscribed\n");
        return;
    }

    struct os_mbuf *om = ble_hs_mbuf_from_flat(data, len);
    int rc = ble_gatts_notify_custom(_service_info.conn_handle, _response_handle, om);
    if (rc == 0) {
        printf("[BLE] Data sent: %d bytes\n", len);
        _service_info.bytes_sent += len;
    } else {
        printf("[BLE] ERROR: Failed to send data rc=%d\n", rc);
    }
}

// Function to mark transfer complete
void ble_data_service_transfer_complete(void)
{
    _service_info.transfer_in_progress = 0;
    printf("[BLE] Transfer complete. Total bytes sent: %lu\n",
           (unsigned long)_service_info.bytes_sent);
}
