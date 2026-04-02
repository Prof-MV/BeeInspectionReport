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

// ==================== Index Management ====================
// Indexes store lists of yard/hive numbers for fast lookup

static std::vector<uint32_t> loadYardIndex() {
    std::vector<uint32_t> index;
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return index;
    }

    size_t size = 0;
    if (nvs_get_blob(handle, YM_YARD_INDEX_KEY, nullptr, &size) == ESP_OK && size > 0) {
        size_t count = size / sizeof(uint32_t);
        index.resize(count);
        nvs_get_blob(handle, YM_YARD_INDEX_KEY, index.data(), &size);
    }

    nvs_close(handle);
    return index;
}

static bool saveYardIndex(const std::vector<uint32_t>& index) {
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        return false;
    }

    esp_err_t err;
    if (index.empty()) {
        err = nvs_erase_key(handle, YM_YARD_INDEX_KEY);
        if (err == ESP_ERR_NVS_NOT_FOUND) err = ESP_OK;
    } else {
        err = nvs_set_blob(handle, YM_YARD_INDEX_KEY, index.data(), index.size() * sizeof(uint32_t));
    }

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err == ESP_OK;
}

static bool addToYardIndex(uint32_t yardNumber) {
    std::vector<uint32_t> index = loadYardIndex();

    for (uint32_t num : index) {
        if (num == yardNumber) return true;  // Already exists
    }

    if (index.size() >= MAX_YARDS) {
        ESP_LOGE(TAG, "Max yards limit reached");
        return false;
    }

    index.push_back(yardNumber);
    return saveYardIndex(index);
}

static std::vector<uint32_t> loadHiveIndex() {
    std::vector<uint32_t> index;
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return index;
    }

    size_t size = 0;
    if (nvs_get_blob(handle, YM_HIVE_INDEX_KEY, nullptr, &size) == ESP_OK && size > 0) {
        size_t count = size / sizeof(uint32_t);
        index.resize(count);
        nvs_get_blob(handle, YM_HIVE_INDEX_KEY, index.data(), &size);
    }

    nvs_close(handle);
    return index;
}

static bool saveHiveIndex(const std::vector<uint32_t>& index) {
    nvs_handle_t handle;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        return false;
    }

    esp_err_t err;
    if (index.empty()) {
        err = nvs_erase_key(handle, YM_HIVE_INDEX_KEY);
        if (err == ESP_ERR_NVS_NOT_FOUND) err = ESP_OK;
    } else {
        err = nvs_set_blob(handle, YM_HIVE_INDEX_KEY, index.data(), index.size() * sizeof(uint32_t));
    }

    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    return err == ESP_OK;
}

