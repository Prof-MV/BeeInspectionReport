/**
 * @file gui_settings.h
 * @brief GUI for Settings app
 * @version 1.0
 * @date 2026-03-20
 */
#pragma once
#include "../../utilities/gui_base/gui_base.h"

class GUI_Settings : public GUI_Base
{
    public:
        void init() override;

        // Main menu
        void renderMainMenu(int selectedOption);

        // Timezone edit screen
        void renderTimezoneEdit(int8_t offset);

        // Clear data confirmation screen
        void renderClearConfirm(bool yesSelected, bool clearing, bool cleared);

        // Sleep confirmation screen
        void renderSleepConfirm(bool yesSelected);
};
