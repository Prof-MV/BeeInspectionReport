/**
 * @file yard_storage.cpp
 * @brief NVS storage implementation for yard and hive management
 * @version 1.0
 * @date 2026-02-12
 */
#include "yard_storage.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>
#include <esp_timer.h>
#include <cstdio>
#include <algorithm>

// For inspection data integration
#include "../app_beehive_inspection/inspection_data.h"
#include "../app_beehive_inspection/inspection_storage.h"

static const char* TAG = "yard_storage";

namespace YARD_MANAGEMENT {

// ==================== NVS Initialization ====================

bool initYardManagementNVS() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "NVS flash init failed: %s", esp_err_to_name(err));
        return false;
    }
    ESP_LOGI(TAG, "NVS storage initialized for yard management");
    return true;
}

// ==================== Helper Functions ====================

static void makeYardKey(uint32_t yardNumber, char* key, size_t keySize) {
    snprintf(key, keySize, "%s%06lu", YM_YARD_PREFIX, (unsigned long)yardNumber);
}

static void makeHiveKey(uint32_t hiveNumber, char* key, size_t keySize) {
    snprintf(key, keySize, "%s%06lu", YM_HIVE_PREFIX, (unsigned long)hiveNumber);
}

// ==================== Yard Operations ====================

bool saveYard(const YardRecord& yard) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    // Pack the record
    YardRecordNVS packed;
    packed.fromRecord(yard);

    // Create key
    char key[20];
    makeYardKey(yard.yardNumber, key, sizeof(key));

    // Write to NVS
    err = nvs_set_blob(handle, key, &packed, sizeof(YardRecordNVS));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write yard %lu: %s", (unsigned long)yard.yardNumber, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit yard %lu: %s", (unsigned long)yard.yardNumber, esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Saved yard %lu", (unsigned long)yard.yardNumber);
    return true;
}

bool loadYard(uint32_t yardNumber, YardRecord& yard) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    char key[20];
    makeYardKey(yardNumber, key, sizeof(key));

    YardRecordNVS packed;
    size_t size = sizeof(YardRecordNVS);
    err = nvs_get_blob(handle, key, &packed, &size);
    nvs_close(handle);

    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to read yard %lu: %s", (unsigned long)yardNumber, esp_err_to_name(err));
        }
        return false;
    }

    // Validate CRC
    if (!packed.validateCRC()) {
        ESP_LOGE(TAG, "CRC validation failed for yard %lu", (unsigned long)yardNumber);
        return false;
    }

    packed.toRecord(yard);

    // Calculate active hive count
    yard.activeHiveCount = static_cast<uint16_t>(getActiveHiveCountInYard(yardNumber));

    return true;
}

bool closeYard(uint32_t yardNumber) {
    YardRecord yard;
    if (!loadYard(yardNumber, yard)) {
        return false;
    }

    yard.status = static_cast<char>(YardStatus::CLOSED);
    yard.closureTimestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);

    return saveYard(yard);
}

bool deleteYard(uint32_t yardNumber) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[20];
    makeYardKey(yardNumber, key, sizeof(key));

    err = nvs_erase_key(handle, key);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

std::vector<uint32_t> getAllYardNumbers() {
    std::vector<uint32_t> numbers;
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return numbers;
    }

    // Iterate through possible yard numbers
    // Note: In a production system, you'd maintain an index
    for (uint32_t num = MIN_YARD_NUMBER; num <= MAX_YARD_NUMBER && numbers.size() < MAX_YARDS; num++) {
        char key[20];
        makeYardKey(num, key, sizeof(key));

        size_t size = 0;
        if (nvs_get_blob(handle, key, nullptr, &size) == ESP_OK && size > 0) {
            numbers.push_back(num);
        }
    }

    nvs_close(handle);
    std::sort(numbers.begin(), numbers.end());
    return numbers;
}

std::vector<uint32_t> getActiveYardNumbers() {
    std::vector<uint32_t> allYards = getAllYardNumbers();
    std::vector<uint32_t> activeYards;

    for (uint32_t num : allYards) {
        YardRecord yard;
        if (loadYard(num, yard) && yard.isActive()) {
            activeYards.push_back(num);
        }
    }

    return activeYards;
}

