/**
 * @file screen_queen_right.cpp
 * @brief Screen 1: Queen Right (Yes/No toggle)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_queen_right.h"

namespace BEEHIVE_INSPECTION {

ScreenQueenRight::ScreenQueenRight(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data)
    : ScreenBase(hal, canvas, data, SCREEN_QUEEN_RIGHT)
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

    // Draw Yes option
    drawOption("YES", 100, _selection, false);

    // Draw No option
    drawOption("NO", 150, !_selection, false);

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
