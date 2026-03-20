/**
 * @file screen_equipment_plan.h
 * @brief Screen: Equipment Plan - Input equipment quantities for a yard
 * @version 1.0
 * @date 2026-03-20
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_data.h"

namespace YARD_MANAGEMENT {

class ScreenEquipmentPlan : public ScreenBaseYM {
private:
    EquipmentPlan _plan;
    int _currentEquipmentIndex;
    bool _dataLoaded;

    static constexpr int EQUIPMENT_COUNT = static_cast<int>(EquipmentType::EQUIPMENT_COUNT);
    static constexpr uint8_t MAX_QUANTITY = 99;

    void loadData();
    void saveData();

public:
    ScreenEquipmentPlan(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenEquipmentPlan() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
