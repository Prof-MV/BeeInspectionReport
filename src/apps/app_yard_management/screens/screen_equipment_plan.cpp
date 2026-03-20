/**
 * @file screen_equipment_plan.cpp
 * @brief Screen: Equipment Plan implementation
 * @version 1.0
 * @date 2026-03-20
 */
#include "screen_equipment_plan.h"
#include "../yard_storage.h"
#include <esp_timer.h>
#include <cstdio>

namespace YARD_MANAGEMENT {

ScreenEquipmentPlan::ScreenEquipmentPlan(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentEquipmentIndex(0)
    , _dataLoaded(false)
{
}

void ScreenEquipmentPlan::loadData() {
    _dataLoaded = false;

    if (_context->selectedYardNumber == 0) {
        return;
    }

    // Try to load existing plan, or create new one
    if (!loadEquipmentPlan(_context->selectedYardNumber, _plan)) {
        _plan.reset();
        _plan.yardNumber = _context->selectedYardNumber;
    }

    _dataLoaded = true;
}

void ScreenEquipmentPlan::saveData() {
    if (!_dataLoaded || _context->selectedYardNumber == 0) {
        return;
    }

    // Update timestamp
    _plan.timestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    _plan.yardNumber = _context->selectedYardNumber;

    if (saveEquipmentPlan(_plan)) {
        buzzSuccess();
    } else {
        buzzError();
    }
}

void ScreenEquipmentPlan::onEnter() {
    _currentEquipmentIndex = 0;
    loadData();
    render();
}

void ScreenEquipmentPlan::onExit() {
    // Data is saved on completion or can be discarded on back
}

void ScreenEquipmentPlan::render() {
    drawBackground();

    // Header
    drawHeader("EQUIPMENT PLAN");

    // Progress indicator
    char progress[24];
    snprintf(progress, sizeof(progress), "%d of %d", _currentEquipmentIndex + 1, EQUIPMENT_COUNT);
    drawSubheader(progress);

    // Current equipment type name
    EquipmentType currentType = static_cast<EquipmentType>(_currentEquipmentIndex);
    const char* equipmentName = getEquipmentName(currentType);

    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(equipmentName, DISPLAY_CENTER_X, 85);

    // Current quantity - use counter input style
    uint8_t currentQty = _plan.quantities[_currentEquipmentIndex];
    drawCounterInput(currentQty, 0, MAX_QUANTITY, 105);

    // Navigation hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);

    if (_currentEquipmentIndex < EQUIPMENT_COUNT - 1) {
        _canvas->drawString("Click: Next", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        _canvas->drawString("Click: Save", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }

    pushToDisplay();
}

void ScreenEquipmentPlan::onRotate(int direction) {
    if (!_dataLoaded) return;

    uint8_t currentQty = _plan.quantities[_currentEquipmentIndex];

    if (direction > 0) {
        // Increment
        if (currentQty < MAX_QUANTITY) {
            currentQty++;
        } else {
            currentQty = 0;  // Wrap to 0
        }
    } else {
        // Decrement
        if (currentQty > 0) {
            currentQty--;
        } else {
            currentQty = MAX_QUANTITY;  // Wrap to max
        }
    }

    _plan.quantities[_currentEquipmentIndex] = currentQty;
    buzzNavigate();
    render();
}

NavigationResult ScreenEquipmentPlan::onConfirm() {
    NavigationResult result;

    if (!_dataLoaded) {
        result.nextScreen = ScreenType::YARD_ACTIONS;
        return result;
    }

    if (_currentEquipmentIndex < EQUIPMENT_COUNT - 1) {
        // Move to next equipment type
        _currentEquipmentIndex++;
        buzzConfirm();
        render();
    } else {
        // All equipment entered, save and return
        saveData();
        result.nextScreen = ScreenType::YARD_ACTIONS;
    }

    return result;
}

NavigationResult ScreenEquipmentPlan::onBack() {
    NavigationResult result;

    if (_currentEquipmentIndex > 0) {
        // Go back to previous equipment type
        _currentEquipmentIndex--;
        buzzNavigate();
        render();
    } else {
        // At first item, go back to yard actions (discard changes)
        result.nextScreen = ScreenType::YARD_ACTIONS;
    }

    return result;
}

} // namespace YARD_MANAGEMENT
