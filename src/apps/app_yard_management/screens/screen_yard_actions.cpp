/**
 * @file screen_yard_actions.cpp
 * @brief Screen: Yard Actions Menu implementation
 * @version 1.0
 * @date 2026-03-20
 */
#include "screen_yard_actions.h"
#include <cstdio>

namespace YARD_MANAGEMENT {

constexpr const char* ScreenYardActions::OPTIONS[OPTION_COUNT];

ScreenYardActions::ScreenYardActions(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _selectedIndex(0)
{
}

void ScreenYardActions::onEnter() {
    _selectedIndex = 0;
    render();
}

void ScreenYardActions::onExit() {
    // Nothing to save
}

void ScreenYardActions::render() {
    drawBackground();

    // Header with yard number
    char header[32];
    snprintf(header, sizeof(header), "YARD %06lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawHeader(header);
    drawSubheader("Select Action");

    drawMenuOptions(OPTIONS, OPTION_COUNT, _selectedIndex, 100);

    pushToDisplay();
}

void ScreenYardActions::onRotate(int direction) {
    _selectedIndex += direction;

    if (_selectedIndex < 0) {
        _selectedIndex = OPTION_COUNT - 1;
    } else if (_selectedIndex >= OPTION_COUNT) {
        _selectedIndex = 0;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenYardActions::onConfirm() {
    NavigationResult result;

    switch (_selectedIndex) {
        case 0:  // Hive Mgmt
            result.nextScreen = ScreenType::HIVE_MGMT;
            break;
        case 1:  // Equipment Plan
            result.nextScreen = ScreenType::EQUIPMENT_PLAN;
            break;
    }

    buzzConfirm();
    return result;
}

NavigationResult ScreenYardActions::onBack() {
    NavigationResult result;
    result.nextScreen = ScreenType::YARD_SUMMARY;
    return result;
}

} // namespace YARD_MANAGEMENT
