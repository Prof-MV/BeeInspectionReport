/**
 * @file screen_select_yard_hive.h
 * @brief Yard and hive selection screen for inspection app
 * @version 1.0
 * @date 2026-03-13
 */
#pragma once
#include "screen_base.h"
#include "../../app_yard_management/yard_data.h"
#include <vector>

namespace BEEHIVE_INSPECTION {

enum class SelectState {
    YARD_SELECT,   // Selecting a yard
    HIVE_SELECT    // Selecting a hive within chosen yard
};

class ScreenSelectYardHive : public ScreenBase {
private:
    SelectState _state;
    int _selectedIndex;
    int _scrollOffset;

    // Yard data
    std::vector<uint32_t> _yardNumbers;
    std::vector<const char*> _yardNames;
    uint32_t _selectedYardNumber;

    // Hive data
    std::vector<uint32_t> _hiveNumbers;

    // Allocated strings for yard names (need to manage memory)
    std::vector<char*> _allocatedNames;

    void loadYards();
    void loadHives(uint32_t yardNumber);
    void renderYardSelect();
    void renderHiveSelect();
    void cleanupAllocatedNames();

public:
    ScreenSelectYardHive(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context);
    ~ScreenSelectYardHive();

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;

    // Check if selection is complete
    bool isSelectionComplete() const { return _context && _context->isValid(); }
};

} // namespace BEEHIVE_INSPECTION
