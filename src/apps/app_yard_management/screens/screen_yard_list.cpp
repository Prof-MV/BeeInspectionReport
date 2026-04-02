/**
 * @file screen_yard_list.cpp
 * @brief Screen 1: Yard List implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_yard_list.h"
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

ScreenYardList::ScreenYardList(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _selectedIndex(0)
    , _scrollOffset(0)
    , _hasManagement(true)
{
}

ScreenYardList::~ScreenYardList() {
    freeItemStrings();
}

void ScreenYardList::freeItemStrings() {
    for (char* str : _itemStrings) {
        delete[] str;
    }
    _itemStrings.clear();
    _displayItems.clear();
}

void ScreenYardList::loadYardList() {
    freeItemStrings();
    _yardNumbers.clear();

    // Get active yards
    _yardNumbers = getActiveYardNumbers();

    // Create display strings
    for (uint32_t yardNum : _yardNumbers) {
        YardRecord yard;
        if (loadYard(yardNum, yard)) {
            char* str = new char[32];
            snprintf(str, 32, "%s", yard.nickname);
            _itemStrings.push_back(str);
            _displayItems.push_back(str);
        }
    }

    // Add Management option at the end
    _hasManagement = true;
    char* mgmtStr = new char[16];
    strcpy(mgmtStr, "Management");
    _itemStrings.push_back(mgmtStr);
    _displayItems.push_back(mgmtStr);
}

void ScreenYardList::onEnter() {
    loadYardList();
    _selectedIndex = 0;
    _scrollOffset = 0;
    render();
}

void ScreenYardList::onExit() {
    // Nothing to save
}

void ScreenYardList::render() {
    drawBackground();

    if (_yardNumbers.empty()) {
        // Empty state
        drawHeader("YARD MGMT");

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No Yards Yet", DISPLAY_CENTER_X, 100);

        // Just show Management option
        drawListItem("Management", 150, true, true);
    } else {
        drawHeader("SELECT YARD");

        // Draw scrollable list
        drawScrollableList(_displayItems.data(), static_cast<int>(_displayItems.size()),
                          _selectedIndex, _scrollOffset, 70);
    }

    pushToDisplay();
}

void ScreenYardList::onRotate(int direction) {
    int itemCount = static_cast<int>(_displayItems.size());
    if (itemCount == 0) return;

    // Update selection
    _selectedIndex += direction;

    // Clamp to valid range
    if (_selectedIndex < 0) {
        _selectedIndex = 0;
    } else if (_selectedIndex >= itemCount) {
        _selectedIndex = itemCount - 1;
    }

    // Adjust scroll offset
    if (_selectedIndex < _scrollOffset) {
        _scrollOffset = _selectedIndex;
    } else if (_selectedIndex >= _scrollOffset + MAX_VISIBLE_ITEMS) {
        _scrollOffset = _selectedIndex - MAX_VISIBLE_ITEMS + 1;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenYardList::onConfirm() {
    NavigationResult result;

    if (_displayItems.empty()) {
        // No items, go to management
        result.nextScreen = ScreenType::MGMT_MENU;
        buzzConfirm();
        return result;
    }

    // Check if Management is selected
    bool isManagementSelected = (_selectedIndex == static_cast<int>(_yardNumbers.size()));

    if (isManagementSelected) {
        result.nextScreen = ScreenType::MGMT_MENU;
    } else {
        // Store selected yard number in context
        _context->selectedYardNumber = _yardNumbers[_selectedIndex];
        result.nextScreen = ScreenType::YARD_SUMMARY;
    }

    buzzConfirm();
    return result;
}

NavigationResult ScreenYardList::onBack() {
    NavigationResult result;
    result.exitApp = true;  // Exit to launcher
    return result;
}

} // namespace YARD_MANAGEMENT
