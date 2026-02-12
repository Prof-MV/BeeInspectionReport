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
#include <cmath>

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

// Circular display constraints (240x240 square, but circular visible area)
constexpr int DISPLAY_WIDTH = 240;
constexpr int DISPLAY_HEIGHT = 240;
constexpr int DISPLAY_CENTER_X = 120;
constexpr int DISPLAY_CENTER_Y = 120;

// Effective visible circular area: ~226px diameter (94% of 240px)
constexpr int VISIBLE_DIAMETER = 226;
constexpr int VISIBLE_RADIUS = 113;

// Safe area for critical content: 150px diameter center
constexpr int SAFE_DIAMETER = 150;
constexpr int SAFE_RADIUS = 75;

// Edge buffer: minimum 10px from edge for visual comfort
constexpr int EDGE_BUFFER = 10;

// Usable content area (accounting for circular display)
constexpr int CONTENT_TOP = 30;      // Start content below header
constexpr int CONTENT_BOTTOM = 200;  // End content above progress indicator
constexpr int CONTENT_LEFT = 30;     // Left edge with buffer
constexpr int CONTENT_RIGHT = 210;   // Right edge with buffer

// Maximum content width within safe area
constexpr int MAX_CONTENT_WIDTH = SAFE_DIAMETER;

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

    // Static helper: Check if point is within visible circular area
    static inline bool isInVisibleArea(int x, int y) {
        int dx = x - DISPLAY_CENTER_X;
        int dy = y - DISPLAY_CENTER_Y;
        return (dx * dx + dy * dy) <= (VISIBLE_RADIUS * VISIBLE_RADIUS);
    }

    // Static helper: Check if point is within safe content area
    static inline bool isInSafeArea(int x, int y) {
        int dx = x - DISPLAY_CENTER_X;
        int dy = y - DISPLAY_CENTER_Y;
        return (dx * dx + dy * dy) <= (SAFE_RADIUS * SAFE_RADIUS);
    }

    // Static helper: Get maximum width at given Y position (for circular display)
    static inline int getMaxWidthAtY(int y) {
        int dy = y - DISPLAY_CENTER_Y;
        if (dy < -VISIBLE_RADIUS || dy > VISIBLE_RADIUS) return 0;
        // Calculate chord width at this y position
        int halfWidth = (int)sqrt((float)(VISIBLE_RADIUS * VISIBLE_RADIUS - dy * dy));
        return halfWidth * 2 - EDGE_BUFFER * 2;
    }
};

} // namespace BEEHIVE_INSPECTION
