/**
 * @file launcher_render_callback.hpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#pragma once
#include "launcher_icons/launcher_icons.h"
#include "../../hal/hal.h"
#include "../app_settings/settings_storage.h"
#include <iostream>
#include <array>
#include <cstdio>


/* Render setting */
#define THEME_COLOR_FG              (uint32_t)0xfa7000
// #define THEME_COLOR_BG              (uint32_t)0x424242
#define THEME_COLOR_BG              TFT_BLACK
#define SELECTOR_COLOR              (uint32_t)0xF3E9D2
#define SELECTOR_RADIUS             5
#define ICON_RADIUS                 22
#define ICON_SELECTED_R_OFFSET      3
#define ICON_TAG_COLOR              SELECTOR_COLOR
#define ICON_TAG_UP_OFFSET          -24
#define ICON_TAG_DOWN_OFFSET        0

#define ICON_NUM                    4


/* Strucut to hold icon value */
struct Icon_t
{
    std::string tag_up;
    std::string tag_down;
    void* icon_pic;
    uint32_t color;
    int x;
    int y;
};

/* Icon list */
static std::array<Icon_t, ICON_NUM> icon_list;
/* Icon color */
static std::array<uint32_t, ICON_NUM> icon_color_list = {
    0xFFA500,  // Beehive Inspection - amber/orange
    0x4CAF50,  // Yard Management - green
    0x2196F3,  // BLE Data Transfer - blue
    0xF44336   // Settings - red
};
/* Icon tag */
static std::array<std::string, ICON_NUM * 2> icon_tag_list = {
    "BEEHIVE", "INSPECT",
    "YARD", "MGMT",
    "BLE", "DATA",
    "SETT", "INGS"
};
/* Icon pic */
static std::array<const uint16_t*, ICON_NUM> icon_pic_list = {
    image_data_icon_beehive,
    image_data_icon_beehive,  // Reuse beehive icon for now
    image_data_icon_beehive,  // Reuse beehive icon for BLE (can replace with BLE icon later)
    image_data_icon_gear      // Settings gear icon
};
/* Sprite to render icon with transparency */
static LGFX_Sprite icon_sprite_list[ICON_NUM];




struct LauncherRender_CB_t : public SMOOTH_MENU::SimpleMenuCallback_t
{
    private:
        LGFX_Sprite* _canvas;
        HAL::HAL* _hal = nullptr;

    public:
        // LauncherRender_CB_t() = default;
        // ~LauncherRender_CB_t() = default;

        inline void setCanvas(LGFX_Sprite* canvasPtr) { _canvas = canvasPtr; }
        inline void setHAL(HAL::HAL* hal) { _hal = hal; }