static bool addToHiveIndex(uint32_t hiveNumber) {
    std::vector<uint32_t> index = loadHiveIndex();

    for (uint32_t num : index) {
        if (num == hiveNumber) return true;
    }

    if (index.size() >= MAX_HIVES) {
        ESP_LOGE(TAG, "Max hives limit reached");
        return false;
    }

    index.push_back(hiveNumber);
    return saveHiveIndex(index);
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

    // Add to index
    if (!addToYardIndex(yard.yardNumber)) {
        ESP_LOGW(TAG, "Failed to add yard %lu to index", (unsigned long)yard.yardNumber);
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
    std::vector<uint32_t> numbers = loadYardIndex();
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
    bool dataExists = loadYard(yardNumber, yard);

    // Also check the index
    std::vector<uint32_t> index = loadYardIndex();
    bool inIndex = false;
    for (uint32_t num : index) {
        if (num == yardNumber) {
            inIndex = true;
            break;
        }
    }

    // If data exists but not in index, try to repair by adding to index
    if (dataExists && !inIndex) {
        ESP_LOGW(TAG, "Yard %lu exists but not in index, repairing...", (unsigned long)yardNumber);
        addToYardIndex(yardNumber);
    }

    // If in index but data doesn't exist, remove from index (orphaned entry)
    if (!dataExists && inIndex) {
        ESP_LOGW(TAG, "Yard %lu in index but data missing, cleaning up...", (unsigned long)yardNumber);
        std::vector<uint32_t> newIndex;
        for (uint32_t num : index) {
            if (num != yardNumber) {
                newIndex.push_back(num);
            }
        }
        saveYardIndex(newIndex);
        return true;  // Number is now available
    }

    return !dataExists;
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

    // Add to index
    if (!addToHiveIndex(hive.hiveNumber)) {
        ESP_LOGW(TAG, "Failed to add hive %lu to index", (unsigned long)hive.hiveNumber);
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
    std::vector<uint32_t> numbers = loadHiveIndex();
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

    // Load all inspection IDs once, then find the latest completed inspection per hive
    std::vector<uint32_t> allInspectionIds = BEEHIVE_INSPECTION::getAllInspectionIDs();

    for (uint32_t hiveNum : activeHives) {
        uint32_t latestTimestamp = 0;
        BEEHIVE_INSPECTION::InspectionRecord latestInspection;
        bool found = false;

        for (uint32_t inspId : allInspectionIds) {
            BEEHIVE_INSPECTION::InspectionRecord inspection;
            if (BEEHIVE_INSPECTION::loadInspectionFromNVS(inspId, inspection)) {
                if (inspection.hiveId == hiveNum && inspection.isComplete &&
                        inspection.timestamp >= latestTimestamp) {
                    latestTimestamp = inspection.timestamp;
                    latestInspection = inspection;
                    found = true;
                }
            }
        }

        if (found) {
            stats.hasInspectionData = true;
            if (latestInspection.queenRight) {
                stats.queenrightHives++;
            }
            stats.totalSupers += latestInspection.superCount;
            for (int i = 0; i < latestInspection.superCount; i++) {
                auto fill = latestInspection.superFill[i];
                if (fill != BEEHIVE_INSPECTION::FillPercentage::UNSET) {
                    float fillValue = 0.0f;
                    switch (fill) {
                        case BEEHIVE_INSPECTION::FillPercentage::PERCENT_25:  fillValue = 25.0f; break;
                        case BEEHIVE_INSPECTION::FillPercentage::PERCENT_50:  fillValue = 50.0f; break;
                        case BEEHIVE_INSPECTION::FillPercentage::PERCENT_75:  fillValue = 75.0f; break;
                        case BEEHIVE_INSPECTION::FillPercentage::PERCENT_100: fillValue = 100.0f; break;
                        default: break;
                    }
                    supersWithFill++;
                    totalFill += fillValue;
                }
            }
        }
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

// ==================== Equipment Plan Operations ====================

static void makeEquipmentPlanKey(uint32_t yardNumber, char* key, size_t keySize) {
    snprintf(key, keySize, "eqp_%06lu", (unsigned long)yardNumber);
}

bool saveEquipmentPlan(const EquipmentPlan& plan) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS: %s", esp_err_to_name(err));
        return false;
    }

    // Pack the record
    EquipmentPlanNVS packed;
    packed.fromRecord(plan);

    // Create key
    char key[20];
    makeEquipmentPlanKey(plan.yardNumber, key, sizeof(key));

    // Write to NVS
    err = nvs_set_blob(handle, key, &packed, sizeof(EquipmentPlanNVS));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to write equipment plan for yard %lu: %s",
                 (unsigned long)plan.yardNumber, esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit equipment plan for yard %lu: %s",
                 (unsigned long)plan.yardNumber, esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Saved equipment plan for yard %lu", (unsigned long)plan.yardNumber);
    return true;
}

bool loadEquipmentPlan(uint32_t yardNumber, EquipmentPlan& plan) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[20];
    makeEquipmentPlanKey(yardNumber, key, sizeof(key));

    EquipmentPlanNVS packed;
    size_t size = sizeof(EquipmentPlanNVS);
    err = nvs_get_blob(handle, key, &packed, &size);
    nvs_close(handle);

    if (err != ESP_OK) {
        if (err != ESP_ERR_NVS_NOT_FOUND) {
            ESP_LOGE(TAG, "Failed to read equipment plan for yard %lu: %s",
                     (unsigned long)yardNumber, esp_err_to_name(err));
        }
        return false;
    }

    // Validate CRC
    if (!packed.validateCRC()) {
        ESP_LOGE(TAG, "CRC validation failed for equipment plan yard %lu",
                 (unsigned long)yardNumber);
        return false;
    }

    packed.toRecord(plan);
    return true;
}

bool deleteEquipmentPlan(uint32_t yardNumber) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    char key[20];
    makeEquipmentPlanKey(yardNumber, key, sizeof(key));

    err = nvs_erase_key(handle, key);
    if (err != ESP_OK && err != ESP_ERR_NVS_NOT_FOUND) {
        nvs_close(handle);
        return false;
    }

    nvs_commit(handle);
    nvs_close(handle);
    ESP_LOGI(TAG, "Deleted equipment plan for yard %lu", (unsigned long)yardNumber);
    return true;
}

