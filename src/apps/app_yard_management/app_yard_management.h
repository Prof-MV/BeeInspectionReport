/**
 * @file app_yard_management.h
 * @brief Yard Management App - Main app class
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "../app.h"
#include "../../hal/hal.h"
#include "gui/gui_yard_management.h"
#include "yard_data.h"
#include "screens/screen_base_ym.h"

namespace MOONCAKE {
namespace USER_APP {

namespace YARD_MANAGEMENT_NS {
    struct Data_t {
        HAL::HAL* hal = nullptr;

        // Current screen state
        YARD_MANAGEMENT::ScreenType currentScreen = YARD_MANAGEMENT::ScreenType::YARD_LIST;

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

class YardManagement : public APP_BASE {
private:
    const char* _tag = "yard_mgmt";

    YARD_MANAGEMENT_NS::Data_t _data;
    GUI_YardManagement _gui;

    // Shared context between screens
    YARD_MANAGEMENT::AppContext _context;

    // Screen instances (created on demand)
    YARD_MANAGEMENT::ScreenBaseYM* _currentScreenPtr = nullptr;

    // Input handling
    void handleEncoder();
    void handleButton();

    // Screen navigation
    void transitionToScreen(YARD_MANAGEMENT::ScreenType newScreen);
    void saveAndExit();

    // Screen factory
    YARD_MANAGEMENT::ScreenBaseYM* createScreen(YARD_MANAGEMENT::ScreenType type);
    void destroyCurrentScreen();

    // Long press indicator
    void drawLongPressProgress(float progress);

public:
    YardManagement() = default;
    ~YardManagement() = default;

    GUI_Base* getGui() override { return &_gui; }

    void onSetup() override;
    void onCreate() override;
    void onRunning() override;
    void onDestroy() override;
};

} // namespace USER_APP
} // namespace MOONCAKE
