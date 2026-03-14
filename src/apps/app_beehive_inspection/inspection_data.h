/**
 * @file inspection_data.h
 * @brief Data structures for beehive inspection records
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include <stdint.h>
#include <cstddef>
#include <array>

namespace BEEHIVE_INSPECTION {

// NVS Storage constants
constexpr const char* NVS_NAMESPACE = "beehive_insp";
constexpr const char* NVS_KEY_PREFIX = "inspect_";
constexpr const char* NVS_KEY_NEXT_ID = "next_id";
constexpr std::size_t MAX_INSPECTION_RECORDS = 100;

// Screen 2: Queen Cell count options
enum class CellCount : uint8_t {
    NO = 0,           // No cells observed
    YES = 1,          // Present but not counted
    COUNT_1_5 = 2,    // 1-5 cells
    COUNT_5_10 = 3,   // 5-10 cells
    COUNT_10_PLUS = 4 // 10+ cells
};

// Screen 3: Super fill percentage
enum class FillPercentage : uint8_t {
    UNSET = 0,
    PERCENT_0 = 1,
    PERCENT_25 = 2,
    PERCENT_50 = 3,
    PERCENT_75 = 4,
    PERCENT_100 = 5
};

// Screen 4: Bee temperament
enum class Temperament : uint8_t {
    CALM = 0,
    ANGRY = 1,
    STICKY = 2,
    FESTOONING = 3,
    BEARDING = 4,
    WASHBOARDING = 5
};

// Screen 6: Treatment type
enum class Treatment : uint8_t {
    NONE = 0,
    OXALIC_ACID = 1,
    HOPGUARD = 2,
    FORMIC_ACID = 3,
    DRONE_FRAME_IN = 4,
    DRONE_FRAME_OUT = 5
};

// Screen 7: Pest flags (bitfield)
enum PestFlag : uint8_t {
    PEST_NONE = 0x00,
    PEST_WAX_MOTH = 0x01,
    PEST_HIVE_BEETLE = 0x02,
    PEST_AFB = 0x04,
    PEST_EFB = 0x08
};

// Helper functions for string conversion
inline const char* getCellCountString(CellCount count) {
    switch (count) {
        case CellCount::NO: return "No";
        case CellCount::YES: return "Yes";
        case CellCount::COUNT_1_5: return "1-5";
        case CellCount::COUNT_5_10: return "5-10";
        case CellCount::COUNT_10_PLUS: return "10+";
        default: return "No";
    }
}

inline const char* getFillPercentageString(FillPercentage fill) {
    switch (fill) {
        case FillPercentage::UNSET: return "--";
        case FillPercentage::PERCENT_0: return "0%";
        case FillPercentage::PERCENT_25: return "25%";
        case FillPercentage::PERCENT_50: return "50%";
        case FillPercentage::PERCENT_75: return "75%";
        case FillPercentage::PERCENT_100: return "100%";
        default: return "--";
    }
}

inline const char* getTemperamentString(Temperament temp) {
    switch (temp) {
        case Temperament::CALM: return "Calm";
        case Temperament::ANGRY: return "Angry";
        case Temperament::STICKY: return "Sticky";
        case Temperament::FESTOONING: return "Festooning";
        case Temperament::BEARDING: return "Bearding";
        case Temperament::WASHBOARDING: return "Washboarding";
        default: return "Calm";
    }
}

inline const char* getTreatmentString(Treatment treatment) {
    switch (treatment) {
        case Treatment::NONE: return "None";
        case Treatment::OXALIC_ACID: return "Oxalic Acid";
        case Treatment::HOPGUARD: return "Hopguard";
        case Treatment::FORMIC_ACID: return "Formic Acid";
        case Treatment::DRONE_FRAME_IN: return "Drone Frame In";
        case Treatment::DRONE_FRAME_OUT: return "Drone Frame Out";
        default: return "None";
    }
}

// Yard/Hive context for inspection display
struct InspectionContext {
    uint32_t yardNumber = 0;
    uint32_t hiveNumber = 0;
    char yardNickname[21] = {0};  // Max 20 chars + null

    void reset() {
        yardNumber = 0;
        hiveNumber = 0;
        yardNickname[0] = '\0';
    }

    bool isValid() const {
        return hiveNumber != 0;
    }
};

// Main inspection data record
struct InspectionRecord {
    // Screen 1: Queen Right
    bool queenRight = true;

    // Screen 2: Queen Cells
    CellCount supersedureCells = CellCount::NO;
    CellCount swarmCells = CellCount::NO;

    // Screen 3: Supers (0-5 supers, each with fill %)
    uint8_t superCount = 0;
    std::array<FillPercentage, 5> superFill = {
        FillPercentage::UNSET,
        FillPercentage::UNSET,
        FillPercentage::UNSET,
        FillPercentage::UNSET,
        FillPercentage::UNSET
    };

    // Screen 4: Temperament
    Temperament temperament = Temperament::CALM;

    // Screen 5: Brood Size (0-40 frames)
    uint8_t broodFrames = 0;

    // Screen 6: Treatment
    Treatment treatment = Treatment::NONE;

    // Screen 7: Pests (bitfield)
    uint8_t pests = PEST_NONE;

    // Helper methods for pest flags
    inline bool hasPest(PestFlag pest) const {
        return (pests & pest) != 0;
    }

    inline void setPest(PestFlag pest, bool value) {
        if (value) {
            pests |= pest;
        } else {
            pests &= ~pest;
        }
    }

    inline void togglePest(PestFlag pest) {
        pests ^= pest;
    }

    // Reset to defaults
    void reset() {
        recordId = 0;
        hiveId = 0;
        timestamp = 0;
        queenRight = true;
        supersedureCells = CellCount::NO;
        swarmCells = CellCount::NO;
        superCount = 0;
        for (int i = 0; i < 5; i++) {
            superFill[i] = FillPercentage::UNSET;
        }
        temperament = Temperament::CALM;
        broodFrames = 0;
        treatment = Treatment::NONE;
        pests = PEST_NONE;
        isComplete = false;
    }

    // Metadata fields for NVS storage
    uint32_t recordId = 0;
    uint32_t hiveId = 0;
    uint32_t timestamp = 0;
    bool isComplete = false;
};

// Bit-packed structure for efficient NVS storage (~27 bytes)
struct __attribute__((packed)) InspectionRecordNVS {
    uint32_t recordId;           // 4 bytes
    uint32_t hiveId;             // 4 bytes
    uint32_t timestamp;          // 4 bytes

    // Bit-packed fields (4 bytes total)
    uint32_t queenRight : 2;          // 0=unknown, 1=yes, 2=no
    uint32_t supersedureCells : 3;    // 0-4 (No/Yes/1-5/5-10/10+)
    uint32_t swarmCells : 3;          // 0-4
    uint32_t superCount : 3;          // 0-5
    uint32_t temperament : 3;         // 0-5
    uint32_t broodFrames : 6;         // 0-40
    uint32_t treatment : 3;           // 0-5
    uint32_t pestFlags : 4;           // 4 bits for 4 pests
    uint32_t isComplete : 1;          // Completion flag
    uint32_t reserved : 4;            // Future use

    // Super fill levels (5 bytes)
    uint8_t superFills[5];            // Each: 0-5 for FillPercentage enum

    uint16_t crc16;                   // CRC checksum (2 bytes)

    // Convert from InspectionRecord
    void fromRecord(const InspectionRecord& rec);

    // Convert to InspectionRecord
    void toRecord(InspectionRecord& rec) const;

    // Calculate and set CRC
    void updateCRC();

    // Validate CRC
    bool validateCRC() const;
};

// CRC16 calculation (CCITT polynomial)
inline uint16_t calculateCRC16(const uint8_t* data, std::size_t length) {
    uint16_t crc = 0xFFFF;
    for (std::size_t i = 0; i < length; i++) {
        crc ^= (uint16_t)data[i] << 8;
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

} // namespace BEEHIVE_INSPECTION
