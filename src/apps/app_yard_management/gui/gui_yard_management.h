/**
 * @file gui_yard_management.h
 * @brief GUI class for yard management app
 * @version 1.0
 * @date 2026-02-12
 */
#pragma once
#include "../../utilities/gui_base/gui_base.h"

class GUI_YardManagement : public GUI_Base {
public:
    GUI_YardManagement() = default;
    ~GUI_YardManagement() override = default;

    void init() override;

    // Get canvas for screen drawing
    LGFX_Sprite* getCanvas() { return _canvas; }

    // Get theme color
    uint32_t getThemeColor() { return _theme_color; }
};