bool isYardNumberUnique(uint32_t yardNumber) {
    YardRecord yard;
    return !loadYard(yardNumber, yard);
}

size_t getYardCount() {
    return getAllYardNumbers().size();
}

size_t getActiveYardCount() {
    return getActiveYardNumbers().size();
}

// ==================== Hive Operations ====================

bool saveHive(const HiveRecord& hive) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    // Pack the record
    HiveRecordNVS packed;
    packed.fromRecord(hive);

    // Create key
    char key[20];
    makeHiveKey(hive.hiveNumber, key, sizeof(key));

    // Write to NVS
    err = nvs_set_blob(handle, key, &packed, sizeof(HiveRecordNVS));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write hive %lu: %s", (unsigned long)hive.hiveNumber, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit hive %lu: %s", (unsigned long)hive.hiveNumber, esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Saved hive %lu", (unsigned long)hive.hiveNumber);
    return true;
}

bool loadHive(uint32_t hiveNumber, HiveRecord& hive) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[20];
    makeHiveKey(hiveNumber, key, sizeof(key));

    HiveRecordNVS packed;
    size_t size = sizeof(HiveRecordNVS);
    err = nvs_get_blob(handle, key, &packed, &size);
    nvs_close(handle);

    if (err != ESP_OK) {
        return false;
    }

    // Validate CRC
    if (!packed.validateCRC()) {
        ESP_LOGE(TAG, "CRC validation failed for hive %lu", (unsigned long)hiveNumber);
        return false;
    }

    packed.toRecord(hive);
    return true;
}

bool closeHive(uint32_t hiveNumber, char reason) {
    HiveRecord hive;
    if (!loadHive(hiveNumber, hive)) {
        return false;
    }

    hive.status = reason;
    hive.closureTimestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    hive.clearRfidTag();  // Remove RFID tag on closure

    return saveHive(hive);
}

bool deleteHive(uint32_t hiveNumber) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[20];
    makeHiveKey(hiveNumber, key, sizeof(key));

    err = nvs_erase_key(handle, key);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);
    return true;
}

std::vector<uint32_t> getAllHiveNumbers() {
    std::vector<uint32_t> numbers;
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return numbers;
    }

    // Iterate through possible hive numbers
    for (uint32_t num = MIN_HIVE_NUMBER; num <= MAX_HIVE_NUMBER && numbers.size() < MAX_HIVES; num++) {
        char key[20];
        makeHiveKey(num, key, sizeof(key));

        size_t size = 0;
        if (nvs_get_blob(handle, key, nullptr, &size) == ESP_OK && size > 0) {
            numbers.push_back(num);
        }
    }

    nvs_close(handle);
    std::sort(numbers.begin(), numbers.end());
    return numbers;
}

std::vector<uint32_t> getHivesInYard(uint32_t yardNumber) {
    std::vector<uint32_t> allHives = getAllHiveNumbers();
    std::vector<uint32_t> yardHives;

    for (uint32_t num : allHives) {
        HiveRecord hive;
        if (loadHive(num, hive) && hive.yardNumber == yardNumber) {
            yardHives.push_back(num);
        }
    }

    return yardHives;
}

std::vector<uint32_t> getActiveHivesInYard(uint32_t yardNumber) {
    std::vector<uint32_t> allHives = getHivesInYard(yardNumber);
    std::vector<uint32_t> activeHives;

    for (uint32_t num : allHives) {
        HiveRecord hive;
        if (loadHive(num, hive) && hive.isActive()) {
            activeHives.push_back(num);
        }
    }

    return activeHives;
}

bool isHiveNumberUnique(uint32_t hiveNumber) {
    HiveRecord hive;
    return !loadHive(hiveNumber, hive);
}

size_t getHiveCount() {
    return getAllHiveNumbers().size();
}

size_t getActiveHiveCount() {
    std::vector<uint32_t> allHives = getAllHiveNumbers();
    size_t count = 0;

    for (uint32_t num : allHives) {
        HiveRecord hive;
        if (loadHive(num, hive) && hive.isActive()) {
            count++;
        }
    }

    return count;
}

size_t getActiveHiveCountInYard(uint32_t yardNumber) {
    return getActiveHivesInYard(yardNumber).size();
}

// ==================== RFID Operations ====================

