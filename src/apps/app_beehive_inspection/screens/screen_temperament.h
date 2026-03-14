/**
 * @file screen_temperament.h
 * @brief Screen 4: Temperament (list selection)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

class ScreenTemperament : public ScreenBase {
private:
    static constexpr int OPTION_COUNT = 6;
    int _selectedIndex;

    static const char* getOptionName(int index);

public:
    ScreenTemperament(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
