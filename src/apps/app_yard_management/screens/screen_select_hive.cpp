/**
 * @file screen_select_hive.cpp
 * @brief Screen: Select Hive implementation
 * @version 1.0
 * @date 2026-03-13
 */
#include "screen_select_hive.h"
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

ScreenSelectHive::ScreenSelectHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _selectedIndex(0)
    , _scrollOffset(0)
{
}

ScreenSelectHive::~ScreenSelectHive() {
    freeItemStrings();
}

void ScreenSelectHive::freeItemStrings() {
    for (char* str : _itemStrings) {
        delete[] str;
    }
    _itemStrings.clear();
    _displayItems.clear();
}

void ScreenSelectHive::loadHiveList() {
    freeItemStrings();
    _hiveNumbers.clear();

    // Get active hives in this yard
    _hiveNumbers = getActiveHivesInYard(_context->selectedYardNumber);

    // Create display strings
    for (uint32_t hiveNum : _hiveNumbers) {
        char* str = new char[16];
        snprintf(str, 16, "Hive %lu", static_cast<unsigned long>(hiveNum));
        _itemStrings.push_back(str);
        _displayItems.push_back(str);
    }
}

void ScreenSelectHive::onEnter() {
    _selectedIndex = 0;
    _scrollOffset = 0;
    loadHiveList();
    render();
}

void ScreenSelectHive::onExit() {
    freeItemStrings();
}

void ScreenSelectHive::render() {
    drawBackground();

    // Header with yard number
    char header[32];
    snprintf(header, sizeof(header), "YARD %lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawHeader(header);
    drawSubheader("Select Hive for Inspection");

    if (_hiveNumbers.empty()) {
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No active hives", DISPLAY_CENTER_X, 120);
    } else {
        drawScrollableList(_displayItems.data(), static_cast<int>(_displayItems.size()),
                          _selectedIndex, _scrollOffset, 75);
    }

    // Navigation hint
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString("Click: Start Inspection", DISPLAY_CENTER_X, CONTENT_BOTTOM);

    pushToDisplay();
}

void ScreenSelectHive::onRotate(int direction) {
    int itemCount = static_cast<int>(_displayItems.size());
    if (itemCount == 0) return;

    _selectedIndex += direction;

    if (_selectedIndex < 0) {
        _selectedIndex = 0;
    } else if (_selectedIndex >= itemCount) {
        _selectedIndex = itemCount - 1;
    }

    // Update scroll offset
    if (_selectedIndex < _scrollOffset) {
        _scrollOffset = _selectedIndex;
    } else if (_selectedIndex >= _scrollOffset + MAX_VISIBLE_ITEMS) {
        _scrollOffset = _selectedIndex - MAX_VISIBLE_ITEMS + 1;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenSelectHive::onConfirm() {
    NavigationResult result;

    if (!_hiveNumbers.empty()) {
        // Store selected hive number
        _context->selectedHiveNumber = _hiveNumbers[_selectedIndex];

        // Store in NVS for inspection app to read
        storeSelectedHiveForInspection(_context->selectedHiveNumber);

        // Signal to launch inspection app
        result.launchInspection = true;
        result.exitApp = true;

        buzzSuccess();
    }

    return result;
}

NavigationResult ScreenSelectHive::onBack() {
    NavigationResult result;
    result.nextScreen = ScreenType::HIVE_MGMT;
    return result;
}

} // namespace YARD_MANAGEMENT
