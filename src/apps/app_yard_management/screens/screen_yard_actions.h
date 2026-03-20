/**
 * @file screen_yard_actions.h
 * @brief Screen: Yard Actions Menu - Hive Mgmt/Equipment Plan options
 * @version 1.0
 * @date 2026-03-20
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

class ScreenYardActions : public ScreenBaseYM {
private:
    int _selectedIndex;
    static constexpr int OPTION_COUNT = 2;
    static constexpr const char* OPTIONS[OPTION_COUNT] = {"Hive Mgmt", "Equipment Plan"};

public:
    ScreenYardActions(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenYardActions() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
