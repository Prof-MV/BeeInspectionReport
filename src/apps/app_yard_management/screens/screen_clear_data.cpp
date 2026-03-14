/**
 * @file screen_clear_data.cpp
 * @brief Screen for clearing all yard management data (debug/reset)
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_clear_data.h"
#include "../yard_storage.h"

namespace YARD_MANAGEMENT {

ScreenClearData::ScreenClearData(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _confirmSelected(false)  // Default to "No"
    , _clearing(false)
    , _cleared(false)
{
}

void ScreenClearData::onEnter() {
    _confirmSelected = false;  // Default to "No" for safety
    _clearing = false;
    _cleared = false;
    render();
}

void ScreenClearData::onExit() {
    // Nothing to save
}

void ScreenClearData::render() {
    drawBackground();
    drawHeader("CLEAR DATA");

    if (_cleared) {
        // Show success message
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextColor(COLOR_SUCCESS);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("Data Cleared!", DISPLAY_CENTER_X, 100);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->drawString("Click to continue", DISPLAY_CENTER_X, 150);
    } else if (_clearing) {
        // Show clearing in progress
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextColor(COLOR_WARNING);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("Clearing...", DISPLAY_CENTER_X, DISPLAY_CENTER_Y);
    } else {
        // Show confirmation dialog
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_WARNING);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("Delete ALL yards", DISPLAY_CENTER_X, 70);
        _canvas->drawString("and hives?", DISPLAY_CENTER_X, 90);

        _canvas->setTextColor(COLOR_ERROR);
        _canvas->drawString("This cannot be undone!", DISPLAY_CENTER_X, 115);

        // Draw Yes/No buttons
        int buttonY = 160;
        int buttonWidth = 70;
        int buttonHeight = 30;
        int spacing = 20;

        // No button (left)
        int noX = DISPLAY_CENTER_X - spacing/2 - buttonWidth;
        uint16_t noColor = !_confirmSelected ? COLOR_SELECTED : COLOR_TEXT_SECONDARY;
        _canvas->drawRoundRect(noX, buttonY, buttonWidth, buttonHeight, 5, noColor);
        _canvas->setTextColor(noColor);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No", noX + buttonWidth/2, buttonY + buttonHeight/2);

        // Yes button (right)
        int yesX = DISPLAY_CENTER_X + spacing/2;
        uint16_t yesColor = _confirmSelected ? COLOR_ERROR : COLOR_TEXT_SECONDARY;
        _canvas->drawRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 5, yesColor);
        _canvas->setTextColor(yesColor);
        _canvas->drawString("Yes", yesX + buttonWidth/2, buttonY + buttonHeight/2);
    }

    pushToDisplay();
}

void ScreenClearData::onRotate(int direction) {
    if (!_clearing && !_cleared) {
        _confirmSelected = !_confirmSelected;
        buzzNavigate();
        render();
    }
}

NavigationResult ScreenClearData::onConfirm() {
    NavigationResult result;

    if (_cleared) {
        // Return to yard list after clearing
        result.nextScreen = ScreenType::YARD_LIST;
        buzzConfirm();
    } else if (_clearing) {
        // Do nothing while clearing
    } else {
        if (_confirmSelected) {
            // User selected "Yes" - clear all data
            _clearing = true;
            render();

            // Perform the clear
            if (clearAllYardManagementData()) {
                _cleared = true;
                buzzSuccess();
            } else {
                // Clear failed - show error and go back
                buzzError();
                result.nextScreen = ScreenType::MGMT_MENU;
            }

            _clearing = false;
            render();
        } else {
            // User selected "No" - go back
            result.nextScreen = ScreenType::MGMT_MENU;
            buzzConfirm();
        }
    }

    return result;
}

NavigationResult ScreenClearData::onBack() {
    NavigationResult result;

    if (_cleared) {
        result.nextScreen = ScreenType::YARD_LIST;
    } else if (!_clearing) {
        result.nextScreen = ScreenType::MGMT_MENU;
    }

    return result;
}

} // namespace YARD_MANAGEMENT
