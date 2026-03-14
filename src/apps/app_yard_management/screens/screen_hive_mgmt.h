/**
 * @file screen_hive_mgmt.h
 * @brief Screen 6: Hive Management Menu - Add/Close Hive options
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

class ScreenHiveMgmt : public ScreenBaseYM {
private:
    int _selectedIndex;
    static constexpr int OPTION_COUNT = 3;
    static constexpr const char* OPTIONS[OPTION_COUNT] = {"Select Hive", "Add Hive", "Close Hive"};

public:
    ScreenHiveMgmt(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenHiveMgmt() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
