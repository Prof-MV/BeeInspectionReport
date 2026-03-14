/**
 * @file screen_clear_data.h
 * @brief Screen for clearing all yard management data (debug/reset)
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

class ScreenClearData : public ScreenBaseYM {
private:
    bool _confirmSelected;  // true = Yes, false = No
    bool _clearing;
    bool _cleared;

public:
    ScreenClearData(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenClearData() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
