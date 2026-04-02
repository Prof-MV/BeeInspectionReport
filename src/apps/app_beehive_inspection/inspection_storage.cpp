/**
 * @file inspection_storage.cpp
 * @brief NVS storage implementation for beehive inspection records
 * @version 1.0
 * @date 2026-02-08
 */
#include "inspection_storage.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <esp_log.h>
#include <cstring>
#include <cstdio>

static const char* TAG = "inspection_storage";

namespace BEEHIVE_INSPECTION {

// InspectionRecordNVS conversion implementations
void InspectionRecordNVS::fromRecord(const InspectionRecord& rec) {
    recordId = rec.recordId;
    hiveId = rec.hiveId;
    timestamp = rec.timestamp;

    queenRight = rec.queenRight ? 1 : 2;
    supersedureCells = static_cast<uint32_t>(rec.supersedureCells);
    swarmCells = static_cast<uint32_t>(rec.swarmCells);
    superCount = rec.superCount;
    temperament = static_cast<uint32_t>(rec.temperament);
    broodFrames = rec.broodFrames;
    treatment = static_cast<uint32_t>(rec.treatment);
    pestFlags = rec.pests;
    isComplete = rec.isComplete ? 1 : 0;
    reserved = 0;

    for (int i = 0; i < 5; i++) {
        superFills[i] = static_cast<uint8_t>(rec.superFill[i]);
    }

    updateCRC();
}

void InspectionRecordNVS::toRecord(InspectionRecord& rec) const {
    rec.recordId = recordId;
    rec.hiveId = hiveId;
    rec.timestamp = timestamp;

    rec.queenRight = (queenRight == 1);
    rec.supersedureCells = static_cast<CellCount>(supersedureCells);
    rec.swarmCells = static_cast<CellCount>(swarmCells);
    rec.superCount = superCount;
    rec.temperament = static_cast<Temperament>(temperament);
    rec.broodFrames = broodFrames;
    rec.treatment = static_cast<Treatment>(treatment);
    rec.pests = pestFlags;
    rec.isComplete = (isComplete != 0);

    for (int i = 0; i < 5; i++) {
        rec.superFill[i] = static_cast<FillPercentage>(superFills[i]);
    }
}

void InspectionRecordNVS::updateCRC() {
    // Calculate CRC over all fields except the CRC itself
    std::size_t dataSize = sizeof(InspectionRecordNVS) - sizeof(crc16);
    crc16 = calculateCRC16(reinterpret_cast<const uint8_t*>(this), dataSize);
}

bool InspectionRecordNVS::validateCRC() const {
    std::size_t dataSize = sizeof(InspectionRecordNVS) - sizeof(crc16);
    uint16_t calculatedCRC = calculateCRC16(reinterpret_cast<const uint8_t*>(this), dataSize);
    return calculatedCRC == crc16;
}

bool initNVSStorage() {
    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        ESP_LOGW(TAG, "NVS partition was truncated, erasing...");
        ESP_ERROR_CHECK(nvs_flash_erase());
        err = nvs_flash_init();
    }

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to initialize NVS: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "NVS storage initialized successfully");
    return true;
}

bool saveInspectionToNVS(const InspectionRecord& record) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    // Convert to packed format
    InspectionRecordNVS packed;
    packed.fromRecord(record);

    // Create key from record ID
    char key[16];
    snprintf(key, sizeof(key), "%s%04lu", NVS_KEY_PREFIX, (unsigned long)record.recordId);

    // Write to NVS
    err = nvs_set_blob(handle, key, &packed, sizeof(InspectionRecordNVS));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write record %lu: %s", (unsigned long)record.recordId, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit NVS: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Saved inspection record %lu", (unsigned long)record.recordId);
    return true;
}

bool loadInspectionFromNVS(uint32_t recordId, InspectionRecord& record) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS for reading: %s", esp_err_to_name(err));
        return false;
    }

    // Create key from record ID
    char key[16];
    snprintf(key, sizeof(key), "%s%04lu", NVS_KEY_PREFIX, (unsigned long)recordId);

    // Read from NVS
    InspectionRecordNVS packed;
    std::size_t size = sizeof(InspectionRecordNVS);
    err = nvs_get_blob(handle, key, &packed, &size);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to read record %lu: %s", (unsigned long)recordId, esp_err_to_name(err));
        return false;
    }

    // Validate CRC
    if (!packed.validateCRC()) {
        ESP_LOGE(TAG, "CRC validation failed for record %lu", (unsigned long)recordId);
        return false;
    }

    // Convert to record format
    packed.toRecord(record);
    ESP_LOGI(TAG, "Loaded inspection record %lu", (unsigned long)recordId);
    return true;
}

std::vector<uint32_t> getAllInspectionIDs() {
    std::vector<uint32_t> ids;

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return ids;
    }

    // Iterate through possible record IDs
    // This is a simple approach - for production, consider using nvs_iterator
    for (uint32_t i = 0; i < MAX_INSPECTION_RECORDS * 2; i++) {
        char key[16];
        snprintf(key, sizeof(key), "%s%04lu", NVS_KEY_PREFIX, (unsigned long)i);

        std::size_t size = 0;
        err = nvs_get_blob(handle, key, nullptr, &size);
        if (err == ESP_OK && size > 0) {
            ids.push_back(i);
        }
    }

    nvs_close(handle);
    return ids;
}

bool deleteInspectionFromNVS(uint32_t recordId) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[16];
    snprintf(key, sizeof(key), "%s%04lu", NVS_KEY_PREFIX, (unsigned long)recordId);

    err = nvs_erase_key(handle, key);
    if (err == ESP_OK) {
        nvs_commit(handle);
    }

    nvs_close(handle);
    return err == ESP_OK;
}

uint32_t getNextRecordID() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return 1;
    }

    uint32_t nextId = 1;
    err = nvs_get_u32(handle, NVS_KEY_NEXT_ID, &nextId);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        nextId = 1;
    }

    // Increment and save for next time
    uint32_t savedId = nextId + 1;
    nvs_set_u32(handle, NVS_KEY_NEXT_ID, savedId);
    nvs_commit(handle);
    nvs_close(handle);

    return nextId;
}

bool getNVSStats(NVSStats& stats) {
    nvs_stats_t nvs_stats;
    esp_err_t err = nvs_get_stats(NVS_DEFAULT_PART_NAME, &nvs_stats);
    if (err != ESP_OK) {
        return false;
    }

    stats.usedEntries = nvs_stats.used_entries;
    stats.freeEntries = nvs_stats.free_entries;
    stats.totalBytes = nvs_stats.total_entries * 32;  // Approximate
    stats.usedBytes = nvs_stats.used_entries * 32;    // Approximate

    return true;
}

bool clearAllInspections() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS for clear: %s", esp_err_to_name(err));
        return false;
    }

    err = nvs_erase_all(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase inspection data: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit inspection data erase: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "All inspection records cleared");
    return true;
}

std::size_t getInspectionCount() {
    return getAllInspectionIDs().size();
}

} // namespace BEEHIVE_INSPECTION
