/**
 * @file screen_base_ym.h
 * @brief Base class for all yard management screens
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "../yard_data.h"
#include "../../../hal/hal.h"
#include <LovyanGFX.hpp>
#include <cmath>

namespace YARD_MANAGEMENT {

// Screen types for navigation
enum class ScreenType {
    YARD_LIST,
    MGMT_MENU,
    CREATE_YARD,
    CLOSE_YARD,
    YARD_SUMMARY,
    HIVE_MGMT,
    ADD_HIVE,
    CLOSE_HIVE,
    NONE  // For back to launcher
};

// Screen indices
constexpr int SCREEN_COUNT = 8;

// Circular display constraints (same as beehive inspection)
constexpr int DISPLAY_WIDTH = 240;
constexpr int DISPLAY_HEIGHT = 240;
constexpr int DISPLAY_CENTER_X = 120;
constexpr int DISPLAY_CENTER_Y = 120;

// Effective visible circular area
constexpr int VISIBLE_DIAMETER = 226;
constexpr int VISIBLE_RADIUS = 113;

// Safe area for critical content
constexpr int SAFE_DIAMETER = 150;
constexpr int SAFE_RADIUS = 75;

// Edge buffer
constexpr int EDGE_BUFFER = 10;

// Usable content area
constexpr int CONTENT_TOP = 30;
constexpr int CONTENT_BOTTOM = 200;
constexpr int CONTENT_LEFT = 30;
constexpr int CONTENT_RIGHT = 210;

// Maximum content width
constexpr int MAX_CONTENT_WIDTH = SAFE_DIAMETER;

// Colors (RGB565) - consistent with beehive inspection
constexpr uint16_t COLOR_BACKGROUND = 0x1082;     // Dark blue
constexpr uint16_t COLOR_TEXT_PRIMARY = 0xFFFF;   // White
constexpr uint16_t COLOR_TEXT_SECONDARY = 0x8410; // Gray
constexpr uint16_t COLOR_SELECTED = 0xFD20;       // Orange (M5Stack)
constexpr uint16_t COLOR_SUCCESS = 0x07E0;        // Green
constexpr uint16_t COLOR_ERROR = 0xF800;          // Red
constexpr uint16_t COLOR_WARNING = 0xFFE0;        // Yellow
constexpr uint16_t COLOR_HIGHLIGHT_BG = 0x4208;   // Highlighted item background

// 6-digit input constants
constexpr int NUM_DIGITS = 6;
constexpr int DIGIT_WIDTH = 28;
constexpr int DIGIT_HEIGHT = 36;
constexpr int DIGIT_SPACING = 4;

// List display constants
constexpr int LIST_ITEM_HEIGHT = 32;
constexpr int MAX_VISIBLE_ITEMS = 4;

// App context structure for passing data between screens
struct AppContext {
    uint32_t selectedYardNumber = 0;
    uint32_t selectedHiveNumber = 0;
    uint32_t newYardNumber = MIN_YARD_NUMBER;
    uint32_t newHiveNumber = MIN_HIVE_NUMBER;
    char yardNickname[MAX_NICKNAME_LENGTH + 1] = {0};
    char rfidTag[RFID_TAG_LENGTH + 1] = {0};
    HiveOriginType originType = HiveOriginType::SWARM;
    uint32_t originHiveNumber = 0;
    char closeReason = 0;  // 'S', 'D', 'O'
};

// Navigation result from screen
struct NavigationResult {
    ScreenType nextScreen = ScreenType::NONE;
    bool exitApp = false;
};

// Base class for all yard management screens
class ScreenBaseYM {
protected:
    HAL::HAL* _hal;
    LGFX_Sprite* _canvas;
    AppContext* _context;
    uint32_t _themeColor;

    // Common drawing helpers
    void drawBackground();
    void drawHeader(const char* title);
    void drawSubheader(const char* text);

    // 6-digit number input widget
    void drawSixDigitInput(uint32_t value, int highlightDigit, int y = 100);
    uint32_t adjustDigit(uint32_t value, int digitIndex, int direction);

    // List selection widget
    void drawListItem(const char* text, int y, bool selected, bool isManagement = false);
    void drawScrollableList(const char** items, int itemCount, int selectedIndex, int scrollOffset, int startY = 70);

    // Menu widget (2-3 options)
    void drawMenuOptions(const char* const* options, int optionCount, int selectedIndex, int startY = 90);

    // RFID scan widget
    void drawRfidScanPrompt(const char* message, bool scanning = true);
    void drawRfidSuccess(const char* tagId);
    void drawRfidError(const char* message);

    // Confirmation dialog
    void drawConfirmDialog(const char* title, const char* message, bool yesSelected);

    // Statistics display
    void drawStatLine(const char* label, const char* value, int y);
    void drawStatLine(const char* label, int value, int y);

    // Nickname input widget
    void drawNicknameInput(const char* current, int cursorPos, char selectedChar, int y = 100);
    char getNextChar(char current, int direction);

    // Push to display
    void pushToDisplay();

    // Buzzer feedback
    void buzzConfirm();
    void buzzNavigate();
    void buzzError();
    void buzzSuccess();

public:
    ScreenBaseYM(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    virtual ~ScreenBaseYM() = default;

    // Screen lifecycle
    virtual void onEnter() = 0;   // Called when screen becomes active
    virtual void onExit() = 0;    // Called before leaving screen
    virtual void render() = 0;    // Draw screen content

    // Input handlers
    virtual void onRotate(int direction) = 0;     // direction: -1 or +1
    virtual NavigationResult onConfirm() = 0;     // Single click
    virtual NavigationResult onBack() = 0;        // Double click

    // Set theme color
    void setThemeColor(uint32_t color) { _themeColor = color; }

    // Static helpers for circular display
    static inline bool isInVisibleArea(int x, int y) {
        int dx = x - DISPLAY_CENTER_X;
        int dy = y - DISPLAY_CENTER_Y;
        return (dx * dx + dy * dy) <= (VISIBLE_RADIUS * VISIBLE_RADIUS);
    }

    static inline int getMaxWidthAtY(int y) {
        int dy = y - DISPLAY_CENTER_Y;
        if (dy < -VISIBLE_RADIUS || dy > VISIBLE_RADIUS) return 0;
        int halfWidth = static_cast<int>(sqrt(static_cast<float>(VISIBLE_RADIUS * VISIBLE_RADIUS - dy * dy)));
        return halfWidth * 2 - EDGE_BUFFER * 2;
    }
};

} // namespace YARD_MANAGEMENT
