/**
 * @file screen_select_yard_hive.cpp
 * @brief Yard and hive selection screen implementation
 * @version 1.0
 * @date 2026-03-13
 */
#include "screen_select_yard_hive.h"
#include "../../app_yard_management/yard_storage.h"
#include <cstdio>
#include <cstring>

namespace BEEHIVE_INSPECTION {

ScreenSelectYardHive::ScreenSelectYardHive(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, 0xFF)  // Special screen index
    , _state(SelectState::YARD_SELECT)
    , _selectedIndex(0)
    , _scrollOffset(0)
    , _selectedYardNumber(0)
{
}

ScreenSelectYardHive::~ScreenSelectYardHive() {
    cleanupAllocatedNames();
}

void ScreenSelectYardHive::cleanupAllocatedNames() {
    for (char* name : _allocatedNames) {
        delete[] name;
    }
    _allocatedNames.clear();
    _yardNames.clear();
}

void ScreenSelectYardHive::loadYards() {
    cleanupAllocatedNames();
    _yardNumbers.clear();

    // Get active yards
    _yardNumbers = YARD_MANAGEMENT::getActiveYardNumbers();

    // Load yard names
    for (uint32_t yardNum : _yardNumbers) {
        YARD_MANAGEMENT::YardRecord yard;
        if (YARD_MANAGEMENT::loadYard(yardNum, yard)) {
            char* name = new char[YARD_MANAGEMENT::MAX_NICKNAME_LENGTH + 1];
            strncpy(name, yard.nickname, YARD_MANAGEMENT::MAX_NICKNAME_LENGTH);
            name[YARD_MANAGEMENT::MAX_NICKNAME_LENGTH] = '\0';
            _allocatedNames.push_back(name);
            _yardNames.push_back(name);
        }
    }
}

void ScreenSelectYardHive::loadHives(uint32_t yardNumber) {
    _hiveNumbers.clear();
    _hiveNumbers = YARD_MANAGEMENT::getActiveHivesInYard(yardNumber);
}

void ScreenSelectYardHive::onEnter() {
    _state = SelectState::YARD_SELECT;
    _selectedIndex = 0;
    _scrollOffset = 0;
    _selectedYardNumber = 0;
    loadYards();
    render();
}

void ScreenSelectYardHive::onExit() {
    // Context should already be set if selection was completed
}

void ScreenSelectYardHive::render() {
    if (_state == SelectState::YARD_SELECT) {
        renderYardSelect();
    } else {
        renderHiveSelect();
    }
}

void ScreenSelectYardHive::renderYardSelect() {
    drawBackground();
    drawHeader("SELECT YARD");

    if (_yardNumbers.empty()) {
        // No yards available
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No yards found", DISPLAY_CENTER_X, DISPLAY_CENTER_Y);
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->drawString("Create a yard first", DISPLAY_CENTER_X, DISPLAY_CENTER_Y + 30);
    } else {
        int yStart = 75;
        int ySpacing = 38;
        int maxVisible = 4;

        // Adjust scroll offset
        if (_selectedIndex < _scrollOffset) {
            _scrollOffset = _selectedIndex;
        } else if (_selectedIndex >= _scrollOffset + maxVisible) {
            _scrollOffset = _selectedIndex - maxVisible + 1;
        }

        for (int i = 0; i < maxVisible && (_scrollOffset + i) < static_cast<int>(_yardNumbers.size()); i++) {
            int idx = _scrollOffset + i;
            int y = yStart + i * ySpacing;
            bool isSelected = (idx == _selectedIndex);
            drawOption(_yardNames[idx], y, isSelected, false);
        }

        // Draw scroll indicators
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        if (_scrollOffset > 0) {
            _canvas->setTextDatum(textdatum_t::top_center);
            _canvas->drawString("^", DISPLAY_CENTER_X, 55);
        }
        if (_scrollOffset + maxVisible < static_cast<int>(_yardNumbers.size())) {
            _canvas->setTextDatum(textdatum_t::bottom_center);
            _canvas->drawString("v", DISPLAY_CENTER_X, 195);
        }
    }

    // Draw back hint at bottom
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString("hold = exit", DISPLAY_CENTER_X, CONTENT_BOTTOM);

    pushToDisplay();
}

void ScreenSelectYardHive::renderHiveSelect() {
    drawBackground();

    // Find yard name for header
    const char* yardName = "SELECT HIVE";
    for (size_t i = 0; i < _yardNumbers.size(); i++) {
        if (_yardNumbers[i] == _selectedYardNumber) {
            yardName = _yardNames[i];
            break;
        }
    }
    drawHeader(yardName);

    if (_hiveNumbers.empty()) {
        // No hives in yard
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No hives", DISPLAY_CENTER_X, DISPLAY_CENTER_Y);
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->drawString("Add hives to yard", DISPLAY_CENTER_X, DISPLAY_CENTER_Y + 30);
    } else {
        int yStart = 75;
        int ySpacing = 38;
        int maxVisible = 4;

        // Adjust scroll offset
        if (_selectedIndex < _scrollOffset) {
            _scrollOffset = _selectedIndex;
        } else if (_selectedIndex >= _scrollOffset + maxVisible) {
            _scrollOffset = _selectedIndex - maxVisible + 1;
        }

        for (int i = 0; i < maxVisible && (_scrollOffset + i) < static_cast<int>(_hiveNumbers.size()); i++) {
            int idx = _scrollOffset + i;
            int y = yStart + i * ySpacing;
            bool isSelected = (idx == _selectedIndex);

            char hiveStr[16];
            snprintf(hiveStr, sizeof(hiveStr), "Hive %lu", static_cast<unsigned long>(_hiveNumbers[idx]));
            drawOption(hiveStr, y, isSelected, false);
        }

        // Draw scroll indicators
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        if (_scrollOffset > 0) {
            _canvas->setTextDatum(textdatum_t::top_center);
            _canvas->drawString("^", DISPLAY_CENTER_X, 55);
        }
        if (_scrollOffset + maxVisible < static_cast<int>(_hiveNumbers.size())) {
            _canvas->setTextDatum(textdatum_t::bottom_center);
            _canvas->drawString("v", DISPLAY_CENTER_X, 195);
        }
    }

    // Draw back hint at bottom
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString("hold = back", DISPLAY_CENTER_X, CONTENT_BOTTOM);

    pushToDisplay();
}

bool ScreenSelectYardHive::onRotate(int direction) {
    int itemCount = (_state == SelectState::YARD_SELECT)
                        ? static_cast<int>(_yardNumbers.size())
                        : static_cast<int>(_hiveNumbers.size());

    if (itemCount == 0) return true;

    _selectedIndex += direction;

    // Wrap around
    if (_selectedIndex < 0) _selectedIndex = itemCount - 1;
    if (_selectedIndex >= itemCount) _selectedIndex = 0;

    buzzNavigate();
    render();
    return true;
}

bool ScreenSelectYardHive::onConfirm() {
    if (_state == SelectState::YARD_SELECT) {
        if (_yardNumbers.empty()) {
            buzzError();
            return false;
        }

        // Select this yard and move to hive selection
        _selectedYardNumber = _yardNumbers[_selectedIndex];
        loadHives(_selectedYardNumber);
        _state = SelectState::HIVE_SELECT;
        _selectedIndex = 0;
        _scrollOffset = 0;
        buzzConfirm();
        render();
        return false;
    } else {
        // Hive selection
        if (_hiveNumbers.empty()) {
            buzzError();
            return false;
        }

        // Set context with selected yard and hive
        uint32_t selectedHive = _hiveNumbers[_selectedIndex];
        _context->yardNumber = _selectedYardNumber;
        _context->hiveNumber = selectedHive;

        // Copy yard nickname
        for (size_t i = 0; i < _yardNumbers.size(); i++) {
            if (_yardNumbers[i] == _selectedYardNumber) {
                strncpy(_context->yardNickname, _yardNames[i], sizeof(_context->yardNickname) - 1);
                _context->yardNickname[sizeof(_context->yardNickname) - 1] = '\0';
                break;
            }
        }

        // Set hive ID in record
        _data->hiveId = selectedHive;

        buzzConfirm();
        return true;  // Signal selection complete, advance to inspection screens
    }
}

bool ScreenSelectYardHive::onBack() {
    if (_state == SelectState::HIVE_SELECT) {
        // Go back to yard selection
        _state = SelectState::YARD_SELECT;
        _selectedIndex = 0;
        _scrollOffset = 0;

        // Find previous yard index
        for (size_t i = 0; i < _yardNumbers.size(); i++) {
            if (_yardNumbers[i] == _selectedYardNumber) {
                _selectedIndex = static_cast<int>(i);
                break;
            }
        }

        buzzNavigate();
        render();
        return true;  // Handled internally
    }
    // On yard selection, double-click exits app
    buzzNavigate();
    return false;  // Exit app
}

} // namespace BEEHIVE_INSPECTION
