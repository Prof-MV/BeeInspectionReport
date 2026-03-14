/**
 * @file screen_base.cpp
 * @brief Base class implementation for all inspection screens
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_base.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

ScreenBase::ScreenBase(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context, uint8_t screenIndex)
    : _hal(hal)
    , _canvas(canvas)
    , _data(data)
    , _context(context)
    , _screenIndex(screenIndex)
    , _themeColor(COLOR_SELECTED)
{
}

void ScreenBase::drawBackground() {
    _canvas->fillScreen(COLOR_BACKGROUND);
}

void ScreenBase::drawHeader(const char* title) {
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextSize(1);
    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);
    // Position header within visible circular area (starts around y=30 for 226px visible diameter)
    _canvas->drawString(title, DISPLAY_CENTER_X, CONTENT_TOP);
}

void ScreenBase::drawSubheader(const char* text) {
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextSize(1);
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(text, DISPLAY_CENTER_X, CONTENT_TOP + 28);
}

void ScreenBase::drawProgressIndicator() {
    // Draw progress dots at bottom - positioned within visible circular area
    // At y=200, the visible width is narrower due to circular display
    int dotSpacing = 10;
    int startX = DISPLAY_CENTER_X - (SCREEN_COUNT * dotSpacing) / 2;
    int y = CONTENT_BOTTOM - 5;  // Stay within visible area

    for (int i = 0; i < SCREEN_COUNT; i++) {
        int x = startX + i * dotSpacing;
        if (i == _screenIndex) {
            _canvas->fillCircle(x, y, 4, COLOR_SELECTED);
        } else if (i < _screenIndex) {
            _canvas->fillCircle(x, y, 3, COLOR_SUCCESS);
        } else {
            _canvas->fillCircle(x, y, 3, COLOR_TEXT_SECONDARY);
        }
    }

    // Draw screen number above dots
    char numStr[8];
    snprintf(numStr, sizeof(numStr), "%d/%d", _screenIndex + 1, SCREEN_COUNT);
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString(numStr, DISPLAY_CENTER_X, y - 10);
}

void ScreenBase::drawLargeValue(const char* value, int y) {
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setFont(&fonts::FreeSansBold18pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(value, DISPLAY_CENTER_X, y);
}

void ScreenBase::drawOption(const char* text, int y, bool selected, bool checked) {
    // Use safe area width (150px) for option boxes to stay within circular display
    int boxWidth = MAX_CONTENT_WIDTH;
    int boxHeight = 36;
    int boxX = (DISPLAY_WIDTH - boxWidth) / 2;

    if (selected) {
        _canvas->fillRoundRect(boxX, y - boxHeight/2, boxWidth, boxHeight, 8, _themeColor);
        _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    } else {
        _canvas->drawRoundRect(boxX, y - boxHeight/2, boxWidth, boxHeight, 8, COLOR_TEXT_SECONDARY);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    }

    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);

    if (checked) {
        char buf[64];
        snprintf(buf, sizeof(buf), "[x] %s", text);
        _canvas->drawString(buf, DISPLAY_CENTER_X, y);
    } else {
        _canvas->drawString(text, DISPLAY_CENTER_X, y);
    }
}

void ScreenBase::drawCheckbox(int x, int y, bool checked, bool focused) {
    int size = 24;

    if (focused) {
        _canvas->drawRoundRect(x - 2, y - 2, size + 4, size + 4, 4, _themeColor);
    }

    if (checked) {
        _canvas->fillRoundRect(x, y, size, size, 4, COLOR_SUCCESS);
        // Draw checkmark
        _canvas->drawLine(x + 5, y + 12, x + 10, y + 18, COLOR_TEXT_PRIMARY);
        _canvas->drawLine(x + 10, y + 18, x + 19, y + 6, COLOR_TEXT_PRIMARY);
    } else {
        _canvas->drawRoundRect(x, y, size, size, 4, COLOR_TEXT_SECONDARY);
    }
}

void ScreenBase::pushToDisplay() {
    _canvas->pushSprite(0, 0);
}

void ScreenBase::buzzConfirm() {
    // Use consistent frequency to avoid LEDC clock source conflicts
    _hal->buzz.tone(2000, 30);
}

void ScreenBase::buzzNavigate() {
    // Use same frequency as confirm, shorter duration for different feel
    _hal->buzz.tone(2000, 15);
}

void ScreenBase::buzzError() {
    // Use same frequency, double beep pattern for error
    _hal->buzz.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(60));
    _hal->buzz.tone(2000, 50);
}

} // namespace BEEHIVE_INSPECTION
