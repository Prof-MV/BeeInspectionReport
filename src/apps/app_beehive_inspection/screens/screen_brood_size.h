/**
 * @file screen_brood_size.h
 * @brief Screen 5: Brood Size (0-40 frames counter)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

class ScreenBroodSize : public ScreenBase {
private:
    int _frames;

public:
    ScreenBroodSize(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
