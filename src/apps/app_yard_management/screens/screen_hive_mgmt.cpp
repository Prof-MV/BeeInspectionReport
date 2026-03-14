/**
 * @file screen_hive_mgmt.cpp
 * @brief Screen 6: Hive Management Menu implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_hive_mgmt.h"
#include <cstdio>

namespace YARD_MANAGEMENT {

constexpr const char* ScreenHiveMgmt::OPTIONS[OPTION_COUNT];

ScreenHiveMgmt::ScreenHiveMgmt(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _selectedIndex(0)
{
}

void ScreenHiveMgmt::onEnter() {
    _selectedIndex = 0;
    render();
}

void ScreenHiveMgmt::onExit() {
    // Nothing to save
}

void ScreenHiveMgmt::render() {
    drawBackground();

    // Header with yard number
    char header[32];
    snprintf(header, sizeof(header), "YARD %06lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawHeader(header);
    drawSubheader("Hive Management");

    drawMenuOptions(OPTIONS, OPTION_COUNT, _selectedIndex, 100);

    pushToDisplay();
}

void ScreenHiveMgmt::onRotate(int direction) {
    _selectedIndex += direction;

    if (_selectedIndex < 0) {
        _selectedIndex = OPTION_COUNT - 1;
    } else if (_selectedIndex >= OPTION_COUNT) {
        _selectedIndex = 0;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenHiveMgmt::onConfirm() {
    NavigationResult result;

    switch (_selectedIndex) {
        case 0:  // Select Hive (for inspection)
            result.nextScreen = ScreenType::SELECT_HIVE;
            break;
        case 1:  // Add Hive
            result.nextScreen = ScreenType::ADD_HIVE;
            break;
        case 2:  // Close Hive
            result.nextScreen = ScreenType::CLOSE_HIVE;
            break;
    }

    buzzConfirm();
    return result;
}

NavigationResult ScreenHiveMgmt::onBack() {
    NavigationResult result;
    result.nextScreen = ScreenType::YARD_SUMMARY;
    return result;
}

} // namespace YARD_MANAGEMENT
