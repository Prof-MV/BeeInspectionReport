/**
 * @file screen_treatment.cpp
 * @brief Screen 6: Treatment (list selection)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_treatment.h"

namespace BEEHIVE_INSPECTION {

const char* ScreenTreatment::getOptionName(int index) {
    switch (index) {
        case 0: return "None";
        case 1: return "Oxalic Acid";
        case 2: return "Hopguard";
        case 3: return "Formic Acid";
        case 4: return "Drone Frame In";
        case 5: return "Drone Frame Out";
        default: return "None";
    }
}

ScreenTreatment::ScreenTreatment(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data)
    : ScreenBase(hal, canvas, data, SCREEN_TREATMENT)
    , _selectedIndex(0)
{
}

void ScreenTreatment::onEnter() {
    _selectedIndex = static_cast<int>(_data->treatment);
    render();
}

void ScreenTreatment::onExit() {
    _data->treatment = static_cast<Treatment>(_selectedIndex);
}

void ScreenTreatment::render() {
    drawBackground();
    drawHeader("TREATMENT");

    // Show 3 options at a time, centered on selection
    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);

    // Calculate which items to show
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

bool ScreenTreatment::onRotate(int direction) {
    _selectedIndex += direction;

    // Wrap around
    if (_selectedIndex < 0) _selectedIndex = OPTION_COUNT - 1;
    if (_selectedIndex >= OPTION_COUNT) _selectedIndex = 0;

    buzzNavigate();
    render();
    return true;
}

bool ScreenTreatment::onConfirm() {
    _data->treatment = static_cast<Treatment>(_selectedIndex);
    buzzConfirm();
    return true;  // Advance to next screen
}

bool ScreenTreatment::onBack() {
    return false;  // Go to previous screen
}

} // namespace BEEHIVE_INSPECTION
