/**
 * @file screen_yard_list.h
 * @brief Screen 1: Yard List - displays active yards with Management option
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_storage.h"
#include <vector>

namespace YARD_MANAGEMENT {

class ScreenYardList : public ScreenBaseYM {
private:
    std::vector<uint32_t> _yardNumbers;
    std::vector<const char*> _displayItems;
    std::vector<char*> _itemStrings;  // For memory management
    int _selectedIndex;
    int _scrollOffset;
    bool _hasManagement;

    void loadYardList();
    void freeItemStrings();

public:
    ScreenYardList(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenYardList() override;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
