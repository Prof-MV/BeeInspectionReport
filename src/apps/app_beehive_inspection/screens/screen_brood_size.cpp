/**
 * @file screen_brood_size.cpp
 * @brief Screen 5: Brood Size (0-40 frames counter)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_brood_size.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

ScreenBroodSize::ScreenBroodSize(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data)
    : ScreenBase(hal, canvas, data, SCREEN_BROOD_SIZE)
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

    // Draw large frame count
    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%d", _frames);

    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setFont(&fonts::FreeSansBold24pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(valueStr, 120, 110);

    // Draw "frames" label
    _canvas->setFont(&fonts::FreeSans12pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->drawString("frames", 120, 155);

    // Draw range indicator
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->drawString("(0 - 40)", 120, 180);

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