        /* Override render callback */
        void renderCallback(
            const std::vector<SMOOTH_MENU::Item_t*>& menuItemList,
            const SMOOTH_MENU::RenderAttribute_t& selector,
            const SMOOTH_MENU::RenderAttribute_t& camera
        ) override
        {
            /* Clear screen */
            _canvas->fillScreen(THEME_COLOR_BG);

            /* Draw selector */
            _canvas->fillSmoothCircle(selector.x, selector.y, SELECTOR_RADIUS, SELECTOR_COLOR);
            // printf("%d %d\n", selector.x, selector.y);
            // printf("%d %d\n", menuItemList[selector.targetItem]->x, menuItemList[selector.targetItem]->y);


            /* Draw icon that moves with menu item */
            int icon_r = 190;
            int x;
            int y;
            for (int i = 0; i < icon_list.size(); i++)
            {
                /* Calculate position */
                x = (menuItemList[i]->x - 120) * icon_r / 120 + 120;
                y = (menuItemList[i]->y - 120) * icon_r / 120 + 120;

                if (i == selector.targetItem)
                {
                    _canvas->fillSmoothCircle(x, y, selector.width + ICON_SELECTED_R_OFFSET, icon_list[i].color);
                    icon_sprite_list[i].pushRotateZoom(_canvas, x, y, 0, 1.1, 1.1, TFT_BLACK);
                }
                else
                {
                    _canvas->fillSmoothCircle(x, y, ICON_RADIUS, icon_list[i].color);
                    icon_sprite_list[i].pushRotateZoom(_canvas, x, y, 0, 1, 1, TFT_BLACK);
                }
            }


            /* Draw Icon tag */
            /* Get Icon offset position */
            int new_r = 20;
            int icon_tag_x = (selector.x - 120) * new_r / 120 + 120;
            int icon_tag_y = (selector.y - 120) * new_r / 120 + 120;

            /* If pointing a icon */
            if (selector.targetItem < icon_list.size())
            {
                _canvas->setFont(GUI_FONT_CN_BIG);
                _canvas->setTextColor(ICON_TAG_COLOR);

                /* Darw tag with offset */
                _canvas->drawCenterString(
                    icon_list[selector.targetItem].tag_up.c_str(),
                    icon_tag_x,
                    icon_tag_y + ICON_TAG_UP_OFFSET
                );
                _canvas->drawCenterString(
                    icon_list[selector.targetItem].tag_down.c_str(),
                    icon_tag_x,
                    icon_tag_y + ICON_TAG_DOWN_OFFSET
                );
            }
            else
            {
                _canvas->drawCenterString(
                    "-. -",
                    icon_tag_x,
                    icon_tag_y + ICON_TAG_UP_OFFSET
                );
            }

            /* Draw time and date at the bottom center */
            if (_hal != nullptr)
            {
                tm time_info;
                if (_hal->rtc.getTime(time_info) == ESP_OK)
                {
                    // Month abbreviations
                    static const char* months[] = {
                        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
                        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
                    };

                    char time_str[16];
                    char date_str[16];

                    // Adjust for timezone from settings
                    int8_t tzOffset = SETTINGS::getTimezoneOffset();
                    int hour = time_info.tm_hour + tzOffset;
                    int day = time_info.tm_mday;
                    int mon = time_info.tm_mon;
                    int year = time_info.tm_year;

                    // Handle day rollover for negative offsets
                    if (hour < 0) {
                        hour += 24;
                        day -= 1;
                        if (day < 1) {
                            mon -= 1;
                            if (mon < 0) {
                                mon = 11;
                                year -= 1;
                            }
                            // Approximate days in previous month
                            static const int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
                            day = days_in_month[mon];
                        }
                    }
                    // Handle day rollover for positive offsets
                    else if (hour >= 24) {
                        hour -= 24;
                        day += 1;
                        static const int days_in_month[] = {31,28,31,30,31,30,31,31,30,31,30,31};
                        if (day > days_in_month[mon]) {
                            day = 1;
                            mon += 1;
                            if (mon > 11) {
                                mon = 0;
                                year += 1;
                            }
                        }
                    }

                    // Format time as HH:MM
                    snprintf(time_str, sizeof(time_str), "%02d:%02d",
                             hour, time_info.tm_min);

                    // Format date as DD MMM YY
                    snprintf(date_str, sizeof(date_str), "%02d %s %02d",
                             day, months[mon], year % 100);

                    // Draw time at top center (bigger font)
                    _canvas->setFont(&fonts::FreeSansBold18pt7b);
                    _canvas->setTextColor(TFT_WHITE);
                    _canvas->setTextDatum(textdatum_t::top_center);
                    _canvas->drawString(time_str, 120, 15);

                    // Draw date at bottom center
                    _canvas->setFont(&fonts::FreeSans9pt7b);
                    _canvas->setTextDatum(textdatum_t::bottom_center);
                    _canvas->drawString(date_str, 120, 225);
                }
            }


            // _canvas->fillScreen(TFT_BLACK);

            // _canvas->pushSprite(0, 0);
        }
};



