/**
 * @file inspection_data.h
 * @brief Data structures for beehive inspection records
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include <stdint.h>
#include <array>

namespace BEEHIVE_INSPECTION {

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
    }
};

} // namespace BEEHIVE_INSPECTION
