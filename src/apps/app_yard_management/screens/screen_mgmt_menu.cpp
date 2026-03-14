/**
 * @file screen_mgmt_menu.cpp
 * @brief Screen 2: Management Menu implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_mgmt_menu.h"

namespace YARD_MANAGEMENT {

constexpr const char* ScreenMgmtMenu::OPTIONS[OPTION_COUNT];

ScreenMgmtMenu::ScreenMgmtMenu(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _selectedIndex(0)
{
}

void ScreenMgmtMenu::onEnter() {
    _selectedIndex = 0;
    render();
}

void ScreenMgmtMenu::onExit() {
    // Nothing to save
}

void ScreenMgmtMenu::render() {
    drawBackground();
    drawHeader("YARD MGMT");

    drawMenuOptions(OPTIONS, OPTION_COUNT, _selectedIndex, 90);

    pushToDisplay();
}

void ScreenMgmtMenu::onRotate(int direction) {
    _selectedIndex += direction;

    if (_selectedIndex < 0) {
        _selectedIndex = OPTION_COUNT - 1;
    } else if (_selectedIndex >= OPTION_COUNT) {
        _selectedIndex = 0;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenMgmtMenu::onConfirm() {
    NavigationResult result;

    switch (_selectedIndex) {
        case 0:  // Create Yard
            result.nextScreen = ScreenType::CREATE_YARD;
            break;
        case 1:  // Close Yard
            result.nextScreen = ScreenType::CLOSE_YARD;
            break;
        case 2:  // Clear Data
            result.nextScreen = ScreenType::CLEAR_DATA;
            break;
    }

    buzzConfirm();
    return result;
}

NavigationResult ScreenMgmtMenu::onBack() {
    NavigationResult result;
    result.nextScreen = ScreenType::YARD_LIST;
    return result;
}

} // namespace YARD_MANAGEMENT
