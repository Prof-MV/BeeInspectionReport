/**
 * @file gui_beehive_inspection.h
 * @brief GUI class for beehive inspection app
 * @version 1.0
 * @date 2026-02-08
 */
#pragma once
#include "../../utilities/gui_base/gui_base.h"

class GUI_BeehiveInspection : public GUI_Base {
public:
    GUI_BeehiveInspection() = default;
    ~GUI_BeehiveInspection() override = default;

    void init() override;

    // Get canvas for screen drawing
    LGFX_Sprite* getCanvas() { return _canvas; }

    // Get theme color
    uint32_t getThemeColor() { return _theme_color; }
};
