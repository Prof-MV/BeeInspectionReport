/**
 * @file screen_create_yard.h
 * @brief Screen 3: Create Yard - Multi-step (Number -> Nickname -> RFID)
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"

namespace YARD_MANAGEMENT {

enum class CreateYardStep {
    NUMBER,
    NICKNAME,
    RFID_SCAN,
    COMPLETE
};

class ScreenCreateYard : public ScreenBaseYM {
private:
    CreateYardStep _currentStep;

    // Nickname input state
    int _cursorPos;
    char _selectedChar;
    char _nickname[MAX_NICKNAME_LENGTH + 1];

    // RFID state
    bool _scanning;
    bool _scanSuccess;
    bool _scanError;
    char _scannedTag[RFID_TAG_LENGTH + 1];
    char _errorMessage[32];

    // Validation
    bool _numberValid;

    void renderNumberStep();
    void renderNicknameStep();
    void renderRfidStep();

    bool validateNumber();
    void startRfidScan();
    void simulateRfidScan();  // For testing without hardware
    bool createYard();

public:
    ScreenCreateYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenCreateYard() override = default;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
    NavigationResult onUpdate() override;  // For RFID auto-detection
};

} // namespace YARD_MANAGEMENT
