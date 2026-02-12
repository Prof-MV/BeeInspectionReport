/**
 * @file gui_yard_management.cpp
 * @brief GUI class implementation for yard management app
 * @version 1.0
 * @date 2026-02-12
 */
#include "gui_yard_management.h"

void GUI_YardManagement::init() {
    // Initialize with dark background (same as beehive inspection)
    _canvas->fillScreen(0x1082);  // COLOR_BACKGROUND
    _canvas->pushSprite(0, 0);
}
