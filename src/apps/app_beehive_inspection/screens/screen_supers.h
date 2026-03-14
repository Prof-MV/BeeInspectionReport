/**
 * @file screen_supers.h
 * @brief Screen 3: Supers (count selection and optional fill percentage)
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "screen_base.h"

namespace BEEHIVE_INSPECTION {

// Screen state
enum class SupersState {
    COUNT_SELECT,  // Selecting number of supers (0-5)
    SUPER_LIST,    // Selecting which super to edit
    FILL_SELECT    // Selecting fill percentage for a super
};

class ScreenSupers : public ScreenBase {
private:
    static constexpr int MAX_SUPERS = 5;
    static constexpr int FILL_OPTIONS = 5;  // 0%, 25%, 50%, 75%, 100%

    SupersState _state;
    int _count;           // Number of supers (0-5)
    int _currentSuper;    // Which super we're editing (0-4)
    int _fillIndex;       // Current fill selection
    int _listIndex;       // Selection in super list (0 to count, where count = "Next")

    FillPercentage _fills[MAX_SUPERS];

    void renderCountSelect();
    void renderSuperList();
    void renderFillSelect();

    static const char* getFillName(int index);

public:
    ScreenSupers(HAL::HAL* hal, LGFX_Sprite* canvas, InspectionRecord* data, InspectionContext* context);

    void onEnter() override;
    void onExit() override;
    void render() override;

    bool onRotate(int direction) override;
    bool onConfirm() override;
    bool onBack() override;
};

} // namespace BEEHIVE_INSPECTION
