/**
 * @file screen_temperament.cpp
 * @brief Screen 4: Temperament (list selection)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_temperament.h"

namespace BEEHIVE_INSPECTION {

const char* ScreenTemperament::getOptionName(int index) {
    switch (index) {
        case 0: return "Calm";
        case 1: return "Angry";
        case 2: return "Sticky";
        case 3: return "Festooning";
        case 4: return "Bearding";
        case 5: return "Washboarding";
        default: return "Calm";
    }
}

ScreenTemperament::ScreenTemperament(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, SCREEN_TEMPERAMENT)
    , _selectedIndex(0)
{
}

void ScreenTemperament::onEnter() {
    _selectedIndex = static_cast<int>(_data->temperament);
    render();
}

void ScreenTemperament::onExit() {
    _data->temperament = static_cast<Temperament>(_selectedIndex);
}

void ScreenTemperament::render() {
    drawBackground();
    drawHeader("TEMPERAMENT");

    // Show 3 options at a time, centered on selection
    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);

    // Calculate which items to show (show selected and neighbors)
    int displayStart = _selectedIndex - 1;
    if (displayStart < 0) displayStart = 0;
    if (displayStart > OPTION_COUNT - 3) displayStart = OPTION_COUNT - 3;

    int yStart = 85;
    int ySpacing = 45;

    for (int i = 0; i < 3; i++) {
        int optionIndex = displayStart + i;
        if (optionIndex >= 0 && optionIndex < OPTION_COUNT) {
            int y = yStart + i * ySpacing;
            bool isSelected = (optionIndex == _selectedIndex);
            drawOption(getOptionName(optionIndex), y, isSelected, false);
        }
    }

    // Draw scroll indicators if needed
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    if (displayStart > 0) {
        _canvas->setTextDatum(textdatum_t::top_center);
        _canvas->drawString("^", 120, 55);
    }
    if (displayStart < OPTION_COUNT - 3) {
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("v", 120, 195);
    }

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenTemperament::onRotate(int direction) {
    _selectedIndex += direction;

    // Wrap around
    if (_selectedIndex < 0) _selectedIndex = OPTION_COUNT - 1;
    if (_selectedIndex >= OPTION_COUNT) _selectedIndex = 0;

    buzzNavigate();
    render();
    return true;
}

bool ScreenTemperament::onConfirm() {
    _data->temperament = static_cast<Temperament>(_selectedIndex);
    buzzConfirm();
    return true;  // Advance to next screen
}

bool ScreenTemperament::onBack() {
    return false;  // Go to previous screen
}

} // namespace BEEHIVE_INSPECTION
