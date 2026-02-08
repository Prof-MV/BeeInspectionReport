/**
 * @file app_beehive_inspection.h
 * @brief Beehive Inspection App - Main app class
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "../app.h"
#include "../../hal/hal.h"
#include "gui/gui_beehive_inspection.h"
#include "inspection_data.h"
#include "screens/screen_base.h"

namespace MOONCAKE {
namespace USER_APP {

namespace BEEHIVE_INSPECTION_NS {
    struct Data_t {
        HAL::HAL* hal = nullptr;

        // Screen management
        int currentScreenIndex = 0;

        // Button timing for click detection
        uint32_t buttonPressStart = 0;
        bool isLongPressHandled = false;

        // Double-click detection
        uint32_t lastReleaseTime = 0;
        bool waitingForDoubleClick = false;

        // Long press visual feedback
        bool showLongPressIndicator = false;
    };

    // Timing constants
    constexpr uint32_t LONG_PRESS_MS = 800;
    constexpr uint32_t DOUBLE_CLICK_WINDOW_MS = 300;
}

class BeehiveInspection : public APP_BASE {
private:
    const char* _tag = "beehive";

    BEEHIVE_INSPECTION_NS::Data_t _data;
    GUI_BeehiveInspection _gui;

    // Data model
    BEEHIVE_INSPECTION::InspectionRecord _record;

    // Screen instances
    BEEHIVE_INSPECTION::ScreenBase* _screens[BEEHIVE_INSPECTION::SCREEN_COUNT] = {nullptr};

    // Input handling
    void handleEncoder();
    void handleButton();

    // Screen navigation
    void goToNextScreen();
    void goToPreviousScreen();
    void saveAndExit();

    // Screen management
    void createScreens();
    void destroyScreens();

    // Long press indicator
    void drawLongPressProgress(float progress);

public:
    BeehiveInspection() = default;
    ~BeehiveInspection() = default;

    GUI_Base* getGui() override { return &_gui; }

    void onSetup() override;
    void onCreate() override;
    void onRunning() override;
    void onDestroy() override;
};

} // namespace USER_APP
} // namespace MOONCAKE
