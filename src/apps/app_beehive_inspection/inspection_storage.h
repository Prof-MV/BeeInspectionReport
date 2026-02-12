/**
 * @file inspection_storage.h
 * @brief NVS storage functions for beehive inspection records
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "inspection_data.h"
#include <vector>

namespace BEEHIVE_INSPECTION {

// NVS storage statistics
struct NVSStats {
    std::size_t usedEntries;
    std::size_t freeEntries;
    std::size_t totalBytes;
    std::size_t usedBytes;
};

// Initialize NVS storage (call once at startup)
bool initNVSStorage();

// Save inspection record to NVS
bool saveInspectionToNVS(const InspectionRecord& record);

// Load inspection record from NVS by record ID
bool loadInspectionFromNVS(uint32_t recordId, InspectionRecord& record);

// Get all stored inspection record IDs
std::vector<uint32_t> getAllInspectionIDs();

// Delete inspection record from NVS
bool deleteInspectionFromNVS(uint32_t recordId);

// Get next available record ID
uint32_t getNextRecordID();

// Get NVS storage statistics
bool getNVSStats(NVSStats& stats);

// Clear all inspection records (factory reset)
bool clearAllInspections();

// Get count of stored inspections
std::size_t getInspectionCount();

} // namespace BEEHIVE_INSPECTION
