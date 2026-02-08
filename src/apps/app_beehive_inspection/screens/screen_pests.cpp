/**
 * @file screen_pests.cpp
 * @brief Screen 7: Pests (toggle checklist)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_pests.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

const char* ScreenPests::getPestName(int index) {
    switch (index) {
        case 0: return "Wax Moth";
        case 1: return "Hive Beetle";
        case 2: return "AFB";
        case 3: return "EFB";
        default: return "";
    }
}

PestFlag ScreenPests::getPestFlag(int index) {
    switch (index) {
        case 0: return PEST_WAX_MOTH;
        case 1: return PEST_HIVE_BEETLE;
        case 2: return PEST_AFB;
        case 3: return PEST_EFB;
        default: return PEST_NONE;
    }
}

ScreenPests::ScreenPests(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data)
    : ScreenBase(hal, canvas, data, SCREEN_PESTS)
    , _focusedIndex(0)
{
}

void ScreenPests::onEnter() {
    _focusedIndex = 0;
    render();
}

void ScreenPests::onExit() {
    // Data is already saved on toggle
}

void ScreenPests::render() {
    drawBackground();
    drawHeader("PESTS");

    int yStart = 60;
    int ySpacing = 32;

    // Draw pest checkboxes
    for (int i = 0; i < PEST_COUNT; i++) {
        int y = yStart + i * ySpacing;
        bool isFocused = (i == _focusedIndex);
        bool isChecked = _data->hasPest(getPestFlag(i));

        // Draw checkbox
        int checkboxX = 40;
        drawCheckbox(checkboxX, y - 12, isChecked, isFocused);

        // Draw label
        _canvas->setFont(&fonts::FreeSansBold12pt7b);
        _canvas->setTextDatum(textdatum_t::middle_left);

        // Use warning color for critical pests (AFB/EFB) if checked
        if (isChecked && (i == 2 || i == 3)) {
            _canvas->setTextColor(COLOR_ERROR);
        } else if (isFocused) {
            _canvas->setTextColor(_themeColor);
        } else {
            _canvas->setTextColor(isChecked ? COLOR_TEXT_PRIMARY : COLOR_TEXT_SECONDARY);
        }

        _canvas->drawString(getPestName(i), checkboxX + 35, y);
    }

    // Draw "Finish" button
    int finishY = yStart + PEST_COUNT * ySpacing + 10;
    bool finishFocused = (_focusedIndex == PEST_COUNT);
    drawOption("FINISH", finishY, finishFocused, false);

    // Draw warning if critical pest detected
    if (_data->hasPest(PEST_AFB) || _data->hasPest(PEST_EFB)) {
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("! CRITICAL PEST !", 120, 215);
    }

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenPests::onRotate(int direction) {
    _focusedIndex += direction;

    // Wrap around
    if (_focusedIndex < 0) _focusedIndex = ITEM_COUNT - 1;
    if (_focusedIndex >= ITEM_COUNT) _focusedIndex = 0;

    buzzNavigate();
    render();
    return true;
}

bool ScreenPests::onConfirm() {
    if (_focusedIndex < PEST_COUNT) {
        // Toggle pest
        PestFlag flag = getPestFlag(_focusedIndex);
        _data->togglePest(flag);

        // Special warning buzz for critical pests
        if (_data->hasPest(flag) && (flag == PEST_AFB || flag == PEST_EFB)) {
            buzzError();
        } else {
            buzzConfirm();
        }

        render();
        return false;  // Stay on screen
    } else {
        // "Finish" selected - complete inspection
        buzzConfirm();
        return true;  // Signal to save and exit
    }
}

bool ScreenPests::onBack() {
    return false;  // Go to previous screen
}

} // namespace BEEHIVE_INSPECTION
