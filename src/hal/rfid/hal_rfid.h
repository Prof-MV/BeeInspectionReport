/**
 * @file hal_rfid.h
 * @brief RFID reader interface using rc522 library
 * @version 1.1
 * @date 2026-02-12
 */
#pragma once

// RFID_ENABLE is defined in hal.h - this header should only be included when RFID_ENABLE=1
#include <stdint.h>
#include <stdbool.h>
#include <driver/i2c_master.h>
#include <rc522.h>

namespace HAL {
namespace RFID {

// Callback type for tag detection
typedef void (*TagCallback)(uint64_t serialNumber, void* userData);

class RfidReader {
private:
    rc522_handle_t _rc522Handle;
    bool _initialized;
    bool _scanning;

    // Last scanned tag info
    volatile bool _tagDetected;
    volatile uint64_t _lastTagSerial;

    // Callback for async mode
    TagCallback _tagCallback;
    void* _callbackUserData;

    // Static event handler (C callback)
    static void eventHandler(void* arg, esp_event_base_t base, int32_t event_id, void* event_data);

    // Instance event handler
    void onTagScanned(uint64_t serialNumber);

public:
    RfidReader();
    ~RfidReader();

    /**
     * @brief Initialize the RFID reader using rc522 library
     * @param busHandle Pre-initialized I2C master bus handle
     * @return true if initialization successful
     */
    bool init(i2c_master_bus_handle_t busHandle);

    /**
     * @brief Deinitialize and free resources
     */
    void deinit();

    /**
     * @brief Check if the RFID reader is initialized
     * @return true if reader is ready
     */
    bool isReady() const;

    /**
     * @brief Start scanning for tags
     * @return true if scanning started
     */
    bool startScanning();

    /**
     * @brief Stop/pause scanning
     * @return true if scanning stopped
     */
    bool stopScanning();

    /**
     * @brief Check if scanning is active
     */
    bool isScanning() const;

    /**
     * @brief Check if a tag was detected since last call
     *        Clears the detected flag after returning
     * @return true if a tag was detected
     */
    bool wasTagDetected();

    /**
     * @brief Get the serial number of the last detected tag
     * @return 64-bit serial number
     */
    uint64_t getLastTagSerial() const;

    /**
     * @brief Get the last tag serial as a hex string
     * @param buffer Output buffer (should be at least 17 bytes)
     * @param bufferSize Size of buffer
     * @return true if successful
     */
    bool getLastTagString(char* buffer, size_t bufferSize) const;

    /**
     * @brief Blocking read: wait for a tag and get its serial string
     * @param buffer Output buffer
     * @param bufferSize Size of buffer
     * @param timeoutMs Maximum time to wait (0 = no timeout)
     * @return true if tag was read
     */
    bool getTagUidString(char* buffer, size_t bufferSize, uint32_t timeoutMs = 500);

    /**
     * @brief Set callback for tag detection (async mode)
     * @param callback Function to call when tag is detected
     * @param userData User data to pass to callback
     */
    void setTagCallback(TagCallback callback, void* userData = nullptr);

    /**
     * @brief Clear the last tag detection
     */
    void clearLastTag();
};

} // namespace RFID
} // namespace HAL
