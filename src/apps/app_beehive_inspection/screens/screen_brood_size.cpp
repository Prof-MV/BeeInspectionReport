/**
 * @file screen_brood_size.cpp
 * @brief Screen 5: Brood Size (0-40 frames counter)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_brood_size.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

ScreenBroodSize::ScreenBroodSize(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, SCREEN_BROOD_SIZE)
    , _frames(0)
{
}

void ScreenBroodSize::onEnter() {
    _frames = _data->broodFrames;
    render();
}

void ScreenBroodSize::onExit() {
    _data->broodFrames = _frames;
}

void ScreenBroodSize::render() {
    drawBackground();
    drawHeader("BROOD SIZE");

    // Draw frame count in input box
    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%d", _frames);

    int boxWidth = 80;
    int boxHeight = 50;
    int boxX = DISPLAY_CENTER_X - boxWidth / 2 - 15;  // Offset left for arrows
    int boxY = 85;

    // Draw the input box
    _canvas->fillRoundRect(boxX, boxY, boxWidth, boxHeight, 8, 0x2104);  // Dark background
    _canvas->drawRoundRect(boxX, boxY, boxWidth, boxHeight, 8, _themeColor);

    // Draw the number centered in box
    _canvas->setFont(&fonts::FreeSansBold24pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(valueStr, boxX + boxWidth / 2, boxY + boxHeight / 2);

    // Draw up/down arrows on the right side of the box
    int arrowX = boxX + boxWidth + 18;
    int arrowSize = 10;

    // Up arrow (top half of box)
    int upArrowY = boxY + boxHeight / 4;
    _canvas->fillTriangle(arrowX, upArrowY - arrowSize,
                          arrowX - arrowSize, upArrowY + arrowSize / 2,
                          arrowX + arrowSize, upArrowY + arrowSize / 2, _themeColor);

    // Down arrow (bottom half of box)
    int downArrowY = boxY + boxHeight * 3 / 4;
    _canvas->fillTriangle(arrowX, downArrowY + arrowSize,
                          arrowX - arrowSize, downArrowY - arrowSize / 2,
                          arrowX + arrowSize, downArrowY - arrowSize / 2, _themeColor);

    // Draw "frames" label
    _canvas->setFont(&fonts::FreeSans12pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString("frames", DISPLAY_CENTER_X, boxY + boxHeight + 25);

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenBroodSize::onRotate(int direction) {
    _frames += direction;

    // Clamp to 0-40 (no wrap-around per PRD)
    if (_frames < 0) _frames = 0;
    if (_frames > 40) _frames = 40;

    buzzNavigate();
    render();
    return true;
}

bool ScreenBroodSize::onConfirm() {
    _data->broodFrames = _frames;
    buzzConfirm();
    return true;  // Advance to next screen
}

bool ScreenBroodSize::onBack() {
    return false;  // Go to previous screen
}

} // namespace BEEHIVE_INSPECTION
