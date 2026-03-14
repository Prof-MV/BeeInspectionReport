/**
 * @file screen_queen_cells.cpp
 * @brief Screen 2: Queen Cells (two-level menu for supersedure/swarm cells)
 * @version 1.0
 * @date 2026-02-08
 */
#include "screen_queen_cells.h"
#include <cstdio>

namespace BEEHIVE_INSPECTION {

const char* ScreenQueenCells::getTypeName(int index) {
    switch (index) {
        case 0: return "Supersedure";
        case 1: return "Swarm Cells";
        case 2: return "Next >";
        default: return "";
    }
}

const char* ScreenQueenCells::getCountName(int index) {
    switch (index) {
        case 0: return "No";
        case 1: return "Yes";
        case 2: return "1-5";
        case 3: return "5-10";
        case 4: return "10+";
        default: return "No";
    }
}

ScreenQueenCells::ScreenQueenCells(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context)
    : ScreenBase(hal, canvas, data, context, SCREEN_QUEEN_CELLS)
    , _state(QueenCellsState::TYPE_SELECT)
    , _typeIndex(0)
    , _countIndex(0)
    , _supersedure(CellCount::NO)
    , _swarm(CellCount::NO)
{
}

void ScreenQueenCells::onEnter() {
    _supersedure = _data->supersedureCells;
    _swarm = _data->swarmCells;
    _state = QueenCellsState::TYPE_SELECT;
    _typeIndex = 0;
    render();
}

void ScreenQueenCells::onExit() {
    _data->supersedureCells = _supersedure;
    _data->swarmCells = _swarm;
}

void ScreenQueenCells::render() {
    if (_state == QueenCellsState::TYPE_SELECT) {
        renderTypeSelect();
    } else {
        renderCountSelect();
    }
}

void ScreenQueenCells::renderTypeSelect() {
    drawBackground();
    drawHeader("QUEEN CELLS");

    int yStart = 75;
    int ySpacing = 45;

    for (int i = 0; i < TYPE_COUNT; i++) {
        int y = yStart + i * ySpacing;
        bool isSelected = (i == _typeIndex);

        // For Supersedure and Swarm, show current value
        if (i < 2) {
            char displayText[32];
            CellCount count = (i == 0) ? _supersedure : _swarm;
            snprintf(displayText, sizeof(displayText), "%s: %s",
                     getTypeName(i), getCountName(static_cast<int>(count)));
            drawOption(displayText, y, isSelected, false);
        } else {
            // "Next" button
            drawOption(getTypeName(i), y, isSelected, false);
        }
    }

    drawProgressIndicator();
    pushToDisplay();
}

void ScreenQueenCells::renderCountSelect() {
    drawBackground();

    // Show which cell type we're editing
    const char* typeStr = (_typeIndex == 0) ? "SUPERSEDURE" : "SWARM CELLS";
    drawHeader(typeStr);

    // Show count options
    int yStart = 65;
    int ySpacing = 32;

    for (int i = 0; i < COUNT_OPTIONS; i++) {
        int y = yStart + i * ySpacing;
        bool isSelected = (i == _countIndex);
        drawOption(getCountName(i), y, isSelected, false);
    }

    drawProgressIndicator();
    pushToDisplay();
}

bool ScreenQueenCells::onRotate(int direction) {
    if (_state == QueenCellsState::TYPE_SELECT) {
        _typeIndex += direction;
        if (_typeIndex < 0) _typeIndex = TYPE_COUNT - 1;
        if (_typeIndex >= TYPE_COUNT) _typeIndex = 0;
    } else {
        _countIndex += direction;
        if (_countIndex < 0) _countIndex = COUNT_OPTIONS - 1;
        if (_countIndex >= COUNT_OPTIONS) _countIndex = 0;
    }

    buzzNavigate();
    render();
    return true;
}

bool ScreenQueenCells::onConfirm() {
    if (_state == QueenCellsState::TYPE_SELECT) {
        if (_typeIndex == 2) {
            // "Next" selected - advance to next screen
            buzzConfirm();
            return true;
        } else {
            // Enter count selection for this type
            _state = QueenCellsState::COUNT_SELECT;
            CellCount currentCount = (_typeIndex == 0) ? _supersedure : _swarm;
            _countIndex = static_cast<int>(currentCount);
            buzzConfirm();
            render();
            return false;
        }
    } else {
        // Save count and return to type selection
        CellCount newCount = static_cast<CellCount>(_countIndex);
        if (_typeIndex == 0) {
            _supersedure = newCount;
        } else {
            _swarm = newCount;
        }
        _state = QueenCellsState::TYPE_SELECT;
        buzzConfirm();
        render();
        return false;
    }
}

bool ScreenQueenCells::onBack() {
    if (_state == QueenCellsState::COUNT_SELECT) {
        // Go back to type selection
        _state = QueenCellsState::TYPE_SELECT;
        render();
        return true;  // Handled internally
    }
    return false;  // Go to previous screen
}

} // namespace BEEHIVE_INSPECTION
