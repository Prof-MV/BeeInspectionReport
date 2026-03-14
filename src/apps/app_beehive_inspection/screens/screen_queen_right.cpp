/**
 * @file screen_queen_right.cpp
 * @brief Screen 1: Queen Right (Yes/No toggle)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_queen_right.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

ScreenQueenRight::ScreenQueenRight(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, SCREEN_QUEEN_RIGHT)
    , _selection(true)
{
}

void ScreenQueenRight::onEnter() {
    _selection = _data->queenRight;
    render();
}

void ScreenQueenRight::onExit() {
    _data->queenRight = _selection;
}

void ScreenQueenRight::render() {
    drawBackground();
    drawHeader("QUEEN RIGHT");

    // Show yard/hive info if available
    if (_context && _context->isValid()) {
        char subheader[48];
        if (_context->yardNickname[0] != '\0') {
            snprintf(subheader, sizeof(subheader), "%s | Hive %lu",
                     _context->yardNickname, static_cast<unsigned long>(_context->hiveNumber));
        } else {
            snprintf(subheader, sizeof(subheader), "Hive %lu",
                     static_cast<unsigned long>(_context->hiveNumber));
        }
        drawSubheader(subheader);
    }

    // Draw Yes option (shifted down if subheader present)
    int yOffset = (_context && _context->isValid()) ? 15 : 0;
    drawOption("YES", 100 + yOffset, _selection, false);

    // Draw No option
    drawOption("NO", 150 + yOffset, !_selection, false);

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenQueenRight::onRotate(int direction) {
    _selection = !_selection;
    buzzNavigate();
    render();
    return true;
}

bool ScreenQueenRight::onConfirm() {
    _data->queenRight = _selection;
    buzzConfirm();
    return true;  // Advance to next screen
}

bool ScreenQueenRight::onBack() {
    return false;  // First screen - can't go back
}

} // namespace BEEHIVE_INSPECTION