std::vector<uint32_t> getYardsWithEquipmentPlans() {
    std::vector<uint32_t> result;
    std::vector<uint32_t> allYards = getAllYardNumbers();

    for (uint32_t yardNum : allYards) {
        EquipmentPlan plan;
        if (loadEquipmentPlan(yardNum, plan)) {
            result.push_back(yardNum);
        }
    }

    std::sort(result.begin(), result.end());
    return result;
}

size_t getEquipmentPlanCount() {
    return getYardsWithEquipmentPlans().size();
}

bool clearAllEquipmentPlans() {
    std::vector<uint32_t> yardsWithPlans = getYardsWithEquipmentPlans();
    bool success = true;

    for (uint32_t yardNum : yardsWithPlans) {
        if (!deleteEquipmentPlan(yardNum)) {
            success = false;
        }
    }

    ESP_LOGI(TAG, "Cleared %d equipment plans", (int)yardsWithPlans.size());
    return success;
}

// ==================== Maintenance ====================

bool clearAllYardManagementData() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to open NVS for clear: %s", esp_err_to_name(err));
        return false;
    }

    err = nvs_erase_all(handle);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to erase yard data: %s", esp_err_to_name(err));
        nvs_close(handle);
        return false;
    }

    err = nvs_commit(handle);
    nvs_close(handle);

    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to commit yard data erase: %s", esp_err_to_name(err));
        return false;
    }

    ESP_LOGI(TAG, "Cleared all yard management data");
    return true;
}

// ==================== Inspection Integration ====================

static const char* SELECTED_HIVE_KEY = "sel_hive";

bool storeSelectedHiveForInspection(uint32_t hiveNumber) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    err = nvs_set_u32(handle, SELECTED_HIVE_KEY, hiveNumber);
    if (err == ESP_OK) {
        err = nvs_commit(handle);
    }

    nvs_close(handle);
    ESP_LOGI(TAG, "Stored selected hive %lu for inspection", (unsigned long)hiveNumber);
    return err == ESP_OK;
}

uint32_t getSelectedHiveForInspection() {
    nvs_handle_t handle;
    uint32_t hiveNumber = 0;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READONLY, &handle) != ESP_OK) {
        return 0;
    }

    nvs_get_u32(handle, SELECTED_HIVE_KEY, &hiveNumber);
    nvs_close(handle);
    return hiveNumber;
}

bool clearSelectedHiveForInspection() {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    err = nvs_erase_key(handle, SELECTED_HIVE_KEY);
    if (err == ESP_ERR_NVS_NOT_FOUND) {
        err = ESP_OK;  // Key didn't exist, that's fine
    }

    if (err == ESP_OK) {
        nvs_commit(handle);
    }

    nvs_close(handle);
    return err == ESP_OK;
}

static const char* LAUNCH_INSPECTION_KEY = "launch_insp";

bool setLaunchInspectionFlag(bool launch) {
    nvs_handle_t handle;
    esp_err_t err = nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err != ESP_OK) {
        return false;
    }

    if (launch) {
        err = nvs_set_u8(handle, LAUNCH_INSPECTION_KEY, 1);
    } else {
        err = nvs_erase_key(handle, LAUNCH_INSPECTION_KEY);
        if (err == ESP_ERR_NVS_NOT_FOUND) {
            err = ESP_OK;
        }
    }

    if (err == ESP_OK) {
        nvs_commit(handle);
    }

    nvs_close(handle);
    ESP_LOGI(TAG, "Set launch inspection flag: %s", launch ? "true" : "false");
    return err == ESP_OK;
}

bool shouldLaunchInspection() {
    nvs_handle_t handle;
    uint8_t value = 0;

    if (nvs_open(YM_NVS_NAMESPACE, NVS_READWRITE, &handle) != ESP_OK) {
        return false;
    }

    esp_err_t err = nvs_get_u8(handle, LAUNCH_INSPECTION_KEY, &value);

    // Clear the flag after reading
    if (err == ESP_OK && value != 0) {
        nvs_erase_key(handle, LAUNCH_INSPECTION_KEY);
        nvs_commit(handle);
    }

    nvs_close(handle);
    return (err == ESP_OK && value != 0);
}

} // namespace YARD_MANAGEMENT
