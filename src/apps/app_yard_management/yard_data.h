/**
 * @file yard_data.h
 * @brief Data structures for yard and hive management
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include <stdint.h>
#include <cstddef>
#include <cstring>

namespace YARD_MANAGEMENT {

// NVS Storage constants
constexpr const char* YM_NVS_NAMESPACE = "yard_mgmt";
constexpr const char* YM_YARD_PREFIX = "yard_";
constexpr const char* YM_HIVE_PREFIX = "hive_";
constexpr const char* YM_YARD_INDEX_KEY = "yard_idx";
constexpr const char* YM_HIVE_INDEX_KEY = "hive_idx";

// Number range constants
constexpr uint32_t MIN_YARD_NUMBER = 100000;
constexpr uint32_t MAX_YARD_NUMBER = 999999;
constexpr uint32_t MIN_HIVE_NUMBER = 100000;
constexpr uint32_t MAX_HIVE_NUMBER = 999999;

// Limits
constexpr size_t MAX_YARDS = 100;
constexpr size_t MAX_HIVES = 1000;
constexpr size_t MAX_NICKNAME_LENGTH = 20;
constexpr size_t RFID_TAG_LENGTH = 16;

// Status enums
enum class YardStatus : char {
    ACTIVE = 'A',
    CLOSED = 'C'
};

enum class HiveStatus : char {
    ACTIVE = 'A',
    SOLD = 'S',
    DEAD = 'D',
    ABSCONDED = 'O'
};

enum class HiveOriginType : uint8_t {
    SWARM = 0,
    SPLIT = 1
};

// Helper functions for status conversion
inline const char* getYardStatusString(YardStatus status) {
    switch (status) {
        case YardStatus::ACTIVE: return "Active";
        case YardStatus::CLOSED: return "Closed";
        default: return "Unknown";
    }
}

inline const char* getHiveStatusString(HiveStatus status) {
    switch (status) {
        case HiveStatus::ACTIVE: return "Active";
        case HiveStatus::SOLD: return "Sold";
        case HiveStatus::DEAD: return "Dead";
        case HiveStatus::ABSCONDED: return "Absconded";
        default: return "Unknown";
    }
}

inline const char* getHiveOriginString(HiveOriginType origin) {
    switch (origin) {
        case HiveOriginType::SWARM: return "Swarm";
        case HiveOriginType::SPLIT: return "Split";
        default: return "Unknown";
    }
}

// Runtime yard record structure
struct YardRecord {
    uint32_t yardNumber;            // Unique 6-digit number (100000-999999)
    char nickname[MAX_NICKNAME_LENGTH + 1];  // Yard nickname (max 20 chars + null)
    char rfidTagId[RFID_TAG_LENGTH + 1];     // RFID tag ID for gate (16 hex chars + null)
    uint32_t creationTimestamp;     // Unix timestamp when yard was created
    uint32_t closureTimestamp;      // Unix timestamp when closed (0 if active)
    char status;                    // 'A' = Active, 'C' = Closed
    uint16_t activeHiveCount;       // Count of active hives (for quick reference)

    // Default constructor
    YardRecord() {
        reset();
    }

    // Reset to defaults
    void reset() {
        yardNumber = MIN_YARD_NUMBER;
        memset(nickname, 0, sizeof(nickname));
        memset(rfidTagId, 0, sizeof(rfidTagId));
        creationTimestamp = 0;
        closureTimestamp = 0;
        status = static_cast<char>(YardStatus::ACTIVE);
        activeHiveCount = 0;
    }

    // Check if active
    bool isActive() const {
        return status == static_cast<char>(YardStatus::ACTIVE);
    }

    // Set nickname safely
    void setNickname(const char* name) {
        strncpy(nickname, name, MAX_NICKNAME_LENGTH);
        nickname[MAX_NICKNAME_LENGTH] = '\0';
    }

    // Set RFID tag safely
    void setRfidTag(const char* tag) {
        strncpy(rfidTagId, tag, RFID_TAG_LENGTH);
        rfidTagId[RFID_TAG_LENGTH] = '\0';
    }
};

// Runtime hive record structure
struct HiveRecord {
    uint32_t hiveNumber;            // Unique 6-digit number (100000-999999)
    uint32_t yardNumber;            // Reference to parent yard's 6-digit number
    HiveOriginType originType;      // SWARM or SPLIT
    uint32_t originHiveNumber;      // If split: origin hive 6-digit number, else 0
    char rfidTagId[RFID_TAG_LENGTH + 1];  // RFID tag ID (empty if closed)
    uint32_t creationTimestamp;     // Unix timestamp when hive was created
    uint32_t closureTimestamp;      // Unix timestamp when closed (0 if active)
    char status;                    // 'A' = Active, 'S' = Sold, 'D' = Dead, 'O' = Absconded

    // Default constructor
    HiveRecord() {
        reset();
    }

    // Reset to defaults
    void reset() {
        hiveNumber = MIN_HIVE_NUMBER;
        yardNumber = 0;
        originType = HiveOriginType::SWARM;
        originHiveNumber = 0;
        memset(rfidTagId, 0, sizeof(rfidTagId));
        creationTimestamp = 0;
        closureTimestamp = 0;
        status = static_cast<char>(HiveStatus::ACTIVE);
    }

    // Check if active
    bool isActive() const {
        return status == static_cast<char>(HiveStatus::ACTIVE);
    }

    // Set RFID tag safely
    void setRfidTag(const char* tag) {
        strncpy(rfidTagId, tag, RFID_TAG_LENGTH);
        rfidTagId[RFID_TAG_LENGTH] = '\0';
    }

    // Clear RFID tag (on closure)
    void clearRfidTag() {
        memset(rfidTagId, 0, sizeof(rfidTagId));
    }
};

// NVS packed yard structure (53 bytes)
struct __attribute__((packed)) YardRecordNVS {
    uint32_t yardNumber;            // 4 bytes
    char nickname[MAX_NICKNAME_LENGTH + 1];  // 21 bytes
    char rfidTagId[RFID_TAG_LENGTH + 1];     // 17 bytes
    uint32_t creationTimestamp;     // 4 bytes
    uint32_t closureTimestamp;      // 4 bytes
    uint8_t status;                 // 1 byte
    uint16_t crc16;                 // 2 bytes

    // Convert from runtime record
    void fromRecord(const YardRecord& rec) {
        yardNumber = rec.yardNumber;
        memcpy(nickname, rec.nickname, sizeof(nickname));
        memcpy(rfidTagId, rec.rfidTagId, sizeof(rfidTagId));
        creationTimestamp = rec.creationTimestamp;
        closureTimestamp = rec.closureTimestamp;
        status = static_cast<uint8_t>(rec.status);
        updateCRC();
    }

    // Convert to runtime record
    void toRecord(YardRecord& rec) const {
        rec.yardNumber = yardNumber;
        memcpy(rec.nickname, nickname, sizeof(nickname));
        memcpy(rec.rfidTagId, rfidTagId, sizeof(rfidTagId));
        rec.creationTimestamp = creationTimestamp;
        rec.closureTimestamp = closureTimestamp;
        rec.status = static_cast<char>(status);
        rec.activeHiveCount = 0;  // Calculated separately
    }

    // Calculate and set CRC
    void updateCRC() {
        crc16 = calculateCRC16(reinterpret_cast<const uint8_t*>(this),
                               sizeof(YardRecordNVS) - sizeof(crc16));
    }

    // Validate CRC
    bool validateCRC() const {
        uint16_t calculated = calculateCRC16(reinterpret_cast<const uint8_t*>(this),
                                             sizeof(YardRecordNVS) - sizeof(crc16));
        return calculated == crc16;
    }

    // CRC16 calculation (CCITT polynomial)
    static uint16_t calculateCRC16(const uint8_t* data, size_t length) {
        uint16_t crc = 0xFFFF;
        for (size_t i = 0; i < length; i++) {
            crc ^= static_cast<uint16_t>(data[i]) << 8;
            for (int j = 0; j < 8; j++) {
                if (crc & 0x8000) {
                    crc = (crc << 1) ^ 0x1021;
                } else {
                    crc <<= 1;
                }
            }
        }
        return crc;
    }
};

// NVS packed hive structure (49 bytes)
struct __attribute__((packed)) HiveRecordNVS {
    uint32_t hiveNumber;            // 4 bytes
    uint32_t yardNumber;            // 4 bytes
    uint8_t originType;             // 1 byte
    uint32_t originHiveNumber;      // 4 bytes
    char rfidTagId[RFID_TAG_LENGTH + 1];  // 17 bytes
    uint32_t creationTimestamp;     // 4 bytes
    uint32_t closureTimestamp;      // 4 bytes
    uint8_t status;                 // 1 byte
    uint16_t crc16;                 // 2 bytes

    // Convert from runtime record
    void fromRecord(const HiveRecord& rec) {
        hiveNumber = rec.hiveNumber;
        yardNumber = rec.yardNumber;
        originType = static_cast<uint8_t>(rec.originType);
        originHiveNumber = rec.originHiveNumber;
        memcpy(rfidTagId, rec.rfidTagId, sizeof(rfidTagId));
        creationTimestamp = rec.creationTimestamp;
        closureTimestamp = rec.closureTimestamp;
        status = static_cast<uint8_t>(rec.status);
        updateCRC();
    }

    // Convert to runtime record
    void toRecord(HiveRecord& rec) const {
        rec.hiveNumber = hiveNumber;
        rec.yardNumber = yardNumber;
        rec.originType = static_cast<HiveOriginType>(originType);
        rec.originHiveNumber = originHiveNumber;
        memcpy(rec.rfidTagId, rfidTagId, sizeof(rfidTagId));
        rec.creationTimestamp = creationTimestamp;
        rec.closureTimestamp = closureTimestamp;
        rec.status = static_cast<char>(status);
    }

    // Calculate and set CRC
    void updateCRC() {
        crc16 = YardRecordNVS::calculateCRC16(reinterpret_cast<const uint8_t*>(this),
                                              sizeof(HiveRecordNVS) - sizeof(crc16));
    }

    // Validate CRC
    bool validateCRC() const {
        uint16_t calculated = YardRecordNVS::calculateCRC16(
            reinterpret_cast<const uint8_t*>(this),
            sizeof(HiveRecordNVS) - sizeof(crc16));
        return calculated == crc16;
    }
};

// Yard summary statistics (computed from inspection data)
struct YardSummaryStats {
    uint32_t yardNumber;            // Yard being summarized
    uint16_t totalHives;            // Count of active hives (status 'A')
    uint16_t queenrightHives;       // Count of queenright hives (from inspections)
    uint16_t totalSupers;           // Total supers across all hives
    float averageFillPercent;       // Average fill % (0-100)
    uint32_t lastCalculated;        // Timestamp of last calculation
    bool hasInspectionData;         // true if any inspection data exists

    // Default constructor
    YardSummaryStats() {
        reset();
    }

    // Reset to defaults
    void reset() {
        yardNumber = 0;
        totalHives = 0;
        queenrightHives = 0;
        totalSupers = 0;
        averageFillPercent = 0.0f;
        lastCalculated = 0;
        hasInspectionData = false;
    }
};

// Equipment types for planning
enum class EquipmentType : uint8_t {
    BROOD_BOXES = 0,
    SUPERS = 1,
    BOTTOM_BOARDS = 2,
    INNER_LIDS = 3,
    OUTER_LIDS = 4,
    QUEEN_EXCLUDERS = 5,
    HIVE_STANDS = 6,
    INSULATION = 7,
    ENTRANCE_REDUCERS = 8,
    FEEDERS = 9,
    EQUIPMENT_COUNT = 10
};

// Equipment names for display
inline const char* getEquipmentName(EquipmentType type) {
    switch (type) {
        case EquipmentType::BROOD_BOXES: return "Brood Boxes";
        case EquipmentType::SUPERS: return "Supers";
        case EquipmentType::BOTTOM_BOARDS: return "Bottom Boards";
        case EquipmentType::INNER_LIDS: return "Inner Lids";
        case EquipmentType::OUTER_LIDS: return "Outer Lids";
        case EquipmentType::QUEEN_EXCLUDERS: return "Queen Excluders";
        case EquipmentType::HIVE_STANDS: return "Hive Stands";
        case EquipmentType::INSULATION: return "Insulation";
        case EquipmentType::ENTRANCE_REDUCERS: return "Ent. Reducers";
        case EquipmentType::FEEDERS: return "Feeders";
        default: return "Unknown";
    }
}

// Equipment plan record
struct EquipmentPlan {
    uint32_t yardNumber;            // Reference to yard
    uint32_t timestamp;             // Unix timestamp when plan was created/updated
    uint8_t quantities[static_cast<int>(EquipmentType::EQUIPMENT_COUNT)];  // Quantity for each equipment type

    // Default constructor
    EquipmentPlan() {
        reset();
    }

    // Reset to defaults
    void reset() {
        yardNumber = 0;
        timestamp = 0;
        for (int i = 0; i < static_cast<int>(EquipmentType::EQUIPMENT_COUNT); i++) {
            quantities[i] = 0;
        }
    }

    // Get quantity for equipment type
    uint8_t getQuantity(EquipmentType type) const {
        int idx = static_cast<int>(type);
        if (idx >= 0 && idx < static_cast<int>(EquipmentType::EQUIPMENT_COUNT)) {
            return quantities[idx];
        }
        return 0;
    }

    // Set quantity for equipment type
    void setQuantity(EquipmentType type, uint8_t qty) {
        int idx = static_cast<int>(type);
        if (idx >= 0 && idx < static_cast<int>(EquipmentType::EQUIPMENT_COUNT)) {
            quantities[idx] = qty;
        }
    }
};

// NVS packed equipment plan structure
struct __attribute__((packed)) EquipmentPlanNVS {
    uint32_t yardNumber;            // 4 bytes
    uint32_t timestamp;             // 4 bytes
    uint8_t quantities[static_cast<int>(EquipmentType::EQUIPMENT_COUNT)];  // 10 bytes
    uint16_t crc16;                 // 2 bytes

    // Convert from runtime record
    void fromRecord(const EquipmentPlan& rec) {
        yardNumber = rec.yardNumber;
        timestamp = rec.timestamp;
        for (int i = 0; i < static_cast<int>(EquipmentType::EQUIPMENT_COUNT); i++) {
            quantities[i] = rec.quantities[i];
        }
        updateCRC();
    }

    // Convert to runtime record
    void toRecord(EquipmentPlan& rec) const {
        rec.yardNumber = yardNumber;
        rec.timestamp = timestamp;
        for (int i = 0; i < static_cast<int>(EquipmentType::EQUIPMENT_COUNT); i++) {
            rec.quantities[i] = quantities[i];
        }
    }

    // Calculate and set CRC
    void updateCRC() {
        crc16 = YardRecordNVS::calculateCRC16(reinterpret_cast<const uint8_t*>(this),
                                              sizeof(EquipmentPlanNVS) - sizeof(crc16));
    }

    // Validate CRC
    bool validateCRC() const {
        uint16_t calculated = YardRecordNVS::calculateCRC16(
            reinterpret_cast<const uint8_t*>(this),
            sizeof(EquipmentPlanNVS) - sizeof(crc16));
        return calculated == crc16;
    }
};

} // namespace YARD_MANAGEMENT
