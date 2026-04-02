/**
 * @file screen_edit_yard.h
 * @brief Screen: Edit Yard - Multi-step (Select -> Nickname -> RFID)
 * @version 1.0
 * @date 2026-03-28
 */
#pragma once
#include "screen_base_ym.h"
#include <vector>

namespace YARD_MANAGEMENT {

enum class EditYardStep {
    SELECT_YARD,
    NICKNAME,
    RFID_SCAN,
    COMPLETE
};

class ScreenEditYard : public ScreenBaseYM {
private:
    EditYardStep _currentStep;

    // Yard selection state
    std::vector<uint32_t> _yardNumbers;
    int _selectedIndex;
    int _scrollOffset;

    // Nickname input state
    int _cursorPos;
    char _selectedChar;
    char _nickname[MAX_NICKNAME_LENGTH + 1];
    char _originalNickname[MAX_NICKNAME_LENGTH + 1];

    // RFID state
    bool _scanning;
    bool _scanSuccess;
    bool _scanError;
    char _scannedTag[RFID_TAG_LENGTH + 1];
    char _originalRfidTag[RFID_TAG_LENGTH + 1];
    char _errorMessage[32];

    void renderSelectYardStep();
    void renderNicknameStep();
    void renderRfidStep();

    void loadYardList();
    void loadSelectedYardData();
    void startRfidScan();
    void checkRfidScan();
    bool updateYard();

public:
    ScreenEditYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenEditYard() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
    NavigationResult onUpdate() override;
};

} // namespace YARD_MANAGEMENT
