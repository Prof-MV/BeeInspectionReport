/**
 * @file screen_queen_cells.h
 * @brief Screen 2: Queen Cells (two-level menu for supersedure/swarm cells)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

// Screen state
enum class QueenCellsState {
    TYPE_SELECT,   // Selecting cell type (Supersedure/Swarm/Next)
    COUNT_SELECT   // Selecting count for chosen type
};

class ScreenQueenCells : public ScreenBase {
private:
    static constexpr int TYPE_COUNT = 3;    // Supersedure, Swarm, Next
    static constexpr int COUNT_OPTIONS = 5; // No, Yes, 1-5, 5-10, 10+

    QueenCellsState _state;
    int _typeIndex;         // 0=Supersedure, 1=Swarm, 2=Next
    int _countIndex;        // Current count selection

    CellCount _supersedure;
    CellCount _swarm;

    void renderTypeSelect();
    void renderCountSelect();

    static const char* getTypeName(int index);
    static const char* getCountName(int index);

public:
    ScreenQueenCells(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
