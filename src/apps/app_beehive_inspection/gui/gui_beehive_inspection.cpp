/**
 * @file gui_beehive_inspection.cpp
 * @brief GUI class implementation for beehive inspection app
 * @version 1.0
 * @date 2026-02-08
 */
#include "gui_beehive_inspection.h"

void GUI_BeehiveInspection::init() {
    // Clear screen with theme color
    _canvas->fillScreen(_theme_color);
    _canvas->pushSprite(0, 0);
}
