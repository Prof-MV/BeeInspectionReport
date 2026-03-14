/**
 * @file screen_supers.cpp
 * @brief Screen 3: Supers (count selection and optional fill percentage)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_supers.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

const char* ScreenSupers::getFillName(int index) {
    switch (index) {
        case 0: return "0%";
        case 1: return "25%";
        case 2: return "50%";
        case 3: return "75%";
        case 4: return "100%";
        default: return "--";
    }
}

ScreenSupers::ScreenSupers(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, SCREEN_SUPERS)
    , _state(SupersState::COUNT_SELECT)
    , _count(0)
    , _currentSuper(0)
    , _fillIndex(0)
    , _listIndex(0)
{
    for (int i = 0; i < MAX_SUPERS; i++) {
        _fills[i] = FillPercentage::UNSET;
    }
}

void ScreenSupers::onEnter() {
    _count = _data->superCount;
    for (int i = 0; i < MAX_SUPERS; i++) {
        _fills[i] = _data->superFill[i];
    }
    _state = SupersState::COUNT_SELECT;
    render();
}

void ScreenSupers::onExit() {
    _data->superCount = _count;
    for (int i = 0; i < MAX_SUPERS; i++) {
        _data->superFill[i] = _fills[i];
    }
}

void ScreenSupers::render() {
    switch (_state) {
        case SupersState::COUNT_SELECT:
            renderCountSelect();
            break;
        case SupersState::SUPER_LIST:
            renderSuperList();
            break;
        case SupersState::FILL_SELECT:
            renderFillSelect();
            break;
    }
}

void ScreenSupers::renderCountSelect() {
    drawBackground();
    drawHeader("SUPERS");

    // Draw large count
    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%d", _count);

    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setFont(&fonts::FreeSansBold24pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(valueStr, 120, 100);

    // Draw "supers" label
    _canvas->setFont(&fonts::FreeSans12pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->drawString("supers", 120, 145);

    // Draw hint
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->drawString("Click to set fill %", 120, 175);

    drawProgressIndicator();
    pushToDisplay();
}

void ScreenSupers::renderSuperList() {
    drawBackground();
    drawHeader("SET FILL %");

    int yStart = 65;
    int ySpacing = 32;
    int itemCount = _count + 1;  // Supers + "Next" button

    // Calculate display range
    int displayStart = _listIndex - 1;
    if (displayStart < 0) displayStart = 0;
    if (displayStart > itemCount - 3 && itemCount > 3) displayStart = itemCount - 3;

    int displayCount = (itemCount < 4) ? itemCount : 4;

    for (int i = 0; i < displayCount; i++) {
        int itemIndex = displayStart + i;
        if (itemIndex >= itemCount) break;

        int y = yStart + i * ySpacing;
        bool isSelected = (itemIndex == _listIndex);

        if (itemIndex < _count) {
            // Super item with fill value
            char displayText[32];
            FillPercentage fill = _fills[itemIndex];
            const char* fillStr = (fill == FillPercentage::UNSET) ? "--" : getFillName(static_cast<int>(fill) - 1);
            snprintf(displayText, sizeof(displayText), "Super %d: %s", itemIndex + 1, fillStr);
            drawOption(displayText, y, isSelected, false);
        } else {
            // "Next" button
            drawOption("Next >", y, isSelected, false);
        }
    }

    drawProgressIndicator();
    pushToDisplay();
}

void ScreenSupers::renderFillSelect() {
    drawBackground();

    char headerStr[32];
    snprintf(headerStr, sizeof(headerStr), "SUPER %d FILL", _currentSuper + 1);
    drawHeader(headerStr);

    int yStart = 70;
    int ySpacing = 32;

    for (int i = 0; i < FILL_OPTIONS; i++) {
        int y = yStart + i * ySpacing;
        bool isSelected = (i == _fillIndex);
        drawOption(getFillName(i), y, isSelected, false);
    }

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenSupers::onRotate(int direction) {
    switch (_state) {
        case SupersState::COUNT_SELECT:
            _count += direction;
            if (_count < 0) _count = MAX_SUPERS;
            if (_count > MAX_SUPERS) _count = 0;
            break;

        case SupersState::SUPER_LIST:
            _listIndex += direction;
            if (_listIndex < 0) _listIndex = _count;  // Wrap to "Next"
            if (_listIndex > _count) _listIndex = 0;
            break;

        case SupersState::FILL_SELECT:
            _fillIndex += direction;
            if (_fillIndex < 0) _fillIndex = FILL_OPTIONS - 1;
            if (_fillIndex >= FILL_OPTIONS) _fillIndex = 0;
            break;
    }

    buzzNavigate();
    render();
    return true;
}

bool ScreenSupers::onConfirm() {
    switch (_state) {
        case SupersState::COUNT_SELECT:
            if (_count == 0) {
                // No supers - advance to next screen
                buzzConfirm();
                return true;
            } else {
                // Go to super list
                _state = SupersState::SUPER_LIST;
                _listIndex = 0;
                buzzConfirm();
                render();
                return false;
            }

        case SupersState::SUPER_LIST:
            if (_listIndex == _count) {
                // "Next" selected - advance to next screen
                buzzConfirm();
                return true;
            } else {
                // Edit this super's fill
                _currentSuper = _listIndex;
                FillPercentage currentFill = _fills[_currentSuper];
                _fillIndex = (currentFill == FillPercentage::UNSET) ? 0 : static_cast<int>(currentFill) - 1;
                _state = SupersState::FILL_SELECT;
                buzzConfirm();
                render();
                return false;
            }

        case SupersState::FILL_SELECT:
            // Save fill and return to list
            _fills[_currentSuper] = static_cast<FillPercentage>(_fillIndex + 1);  // +1 because UNSET=0
            _state = SupersState::SUPER_LIST;
            buzzConfirm();
            render();
            return false;
    }

    return false;
}

bool ScreenSupers::onBack() {
    switch (_state) {
        case SupersState::COUNT_SELECT:
            return false;  // Go to previous screen

        case SupersState::SUPER_LIST:
            _state = SupersState::COUNT_SELECT;
            render();
            return true;  // Handled internally

        case SupersState::FILL_SELECT:
            _state = SupersState::SUPER_LIST;
            render();
            return true;  // Handled internally
    }

    return false;
}

} // namespace BEEHIVE_INSPECTION
