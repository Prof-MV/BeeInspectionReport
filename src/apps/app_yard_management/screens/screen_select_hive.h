/**
 * @file screen_select_hive.h
 * @brief Screen: Select Hive - Choose a hive for inspection
 * @version 1.0
 * @date 2026-03-13
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_storage.h"
#include <vector>

namespace YARD_MANAGEMENT {

class ScreenSelectHive : public ScreenBaseYM {
private:
    std::vector<uint32_t> _hiveNumbers;
    std::vector<const char*> _displayItems;
    std::vector<char*> _itemStrings;
    int _selectedIndex;
    int _scrollOffset;

    void loadHiveList();
    void freeItemStrings();

public:
    ScreenSelectHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenSelectHive() override;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
