/**
 * @file screen_pests.h
 * @brief Screen 7: Pests (toggle checklist)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

class ScreenPests : public ScreenBase {
private:
    static constexpr int PEST_COUNT = 4;
    static constexpr int ITEM_COUNT = 5;  // 4 pests + "Finish" button

    int _focusedIndex;  // 0-3 = pests, 4 = Finish

    static const char* getPestName(int index);
    static PestFlag getPestFlag(int index);

public:
    ScreenPests(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
