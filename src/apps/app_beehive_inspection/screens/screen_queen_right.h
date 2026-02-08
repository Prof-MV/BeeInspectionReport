/**
 * @file screen_queen_right.h
 * @brief Screen 1: Queen Right (Yes/No toggle)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

class ScreenQueenRight : public ScreenBase {
private:
    bool _selection;  // true = Yes, false = No

public:
    ScreenQueenRight(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
