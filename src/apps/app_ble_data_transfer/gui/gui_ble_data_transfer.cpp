/**
 * @file gui_ble_data_transfer.cpp
 * @brief GUI implementation for BLE Data Transfer app
 * @version 1.0
 * @date 2026-03-19
 */
#include "gui_ble_data_transfer.h"

void GUI_BLE_DataTransfer::init()
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

void GUI_BLE_DataTransfer::renderPage(
    ConnectionStatus_t status,
    const char* deviceName,
    uint16_t inspectionCount,
    uint16_t yardCount,
    uint16_t hiveCount,
    uint8_t transferProgress)
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
    _canvas->drawCenterString("BLE DATA", bubble.x, bubble.y - 60);

    // Device name
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_DARKGRAY);
    _canvas->drawCenterString(deviceName, bubble.x, bubble.y - 38);

    // Status indicator
    int statusY = bubble.y - 15;
    _canvas->setTextColor(TFT_BLACK);
    _canvas->drawString("[Status]:", 25, statusY);

    switch (status) {
        case Status_Advertising:
            _canvas->setTextColor(TFT_ORANGE);
            _canvas->drawString("Advertising...", 100, statusY);
            break;

        case Status_Connected:
            _canvas->setTextColor(TFT_DARKGREEN);
            _canvas->drawString("Connected!", 100, statusY);
            break;

        case Status_Transferring:
            _canvas->setTextColor(TFT_BLUE);
            char progStr[32];
            snprintf(progStr, sizeof(progStr), "Sending %d%%", transferProgress);
            _canvas->drawString(progStr, 100, statusY);
            break;
    }

    // Data counts
    int dataY = bubble.y + 8;
    _canvas->setTextColor(TFT_BLACK);
    char countStr[48];

    snprintf(countStr, sizeof(countStr), "Inspections: %d", inspectionCount);
    _canvas->drawString(countStr, 30, dataY);

    snprintf(countStr, sizeof(countStr), "Yards: %d  Hives: %d", yardCount, hiveCount);
    _canvas->drawString(countStr, 30, dataY + 20);

    // Transfer progress bar (if transferring)
    if (status == Status_Transferring && transferProgress > 0) {
        int barX = 30;
        int barY = dataY + 45;
        int barWidth = 180;
        int barHeight = 12;

        // Background
        _canvas->fillRoundRect(barX, barY, barWidth, barHeight, 4, TFT_LIGHTGRAY);

        // Progress
        int progressWidth = (barWidth * transferProgress) / 100;
        if (progressWidth > 0) {
            _canvas->fillRoundRect(barX, barY, progressWidth, barHeight, 4, TFT_BLUE);
        }
    }

    // Help text at bottom
    _canvas->setFont(GUI_FONT_CN_SMALL);
    _canvas->setTextColor(TFT_DARKGRAY);
    if (status == Status_Advertising) {
        _canvas->drawCenterString("Open BeeData app on phone", bubble.x, bubble.y + 55);
    } else if (status == Status_Connected) {
        _canvas->drawCenterString("Ready to transfer data", bubble.x, bubble.y + 55);
    }

    // Icon
    _draw_top_icon();

    // Quit button
    _draw_quit_button();

    _canvas->drawCircle(120, 120, 120, TFT_DARKGRAY);
    _canvas->pushSprite(0, 0);
}
