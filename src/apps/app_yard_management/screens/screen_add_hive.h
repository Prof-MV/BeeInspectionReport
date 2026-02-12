/**
 * @file screen_add_hive.h
 * @brief Screen 7: Add Hive - Multi-step (Number -> Origin -> RFID)
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_storage.h"
#include <vector>

namespace YARD_MANAGEMENT {

enum class AddHiveStep {
    NUMBER,
    ORIGIN,
    RFID_SCAN,
    COMPLETE
};

class ScreenAddHive : public ScreenBaseYM {
private:
    AddHiveStep _currentStep;

    // Number input state
    int _currentDigit;

    // Origin selection state
    std::vector<uint32_t> _originHives;
    std::vector<const char*> _originOptions;
    std::vector<char*> _optionStrings;
    int _originSelectedIndex;
    int _originScrollOffset;

    // RFID state
    bool _scanning;
    bool _scanSuccess;
    bool _scanError;
    char _scannedTag[RFID_TAG_LENGTH + 1];
    char _errorMessage[32];

    // Validation
    bool _numberValid;

    void loadOriginOptions();
    void freeOptionStrings();
    void renderNumberStep();
    void renderOriginStep();
    void renderRfidStep();

    bool validateNumber();
    void startRfidScan();
    void simulateRfidScan();
    bool createHive();

public:
    ScreenAddHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenAddHive() override;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
