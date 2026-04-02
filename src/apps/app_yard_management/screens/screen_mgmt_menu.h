/**
 * @file screen_mgmt_menu.h
 * @brief Screen 2: Management Menu - Create/Close Yard options
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

class ScreenMgmtMenu : public ScreenBaseYM {
private:
    int _selectedIndex;
    static constexpr int OPTION_COUNT = 3;
    static constexpr const char* OPTIONS[OPTION_COUNT] = {"Create Yard", "Close Yard", "Edit Yard"};

public:
    ScreenMgmtMenu(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenMgmtMenu() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
