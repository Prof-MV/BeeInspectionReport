/**
 * @file yard_storage.h
 * @brief NVS storage interface for yard and hive management
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "yard_data.h"
#include <vector>

namespace YARD_MANAGEMENT {

// Initialize NVS storage for yard management
bool initYardManagementNVS();

// ==================== Yard Operations ====================

// Save a yard record to NVS
bool saveYard(const YardRecord& yard);

// Load a yard record from NVS
bool loadYard(uint32_t yardNumber, YardRecord& yard);

// Close a yard (change status to 'C')
bool closeYard(uint32_t yardNumber);

// Delete a yard record from NVS (use with caution)
bool deleteYard(uint32_t yardNumber);

// Get all yard numbers (active and closed)
std::vector<uint32_t> getAllYardNumbers();

// Get only active yard numbers
std::vector<uint32_t> getActiveYardNumbers();

// Check if yard number is unique (doesn't exist in storage)
bool isYardNumberUnique(uint32_t yardNumber);

// Get count of yards
size_t getYardCount();
size_t getActiveYardCount();

// ==================== Hive Operations ====================

// Save a hive record to NVS
bool saveHive(const HiveRecord& hive);

// Load a hive record from NVS
bool loadHive(uint32_t hiveNumber, HiveRecord& hive);

// Close a hive (change status and clear RFID)
bool closeHive(uint32_t hiveNumber, char reason);

// Delete a hive record from NVS (use with caution)
bool deleteHive(uint32_t hiveNumber);

// Get all hive numbers (active and closed)
std::vector<uint32_t> getAllHiveNumbers();

// Get all hive numbers in a specific yard
std::vector<uint32_t> getHivesInYard(uint32_t yardNumber);

// Get only active hive numbers in a specific yard
std::vector<uint32_t> getActiveHivesInYard(uint32_t yardNumber);

// Check if hive number is unique system-wide
bool isHiveNumberUnique(uint32_t hiveNumber);

// Get count of hives
size_t getHiveCount();
size_t getActiveHiveCount();
size_t getActiveHiveCountInYard(uint32_t yardNumber);

// ==================== RFID Operations ====================

// Check if RFID tag is unique (across yards and/or hives)
bool isRfidUnique(const char* rfidTag, bool checkYards = true, bool checkHives = true);

// Find yard by RFID tag (returns 0 if not found)
uint32_t findYardByRfid(const char* rfidTag);

// Find hive by RFID tag (returns 0 if not found)
uint32_t findHiveByRfid(const char* rfidTag);

// ==================== Statistics ====================

// Calculate yard summary statistics (requires inspection data integration)
YardSummaryStats calculateYardStats(uint32_t yardNumber);

// Update active hive count for a yard
bool updateYardHiveCount(uint32_t yardNumber);

// ==================== Maintenance ====================

// Clear all yard and hive data (factory reset)
bool clearAllYardManagementData();

// ==================== Inspection Integration ====================

// Store selected hive for inspection app to read
bool storeSelectedHiveForInspection(uint32_t hiveNumber);

// Get selected hive for inspection (returns 0 if none selected)
uint32_t getSelectedHiveForInspection();

// Clear selected hive after inspection starts
bool clearSelectedHiveForInspection();

} // namespace YARD_MANAGEMENT
