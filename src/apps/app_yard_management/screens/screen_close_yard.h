/**
 * @file screen_close_yard.h
 * @brief Screen 4: Close Yard - Select yard and confirm closure
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_storage.h"
#include <vector>

namespace YARD_MANAGEMENT {

enum class CloseYardStep {
    SELECT,
    CONFIRM
};

class ScreenCloseYard : public ScreenBaseYM {
private:
    CloseYardStep _currentStep;

    // Selection state
    std::vector<uint32_t> _yardNumbers;
    std::vector<const char*> _displayItems;
    std::vector<char*> _itemStrings;
    int _selectedIndex;
    int _scrollOffset;

    // Confirmation state
    bool _yesSelected;
    uint32_t _yardToClose;
    char _yardName[MAX_NICKNAME_LENGTH + 1];

    void loadYardList();
    void freeItemStrings();
    void renderSelectStep();
    void renderConfirmStep();

public:
    ScreenCloseYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenCloseYard() override;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
