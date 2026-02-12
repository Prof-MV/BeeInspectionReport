/**
 * @file screen_yard_summary.h
 * @brief Screen 5: Yard Summary - Display statistics
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

class ScreenYardSummary : public ScreenBaseYM {
private:
    YardSummaryStats _stats;
    YardRecord _yard;
    bool _dataLoaded;

    void loadData();

public:
    ScreenYardSummary(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenYardSummary() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
