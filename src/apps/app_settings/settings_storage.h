/**
 * @file settings_storage.h
 * @brief Shared settings storage access
 * @version 1.0
 * @date 2026-03-20
 */
#pragma once
#include <cstdint>

namespace SETTINGS {

// NVS keys
constexpr const char* NVS_NAMESPACE = "settings";
constexpr const char* NVS_KEY_TIMEZONE = "tz_offset";

// Default values
constexpr int8_t DEFAULT_TIMEZONE = -4;  // GMT-4

/**
 * @brief Get the saved timezone offset from NVS
 * @return Timezone offset in hours from UTC (can be negative)
 */
int8_t getTimezoneOffset();

/**
 * @brief Save timezone offset to NVS
 * @param offset Timezone offset in hours from UTC
 */
void setTimezoneOffset(int8_t offset);

} // namespace SETTINGS
