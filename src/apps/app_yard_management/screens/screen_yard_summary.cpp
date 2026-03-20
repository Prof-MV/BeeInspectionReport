/**
 * @file screen_yard_summary.cpp
 * @brief Screen 5: Yard Summary implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_yard_summary.h"
#include "../yard_storage.h"
#include <cstdio>

namespace YARD_MANAGEMENT {

ScreenYardSummary::ScreenYardSummary(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _dataLoaded(false)
{
}

void ScreenYardSummary::loadData() {
    _dataLoaded = false;

    if (_context->selectedYardNumber == 0) {
        return;
    }

    // Load yard record
    if (!loadYard(_context->selectedYardNumber, _yard)) {
        return;
    }

    // Calculate statistics
    _stats = calculateYardStats(_context->selectedYardNumber);
    _dataLoaded = true;
}

void ScreenYardSummary::onEnter() {
    loadData();
    render();
}

void ScreenYardSummary::onExit() {
    // Nothing to save
}

void ScreenYardSummary::render() {
    drawBackground();

    if (!_dataLoaded) {
        drawHeader("YARD SUMMARY");

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("Failed to load yard", DISPLAY_CENTER_X, 120);

        pushToDisplay();
        return;
    }

    // Draw header with yard number
    char header[32];
    snprintf(header, sizeof(header), "YARD %06lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawHeader(header);

    // Draw nickname as subheader
    drawSubheader(_yard.nickname);

    // Draw statistics
    int startY = 80;
    int lineSpacing = 28;

    // Total Hives
    drawStatLine("Total Hives:", static_cast<int>(_stats.totalHives), startY);

    // Queenright
    if (_stats.hasInspectionData) {
        drawStatLine("Queenright:", static_cast<int>(_stats.queenrightHives), startY + lineSpacing);
    } else {
        drawStatLine("Queenright:", "N/A", startY + lineSpacing);
    }

    // Total Supers
    if (_stats.hasInspectionData) {
        drawStatLine("Total Supers:", static_cast<int>(_stats.totalSupers), startY + lineSpacing * 2);
    } else {
        drawStatLine("Total Supers:", "N/A", startY + lineSpacing * 2);
    }

    // Average Fill
    if (_stats.hasInspectionData && _stats.totalSupers > 0) {
        char fillStr[16];
        snprintf(fillStr, sizeof(fillStr), "%.0f%%", _stats.averageFillPercent);
        drawStatLine("Avg Fill:", fillStr, startY + lineSpacing * 3);
    } else {
        drawStatLine("Avg Fill:", "N/A", startY + lineSpacing * 3);
    }

    // Navigation hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString("Click: Actions", DISPLAY_CENTER_X, CONTENT_BOTTOM);

    pushToDisplay();
}

void ScreenYardSummary::onRotate(int direction) {
    // No rotation action on summary screen
    // Could add scrolling if more stats are added
}

NavigationResult ScreenYardSummary::onConfirm() {
    NavigationResult result;
    result.nextScreen = ScreenType::YARD_ACTIONS;
    buzzConfirm();
    return result;
}

NavigationResult ScreenYardSummary::onBack() {
    NavigationResult result;
    result.nextScreen = ScreenType::YARD_LIST;
    return result;
}

} // namespace YARD_MANAGEMENT
