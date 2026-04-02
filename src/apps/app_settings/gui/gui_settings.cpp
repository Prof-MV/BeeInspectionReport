/**
 * @file gui_settings.cpp
 * @brief GUI implementation for Settings app
 * @version 1.0
 * @date 2026-03-20
 */
#include "gui_settings.h"
#include <cstdio>

void GUI_Settings::init()
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}

void GUI_Settings::renderMainMenu(int selectedOption)
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Main info bubble
    BasicObeject_t bubble;
    bubble.x = 120;
    bubble.y = 125;
    bubble.width = 220;
    bubble.height = 150;
    _canvas->fillSmoothRoundRect(bubble.x - bubble.width / 2, bubble.y - bubble.height / 2,
                                  bubble.width, bubble.height, 30, TFT_WHITE);

    // Title
    _canvas->setFont(GUI_FONT_CN_BIG);
    _canvas->setTextColor(_theme_color);
    _canvas->drawCenterString("SETTINGS", bubble.x, bubble.y - 65);

    // Menu options
    _canvas->setFont(GUI_FONT_CN_SMALL);

    const char* options[] = {"Timezone", "Clear Data", "Sleep", "Beep", "Done"};
    int optionCount = 5;
    int startY = bubble.y - 30;
    int spacing = 22;

    for (int i = 0; i < optionCount; i++) {
        int y = startY + i * spacing;

        if (i == selectedOption) {
            // Selected item - highlighted
            _canvas->fillRoundRect(bubble.x - 80, y - 2, 160, 20, 8, _theme_color);
            _canvas->setTextColor(TFT_WHITE);
        } else {
            _canvas->setTextColor(TFT_DARKGRAY);
        }

        _canvas->drawCenterString(options[i], bubble.x, y);
    }

    // Help text at bottom
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_DARKGRAY);
    _canvas->drawCenterString("rotate to select", bubble.x, bubble.y + 58);

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}

void GUI_Settings::renderTimezoneEdit(int8_t offset)
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Main info bubble
    BasicObeject_t bubble;
    bubble.x = 120;
    bubble.y = 125;
    bubble.width = 220;
    bubble.height = 150;
    _canvas->fillSmoothRoundRect(bubble.x - bubble.width / 2, bubble.y - bubble.height / 2,
                                  bubble.width, bubble.height, 30, TFT_WHITE);

    // Title
    _canvas->setFont(GUI_FONT_CN_BIG);
    _canvas->setTextColor(_theme_color);
    _canvas->drawCenterString("TIMEZONE", bubble.x, bubble.y - 60);

    // Current timezone value
    _canvas->setFont(&fonts::FreeSansBold18pt7b);
    _canvas->setTextColor(TFT_BLACK);

    char tzStr[16];
    if (offset >= 0) {
        snprintf(tzStr, sizeof(tzStr), "GMT+%d", offset);
    } else {
        snprintf(tzStr, sizeof(tzStr), "GMT%d", offset);
    }
    _canvas->drawCenterString(tzStr, bubble.x, bubble.y - 15);

    // Arrows indicating rotation
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_DARKGRAY);
    _canvas->drawCenterString("< rotate to adjust >", bubble.x, bubble.y + 20);

    // Help text at bottom
    _canvas->drawCenterString("click to save", bubble.x, bubble.y + 55);

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}

