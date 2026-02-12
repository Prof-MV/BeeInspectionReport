/**
 * @file screen_close_yard.cpp
 * @brief Screen 4: Close Yard implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_close_yard.h"
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

ScreenCloseYard::ScreenCloseYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(CloseYardStep::SELECT)
    , _selectedIndex(0)
    , _scrollOffset(0)
    , _yesSelected(false)
    , _yardToClose(0)
{
    memset(_yardName, 0, sizeof(_yardName));
}

ScreenCloseYard::~ScreenCloseYard() {
    freeItemStrings();
}

void ScreenCloseYard::freeItemStrings() {
    for (char* str : _itemStrings) {
        delete[] str;
    }
    _itemStrings.clear();
    _displayItems.clear();
}

void ScreenCloseYard::loadYardList() {
    freeItemStrings();
    _yardNumbers.clear();

    // Get active yards
    _yardNumbers = getActiveYardNumbers();

    // Create display strings
    for (uint32_t yardNum : _yardNumbers) {
        YardRecord yard;
        if (loadYard(yardNum, yard)) {
            char* str = new char[32];
            snprintf(str, 32, "%06lu - %s", static_cast<unsigned long>(yardNum), yard.nickname);
            _itemStrings.push_back(str);
            _displayItems.push_back(str);
        }
    }
}

void ScreenCloseYard::onEnter() {
    _currentStep = CloseYardStep::SELECT;
    _selectedIndex = 0;
    _scrollOffset = 0;
    _yesSelected = false;
    loadYardList();
    render();
}

void ScreenCloseYard::onExit() {
    // Nothing to save
}

void ScreenCloseYard::render() {
    drawBackground();

    switch (_currentStep) {
        case CloseYardStep::SELECT:
            renderSelectStep();
            break;
        case CloseYardStep::CONFIRM:
            renderConfirmStep();
            break;
    }

    pushToDisplay();
}

void ScreenCloseYard::renderSelectStep() {
    drawHeader("CLOSE YARD");

    if (_yardNumbers.empty()) {
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No active yards", DISPLAY_CENTER_X, 120);

        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Dbl-click: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        drawSubheader("Select yard to close");
        drawScrollableList(_displayItems.data(), static_cast<int>(_displayItems.size()),
                          _selectedIndex, _scrollOffset, 75);
    }
}

void ScreenCloseYard::renderConfirmStep() {
    drawBackground();

    char title[32];
    snprintf(title, sizeof(title), "Close %06lu?", static_cast<unsigned long>(_yardToClose));

    drawConfirmDialog(title, _yardName, _yesSelected);
}

void ScreenCloseYard::onRotate(int direction) {
    switch (_currentStep) {
        case CloseYardStep::SELECT: {
            int itemCount = static_cast<int>(_displayItems.size());
            if (itemCount == 0) return;

            _selectedIndex += direction;

            if (_selectedIndex < 0) {
                _selectedIndex = 0;
            } else if (_selectedIndex >= itemCount) {
                _selectedIndex = itemCount - 1;
            }

            // Adjust scroll
            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            } else if (_selectedIndex >= _scrollOffset + MAX_VISIBLE_ITEMS) {
                _scrollOffset = _selectedIndex - MAX_VISIBLE_ITEMS + 1;
            }
            break;
        }

        case CloseYardStep::CONFIRM:
            _yesSelected = !_yesSelected;
            break;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenCloseYard::onConfirm() {
    NavigationResult result;

    switch (_currentStep) {
        case CloseYardStep::SELECT:
            if (!_yardNumbers.empty()) {
                _yardToClose = _yardNumbers[_selectedIndex];

                // Get yard name for confirmation
                YardRecord yard;
                if (loadYard(_yardToClose, yard)) {
                    strncpy(_yardName, yard.nickname, MAX_NICKNAME_LENGTH);
                }

                _currentStep = CloseYardStep::CONFIRM;
                _yesSelected = false;  // Default to No for safety
                buzzNavigate();
                render();
            }
            break;

        case CloseYardStep::CONFIRM:
            if (_yesSelected) {
                // Close the yard
                if (closeYard(_yardToClose)) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_LIST;
                } else {
                    buzzError();
                    // Stay on confirm, user can retry
                }
            } else {
                // Cancelled
                result.nextScreen = ScreenType::YARD_LIST;
            }
            break;
    }

    return result;
}

NavigationResult ScreenCloseYard::onBack() {
    NavigationResult result;

    switch (_currentStep) {
        case CloseYardStep::SELECT:
            result.nextScreen = ScreenType::MGMT_MENU;
            break;

        case CloseYardStep::CONFIRM:
            _currentStep = CloseYardStep::SELECT;
            render();
            break;
    }

    return result;
}

} // namespace YARD_MANAGEMENT