bool isRfidUnique(const char* rfidTag, bool checkYards, bool checkHives) {
    if (rfidTag == nullptr || rfidTag[0] == '\0') {
        return true;  // Empty tag is considered unique
    }

    if (checkYards) {
        std::vector<uint32_t> yards = getAllYardNumbers();
        for (uint32_t num : yards) {
            YardRecord yard;
            if (loadYard(num, yard) && yard.isActive()) {
                if (strcmp(yard.rfidTagId, rfidTag) == 0) {
                    return false;  // Found duplicate in yards
                }
            }
        }
    }

    if (checkHives) {
        std::vector<uint32_t> hives = getAllHiveNumbers();
        for (uint32_t num : hives) {
            HiveRecord hive;
            if (loadHive(num, hive) && hive.isActive()) {
                if (strcmp(hive.rfidTagId, rfidTag) == 0) {
                    return false;  // Found duplicate in hives
                }
            }
        }
    }

    return true;
}

uint32_t findYardByRfid(const char* rfidTag) {
    if (rfidTag == nullptr || rfidTag[0] == '\0') {
        return 0;
    }

    std::vector<uint32_t> yards = getAllYardNumbers();
    for (uint32_t num : yards) {
        YardRecord yard;
        if (loadYard(num, yard) && strcmp(yard.rfidTagId, rfidTag) == 0) {
            return num;
        }
    }

    return 0;
}

uint32_t findHiveByRfid(const char* rfidTag) {
    if (rfidTag == nullptr || rfidTag[0] == '\0') {
        return 0;
    }

    std::vector<uint32_t> hives = getAllHiveNumbers();
    for (uint32_t num : hives) {
        HiveRecord hive;
        if (loadHive(num, hive) && strcmp(hive.rfidTagId, rfidTag) == 0) {
            return num;
        }
    }

    return 0;
}

// ==================== Statistics ====================

YardSummaryStats calculateYardStats(uint32_t yardNumber) {
    YardSummaryStats stats;
    stats.yardNumber = yardNumber;
    stats.lastCalculated = static_cast<uint32_t>(esp_timer_get_time() / 1000000);

    // Get all active hives in yard
    std::vector<uint32_t> activeHives = getActiveHivesInYard(yardNumber);
    stats.totalHives = static_cast<uint16_t>(activeHives.size());

    if (stats.totalHives == 0) {
        return stats;
    }

    // Calculate statistics from inspection data
    uint16_t supersWithFill = 0;
    float totalFill = 0.0f;

    for (uint32_t hiveNum : activeHives) {
        (void)hiveNum;  // Suppress unused warning until inspection integration is complete
        // Try to get latest inspection for this hive
        // Note: This requires inspection storage integration
        BEEHIVE_INSPECTION::InspectionRecord inspection;

        // For now, we'll check if inspection data exists
        // In full implementation, query by hive ID
        // This is a simplified version - full implementation would
        // iterate through inspection records to find latest for each hive

        // Placeholder: Inspection data integration would go here
        // if (getLatestInspectionForHive(hiveNum, inspection)) {
        //     stats.hasInspectionData = true;
        //     if (inspection.queenRight) {
        //         stats.queenrightHives++;
        //     }
        //     stats.totalSupers += inspection.superCount;
        //     for (int i = 0; i < inspection.superCount; i++) {
        //         auto fill = inspection.superFill[i];
        //         if (fill != BEEHIVE_INSPECTION::FillPercentage::UNSET) {
        //             supersWithFill++;
        //             totalFill += getFillPercentValue(fill);
        //         }
        //     }
        // }
    }

    if (supersWithFill > 0) {
        stats.averageFillPercent = totalFill / supersWithFill;
    }

    return stats;
}

bool updateYardHiveCount(uint32_t yardNumber) {
    YardRecord yard;
    if (!loadYard(yardNumber, yard)) {
        return false;
    }

    yard.activeHiveCount = static_cast<uint16_t>(getActiveHiveCountInYard(yardNumber));
    return saveYard(yard);
}

// ==================== Maintenance ====================

bool clearAllYardManagementData() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    err = nvs_erase_all(handle);
    if (err != ESP_OK) {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);

    ESP_LOGI(TAG, "Cleared all yard management data");
    return true;
}

} // namespace YARD_MANAGEMENT
