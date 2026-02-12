/**
 * @file screen_close_hive.h
 * @brief Screen 8: Close Hive - Multi-step (Select -> Reason -> RFID)
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "screen_base_ym.h"
#include "../yard_storage.h"
#include <vector>

namespace YARD_MANAGEMENT {

enum class CloseHiveStep {
    SELECT,
    REASON,
    RFID_SCAN,
    COMPLETE
};

class ScreenCloseHive : public ScreenBaseYM {
private:
    CloseHiveStep _currentStep;

    // Hive selection state
    std::vector<uint32_t> _hiveNumbers;
    std::vector<const char*> _displayItems;
    std::vector<char*> _itemStrings;
    int _selectedIndex;
    int _scrollOffset;

    // Reason selection state
    int _reasonSelectedIndex;
    static constexpr int REASON_COUNT = 3;
    static constexpr const char* REASONS[REASON_COUNT] = {"Sold", "Dead", "Absconded"};
    static constexpr char REASON_CODES[REASON_COUNT] = {'S', 'D', 'O'};

    // RFID state
    bool _scanning;
    bool _scanSuccess;
    bool _scanError;
    char _scannedTag[RFID_TAG_LENGTH + 1];
    char _expectedTag[RFID_TAG_LENGTH + 1];
    char _errorMessage[32];

    uint32_t _hiveToClose;

    void loadHiveList();
    void freeItemStrings();
    void renderSelectStep();
    void renderReasonStep();
    void renderRfidStep();

    void startRfidScan();
    void simulateRfidScan();
    bool verifyRfidTag();
    bool closeSelectedHive();

public:
    ScreenCloseHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context);
    ~ScreenCloseHive() override;

    void onEnter() override;
    void onExit() override;
    void render() override;

    void onRotate(int direction) override;
    NavigationResult onConfirm() override;
    NavigationResult onBack() override;
};

} // namespace YARD_MANAGEMENT
