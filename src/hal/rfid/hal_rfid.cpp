/**
 * @file hal_rfid.cpp
 * @brief RFID reader implementation using rc522 library
 * @version 1.1
 * @date 2026-02-12
 */
#include "../hal.h"  // For RFID_ENABLE define

#if RFID_ENABLE
#include "hal_rfid.h"
#include <esp_log.h>
#include <cstring>
#include <cstdio>
#include <cinttypes>

static const char* TAG = "rfid";

namespace HAL {
namespace RFID {

// Static event handler - bridges C callback to C++ instance method
void RfidReader::eventHandler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data) {
    if (event_id == RC522_EVENT_TAG_SCANNED) {
        rc522_event_data_t* data = (rc522_event_data_t*)event_data;
        rc522_tag_t* tag = (rc522_tag_t*)data->ptr;

        // Get the RfidReader instance and call the instance method
        RfidReader* reader = (RfidReader*)arg;
        reader->onTagScanned(tag->serial_number);
    }
}

void RfidReader::onTagScanned(uint64_t serialNumber) {
    ESP_LOGI(TAG, "Tag scanned (sn: %" PRIu64 ")", serialNumber);

    _lastTagSerial = serialNumber;
    _tagDetected = true;

    // Call user callback if set
    if (_tagCallback) {
        _tagCallback(serialNumber, _callbackUserData);
    }
}

RfidReader::RfidReader()
    : _rc522Handle(nullptr)
    , _initialized(false)
    , _scanning(false)
    , _tagDetected(false)
    , _lastTagSerial(0)
    , _tagCallback(nullptr)
    , _callbackUserData(nullptr)
{
}

RfidReader::~RfidReader() {
    deinit();
}

bool RfidReader::init(i2c_port_t i2cPort) {
    if (_initialized) {
        ESP_LOGW(TAG, "RFID reader already initialized");
        return true;
    }

    ESP_LOGI(TAG, "Initializing RFID reader on I2C port %d", i2cPort);

    // Setup config
    rc522_config_t config;
    memset(&config, 0, sizeof(rc522_config_t));
    config.transport = RC522_TRANSPORT_I2C;
    config.i2c.port = i2cPort;

    // Create handler
    esp_err_t err = rc522_create(&config, &_rc522Handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to create rc522 handler: %s", esp_err_to_name(err));
        return false;
    }

    // Register event handler
    err = rc522_register_events(_rc522Handle, RC522_EVENT_ANY, eventHandler, this);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to register rc522 events: %s", esp_err_to_name(err));
        rc522_destroy(_rc522Handle);
        _rc522Handle = nullptr;
        return false;
    }

    // Start scanning
    err = rc522_start(_rc522Handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to start rc522: %s", esp_err_to_name(err));
        rc522_unregister_events(_rc522Handle, RC522_EVENT_ANY, eventHandler);
        rc522_destroy(_rc522Handle);
        _rc522Handle = nullptr;
        return false;
    }

    _initialized = true;
    _scanning = true;
    ESP_LOGI(TAG, "RFID reader initialized and scanning");
    return true;
}

void RfidReader::deinit() {
    if (!_initialized || _rc522Handle == nullptr) {
        return;
    }

    ESP_LOGI(TAG, "Deinitializing RFID reader");

    rc522_unregister_events(_rc522Handle, RC522_EVENT_ANY, eventHandler);
    rc522_destroy(_rc522Handle);
    _rc522Handle = nullptr;
    _initialized = false;
    _scanning = false;
}

bool RfidReader::isReady() const {
    return _initialized;
}

bool RfidReader::startScanning() {
    if (!_initialized || _rc522Handle == nullptr) {
        return false;
    }

    esp_err_t err = rc522_start(_rc522Handle);
    if (err == ESP_OK) {
        _scanning = true;
        return true;
    }
    return false;
}

bool RfidReader::stopScanning() {
    if (!_initialized || _rc522Handle == nullptr) {
        return false;
    }

    esp_err_t err = rc522_pause(_rc522Handle);
    if (err == ESP_OK) {
        _scanning = false;
        return true;
    }
    return false;
}

bool RfidReader::isScanning() const {
    return _scanning;
}

bool RfidReader::wasTagDetected() {
    bool detected = _tagDetected;
    _tagDetected = false;  // Clear flag
    return detected;
}

uint64_t RfidReader::getLastTagSerial() const {
    return _lastTagSerial;
}

bool RfidReader::getLastTagString(char* buffer, size_t bufferSize) const {
    if (bufferSize < 17) {  // Need at least 16 hex chars + null
        return false;
    }

    if (_lastTagSerial == 0) {
        return false;
    }

    snprintf(buffer, bufferSize, "%016" PRIX64, _lastTagSerial);
    return true;
}

bool RfidReader::getTagUidString(char* buffer, size_t bufferSize, uint32_t timeoutMs) {
    if (!_initialized) {
        ESP_LOGW(TAG, "RFID reader not initialized");
        return false;
    }

    // Make sure we're scanning
    if (!_scanning) {
        startScanning();
    }

    // If a tag was already detected, use it immediately
    if (_tagDetected && _lastTagSerial != 0) {
        _tagDetected = false;  // Clear for next detection
        return getLastTagString(buffer, bufferSize);
    }

    // Wait for a tag
    uint32_t startTime = xTaskGetTickCount() * portTICK_PERIOD_MS;
    while (!_tagDetected) {
        vTaskDelay(pdMS_TO_TICKS(10));

        if (timeoutMs > 0) {
            uint32_t elapsed = (xTaskGetTickCount() * portTICK_PERIOD_MS) - startTime;
            if (elapsed >= timeoutMs) {
                ESP_LOGD(TAG, "Tag read timeout");
                return false;
            }
        }
    }

    // Got a tag, format it
    return getLastTagString(buffer, bufferSize);
}

void RfidReader::setTagCallback(TagCallback callback, void* userData) {
    _tagCallback = callback;
    _callbackUserData = userData;
}

void RfidReader::clearLastTag() {
    _tagDetected = false;
    _lastTagSerial = 0;
}

} // namespace RFID
} // namespace HAL

#endif // RFID_ENABLE