void GUI_Settings::renderClearConfirm(bool yesSelected, bool clearing, bool cleared, bool failed)
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Main info bubble
    BasicObeject_t bubble;
    bubble.x = 120;
    bubble.y = 125;
    bubble.width = 220;
    bubble.height = 150;
    _canvas->fillSmoothRoundRect(bubble.x - bubble.width / 2, bubble.y - bubble.height / 2,
                                  bubble.width, bubble.height, 30, TFT_WHITE);

    // Title
    _canvas->setFont(GUI_FONT_CN_BIG);
    _canvas->setTextColor(TFT_RED);
    _canvas->drawCenterString("CLEAR DATA", bubble.x, bubble.y - 60);

    if (cleared) {
        // Success message
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextColor(TFT_DARKGREEN);
        _canvas->drawCenterString("Data Cleared!", bubble.x, bubble.y - 10);

        _canvas->setFont(GUI_FONT_CN_SMALL);
        _canvas->setTextColor(TFT_DARKGRAY);
        _canvas->drawCenterString("click to continue", bubble.x, bubble.y + 30);
    } else if (failed) {
        // Error message
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextColor(TFT_RED);
        _canvas->drawCenterString("Clear Failed!", bubble.x, bubble.y - 10);

        _canvas->setFont(GUI_FONT_CN_SMALL);
        _canvas->setTextColor(TFT_DARKGRAY);
        _canvas->drawCenterString("click to continue", bubble.x, bubble.y + 30);
    } else if (clearing) {
        // Clearing in progress
        _canvas->setFont(&fonts::FreeSans12pt7b);
        _canvas->setTextColor(TFT_ORANGE);
        _canvas->drawCenterString("Clearing...", bubble.x, bubble.y);
    } else {
        // Confirmation dialog
        _canvas->setFont(GUI_FONT_CN_SMALL);
        _canvas->setTextColor(TFT_BLACK);
        _canvas->drawCenterString("Delete ALL yards,", bubble.x, bubble.y - 30);
        _canvas->drawCenterString("hives & inspections?", bubble.x, bubble.y - 12);

        _canvas->setTextColor(TFT_RED);
        _canvas->drawCenterString("Cannot be undone!", bubble.x, bubble.y + 10);

        // Yes/No buttons
        int buttonY = bubble.y + 40;
        int buttonWidth = 60;
        int buttonHeight = 25;
        int spacing = 20;

        // No button (left)
        int noX = bubble.x - spacing/2 - buttonWidth;
        uint16_t noColor = !yesSelected ? TFT_DARKGREEN : TFT_DARKGRAY;
        _canvas->drawRoundRect(noX, buttonY, buttonWidth, buttonHeight, 5, noColor);
        _canvas->setTextColor(noColor);
        _canvas->drawCenterString("No", noX + buttonWidth/2, buttonY + 6);

        // Yes button (right)
        int yesX = bubble.x + spacing/2;
        uint16_t yesColor = yesSelected ? TFT_RED : TFT_DARKGRAY;
        _canvas->drawRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 5, yesColor);
        _canvas->setTextColor(yesColor);
        _canvas->drawCenterString("Yes", yesX + buttonWidth/2, buttonY + 6);
    }

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}

void GUI_Settings::renderSleepConfirm(bool yesSelected)
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Main info bubble
    BasicObeject_t bubble;
    bubble.x = 120;
    bubble.y = 125;
    bubble.width = 220;
    bubble.height = 150;
    _canvas->fillSmoothRoundRect(bubble.x - bubble.width / 2, bubble.y - bubble.height / 2,
                                  bubble.width, bubble.height, 30, TFT_WHITE);

    // Title
    _canvas->setFont(GUI_FONT_CN_BIG);
    _canvas->setTextColor(_theme_color);
    _canvas->drawCenterString("SLEEP", bubble.x, bubble.y - 60);

    // Message
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_BLACK);
    _canvas->drawCenterString("Enter sleep mode?", bubble.x, bubble.y - 25);
    _canvas->setTextColor(TFT_DARKGRAY);
    _canvas->drawCenterString("Press button or", bubble.x, bubble.y - 5);
    _canvas->drawCenterString("rotate to wake up", bubble.x, bubble.y + 12);

    // Yes/No buttons
    int buttonY = bubble.y + 40;
    int buttonWidth = 60;
    int buttonHeight = 25;
    int spacing = 20;

    // No button (left)
    int noX = bubble.x - spacing/2 - buttonWidth;
    uint16_t noColor = !yesSelected ? TFT_DARKGREEN : TFT_DARKGRAY;
    _canvas->drawRoundRect(noX, buttonY, buttonWidth, buttonHeight, 5, noColor);
    _canvas->setTextColor(noColor);
    _canvas->drawCenterString("No", noX + buttonWidth/2, buttonY + 6);

    // Yes button (right)
    int yesX = bubble.x + spacing/2;
    uint16_t yesColor = yesSelected ? _theme_color : TFT_DARKGRAY;
    _canvas->drawRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 5, yesColor);
    _canvas->setTextColor(yesColor);
    _canvas->drawCenterString("Yes", yesX + buttonWidth/2, buttonY + 6);

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}

void GUI_Settings::renderBeepTest()
{
    _canvas->fillScreen(TFT_DARKGRAY);
    _canvas->fillSmoothCircle(120, 120, 120, _theme_color);

    // Main info bubble
    BasicObeject_t bubble;
    bubble.x = 120;
    bubble.y = 125;
    bubble.width = 220;
    bubble.height = 150;
    _canvas->fillSmoothRoundRect(bubble.x - bubble.width / 2, bubble.y - bubble.height / 2,
                                  bubble.width, bubble.height, 30, TFT_WHITE);

    // Title
    _canvas->setFont(GUI_FONT_CN_BIG);
    _canvas->setTextColor(_theme_color);
    _canvas->drawCenterString("BEEP TEST", bubble.x, bubble.y - 60);

    // Message
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_BLACK);
    _canvas->drawCenterString("Press button to", bubble.x, bubble.y - 15);
    _canvas->drawCenterString("play test beep", bubble.x, bubble.y + 5);

    // Help text
    _canvas->setTextColor(TFT_DARKGRAY);
    _canvas->drawCenterString("click to test & exit", bubble.x, bubble.y + 55);

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}
