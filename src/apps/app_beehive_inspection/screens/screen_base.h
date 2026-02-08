/**
 * @file screen_base.h
 * @brief Base class for all inspection screens
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "../inspection_data.h"
#include "../../../hal/hal.h"
#include <LovyanGFX.hpp>

namespace BEEHIVE_INSPECTION {

// Screen indices
constexpr int SCREEN_COUNT = 7;
constexpr int SCREEN_QUEEN_RIGHT = 0;
constexpr int SCREEN_QUEEN_CELLS = 1;
constexpr int SCREEN_SUPERS = 2;
constexpr int SCREEN_TEMPERAMENT = 3;
constexpr int SCREEN_BROOD_SIZE = 4;
constexpr int SCREEN_TREATMENT = 5;
constexpr int SCREEN_PESTS = 6;

// Colors (RGB565)
constexpr uint16_t COLOR_BACKGROUND = 0x1082;     // Dark blue
constexpr uint16_t COLOR_TEXT_PRIMARY = 0xFFFF;   // White
constexpr uint16_t COLOR_TEXT_SECONDARY = 0x8410; // Gray
constexpr uint16_t COLOR_SELECTED = 0xFD20;       // Orange (M5Stack)
constexpr uint16_t COLOR_SUCCESS = 0x07E0;        // Green
constexpr uint16_t COLOR_ERROR = 0xF800;          // Red
constexpr uint16_t COLOR_WARNING = 0xFFE0;        // Yellow

// Base class for all inspection screens
class ScreenBase {
protected:
    HAL::HAL* _hal;
    LGFX_Sprite* _canvas;
    InspectionRecord* _data;
    uint8_t _screenIndex;
    uint32_t _themeColor;

    // Common drawing helpers
    void drawBackground();
    void drawHeader(const char* title);
    void drawProgressIndicator();
    void drawLargeValue(const char* value, int y = 120);
    void drawOption(const char* text, int y, bool selected, bool checked = false);
    void drawCheckbox(int x, int y, bool checked, bool focused);
    void pushToDisplay();

    // Buzzer feedback
    void buzzConfirm();
    void buzzNavigate();
    void buzzError();

public:
    ScreenBase(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, uint8_t screenIndex);
    virtual ~ScreenBase() = default;

    // Screen lifecycle
    virtual void onEnter() = 0;   // Called when screen becomes active
    virtual void onExit() = 0;    // Called before leaving screen
    virtual void render() = 0;    // Draw screen content

    // Input handlers - return true if input was consumed
    virtual bool onRotate(int direction) = 0;  // direction: -1 or +1
    virtual bool onConfirm() = 0;              // Single click - return true to advance
    virtual bool onBack() = 0;                 // Double click - return true if handled internally

    // Get screen index
    uint8_t getScreenIndex() const { return _screenIndex; }

    // Set theme color
    void setThemeColor(uint32_t color) { _themeColor = color; }
};

} // namespace BEEHIVE_INSPECTION
