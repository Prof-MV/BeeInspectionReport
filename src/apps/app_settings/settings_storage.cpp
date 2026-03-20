/**
 * @file settings_storage.cpp
 * @brief Shared settings storage implementation
 * @version 1.0
 * @date 2026-03-20
 */
#include "settings_storage.h"
#include <nvs_flash.h>
#include <nvs.h>
#include <cstdio>

namespace SETTINGS {

// Cached timezone value to avoid frequent NVS reads
static int8_t cachedTimezone = DEFAULT_TIMEZONE;
static bool cacheInitialized = false;
static bool nvsInitialized = false;

// Ensure NVS is initialized
static void ensureNvsInit()
{
    if (nvsInitialized) {
        return;
    }

    esp_err_t err = nvs_flash_init();
    if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
        // NVS partition was truncated and needs to be erased
        printf("[Settings] NVS needs erase, erasing...\n");
        nvs_flash_erase();
        err = nvs_flash_init();
    }

    if (err == ESP_OK) {
        nvsInitialized = true;
        printf("[Settings] NVS initialized\n");
    } else {
        printf("[Settings] ERROR: NVS init failed, err=%d\n", err);
    }
}

int8_t getTimezoneOffset()
{
    if (!cacheInitialized) {
        ensureNvsInit();

        nvs_handle_t handle;
        esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READONLY, &handle);
        if (err == ESP_OK) {
            nvs_get_i8(handle, NVS_KEY_TIMEZONE, &cachedTimezone);
            nvs_close(handle);
        }
        cacheInitialized = true;
    }
    return cachedTimezone;
}

void setTimezoneOffset(int8_t offset)
{
    ensureNvsInit();

    nvs_handle_t handle;
    esp_err_t err = nvs_open(NVS_NAMESPACE, NVS_READWRITE, &handle);
    if (err == ESP_OK) {
        nvs_set_i8(handle, NVS_KEY_TIMEZONE, offset);
        nvs_commit(handle);
        nvs_close(handle);

        // Update cache
        cachedTimezone = offset;
        cacheInitialized = true;

        printf("[Settings] Timezone saved: GMT%+d\n", offset);
    } else {
        printf("[Settings] ERROR: Failed to save timezone, err=%d\n", err);
    }
}

} // namespace SETTINGS
