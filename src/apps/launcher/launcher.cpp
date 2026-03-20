/**
 * @file launcher.cpp
 * @author Forairaaaaa
 * @brief 
 * @version 0.1
 * @date 2023-07-25
 * 
 * @copyright Copyright (c) 2023
 * 
 */
#include "launcher.h"
#include "../common_define.h"
#include "../app_yard_management/yard_storage.h"
#include "../app_yard_management/yard_data.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"


using namespace MOONCAKE::USER_APP;


void Launcher::_menu_init()
{
    /* Create a menu to handle selector */
    _data.menu = new SMOOTH_MENU::Simple_Menu;
    _data.menu_render_cb = new LauncherRender_CB_t;
    _data.menu_render_cb->setCanvas(_data.hal->canvas);
    _data.menu_render_cb->setHAL(_data.hal);

    _data.menu->init(240, 240);
    _data.menu->setRenderCallback(_data.menu_render_cb);

    /* Set selector anim */
    auto cfg_selector = _data.menu->getSelector()->config();
    cfg_selector.animPath_x = LVGL::overshoot;
    cfg_selector.animPath_y = LVGL::overshoot;
    cfg_selector.animTime_x = 300;
    cfg_selector.animTime_y = 300;
    _data.menu->getSelector()->config(cfg_selector);

    /* Set menu looply */
    _data.menu->setMenuLoopMode(true);
    _data.menu->getMenu()->x = 0;
    _data.menu->getMenu()->y = 0;

    /* Push selector points into menu */
    int a = 120;
    int b = 120;
    int r = 60;
    int n = ICON_NUM;  // Number of icons
    int x;
    int y;
    // Angles for 4 icons: left side (140°, 180°, 220°) + right side (0° = 3 o'clock)
    double angles[] = {7.0 * 3.14159 / 9.0,    // 140° (upper-left)
                       3.14159,                 // 180° (9 o'clock)
                       11.0 * 3.14159 / 9.0,    // 220° (lower-left)
                       0.0};                    // 0° (3 o'clock) - Settings
    for (int i = 0; i < n; i++)
    {
        x = a + r * std::cos(angles[i]);
        y = b + r * std::sin(angles[i]);
        _data.menu->getMenu()->addItem("", x, y, ICON_RADIUS, ICON_RADIUS);
    }
}


void Launcher::_icon_list_init()
{
    for (int i = 0; i < icon_list.size(); i++)
    {
        /* Set colors */
        icon_list[i].color = icon_color_list[i];

        /* Set tags */
        icon_list[i].tag_up = icon_tag_list[i * 2];
        icon_list[i].tag_down = icon_tag_list[i * 2 + 1];

        /* Push Icon pic into sprite */
        icon_sprite_list[i].createSprite(42, 42);
        icon_sprite_list[i].pushImage(0, 0, 42, 42, icon_pic_list[i]);
    }

    /* Icon position */
    int a = 120;
    int b = 120;
    int r = 190 / 2 ;
    int n = ICON_NUM; // Number of icons

    // Angles for 4 icons: left side (140°, 180°, 220°) + right side (0° = 3 o'clock)
    double angles[] = {7.0 * 3.14159 / 9.0,    // 140° (upper-left)
                       3.14159,                 // 180° (9 o'clock)
                       11.0 * 3.14159 / 9.0,    // 220° (lower-left)
                       0.0};                    // 0° (3 o'clock) - Settings

    /* Set icon position */
    int x;
    int y;
    for (int i = 0; i < icon_list.size(); i++)
    {
        x = a + r * std::cos(angles[i]);
        y = b + r * std::sin(angles[i]);

        icon_list[i].x = x;
        icon_list[i].y = y;
    }
}


void Launcher::_launcher_init()
{
    _menu_init();
    _icon_list_init();
}


void Launcher::_launcher_loop()
{
    _data.menu->update(millis());
    _canvas_update();
    // delay(5);

    /* Check for RFID tags */
    _check_rfid();

    /* If scrolled */
    if (_data.hal->encoder.wasMoved(true))
    {
        // printf("%d\n", _data.hal->encoder.getPosition());
        if (_data.hal->encoder.getDirection() < 1)
            _data.menu->goNext();
        else 
            _data.menu->goLast();
    }

    /* If button pressed */
    if (!_data.hal->encoder.btn.read())
    {
        _data.menu->getSelector()->pressed();

        /* Hold until button release */
        while (!_data.hal->encoder.btn.read())
        {
            _data.menu->update(millis());
            _canvas_update();
        }

        _data.menu->getSelector()->released();

        // /* Hold until anim finish */
        // while (!_data.menu->getSelector()->isAnimFinished())
        // {
        //     _data.menu->update(millis());
        //     _canvas_update();
        // }

        /* App open callback */
        _app_open_callback(_data.menu->getSelector()->getTargetItem());
    }

    /* If touched */
    if (_data.hal->tp.isTouched())
    {
        _data.hal->tp.update();

        // printf("%d %d\n", _data.hal->tp.getTouchPointBuffer().x, _data.hal->tp.getTouchPointBuffer().y);
        // return;

        /* Check if in the center circle (r = 50) */
        int x = _data.hal->tp.getTouchPointBuffer().x - 120;
        int y = _data.hal->tp.getTouchPointBuffer().y - 120;
        if ((x * x + y * y) > (50 * 50))
        {
            // printf("no\n");
            return;
        }
        // printf("yes\n");

        /* Call button pressed callback */
        HAL::HAL::_encoder_button_pressed_callback(nullptr, _data.hal);


        if (_data.menu->getSelector()->isAnimFinished())
        {
            /* App open callback */
            _app_open_callback(_data.menu->getSelector()->getTargetItem());

            /* Wait until released */
            while (_data.hal->tp.isTouched())
            {
                _data.menu->update(millis());
                _canvas_update();
            }
        }
    }
}


void Launcher::_app_open_callback(uint8_t selectedNum)
{
    _log("selected %d", selectedNum);

    /* If not in icon list */
    if (selectedNum >= icon_list.size())
    {
        return;
    }


    /* Get theme color */
    uint32_t theme_color = icon_list[selectedNum].color;


    /* Play app open anim */
    for (int i = 0; i < 10; i++)
    {
        _data.hal->canvas->fillSmoothCircle(icon_list[selectedNum].x, icon_list[selectedNum].y, i * 24, theme_color);
        _canvas_update();
    }

    

    // /* ----------------------- Simple test ----------------------- */
    // std::array<GUI_Base*, ICON_NUM> gui_list;
    // gui_list[0] = new GUI_Base;
    // gui_list[1] = new GUI_Base;
    // gui_list[2] = new GUI_Base;
    // gui_list[3] = new GUI_Base;
    // gui_list[4] = new GUI_Base;
    // gui_list[5] = new GUI_Base;
    // gui_list[6] = new GUI_Base;
    // gui_list[7] = new GUI_Base;

    // /* Open app */
    // gui_list[selectedNum]->setThemeColor(icon_list[selectedNum].color);
    // gui_list[selectedNum]->init(_data.hal->canvas, &icon_sprite_list[selectedNum]);
    // while (1)
    // {
    //     if (_data.hal->encoder.btn.pressed())
    //     {
    //         /* Hold until button release */
    //         while (!_data.hal->encoder.btn.read());
    //         break;
    //     }
    // }
    // /* ----------------------- Simple test ----------------------- */






    // /* ----------------------- Simple app test ----------------------- */
    // std::array<MOONCAKE::APP_BASE*, ICON_NUM> app_list;
    // app_list[0] = new MOONCAKE::USER_APP::LCD_Test;
    // app_list[1] = new MOONCAKE::USER_APP::RTC_Test;
    // app_list[2] = new MOONCAKE::USER_APP::RFID_Test;
    // app_list[3] = new MOONCAKE::USER_APP::Set_Brightness;
    // app_list[4] = new MOONCAKE::USER_APP::WiFi_Scan;
    // app_list[5] = new MOONCAKE::APP_BASE;
    // app_list[6] = new MOONCAKE::APP_BASE;
    // app_list[7] = new MOONCAKE::USER_APP::VideoShit;

    // if (app_list[selectedNum]->getGui() != nullptr)
    // {
    //     app_list[selectedNum]->getGui()->setThemeColor(icon_list[selectedNum].color);
    //     app_list[selectedNum]->getGui()->init(_data.hal->canvas, &icon_sprite_list[selectedNum]);

    //     _simple_app_manager(app_list[selectedNum]);
    // }

    // /* Free */
    // for (auto& i : app_list)
    // {
    //     delete i;
    // }
    // /* ----------------------- Simple app test ----------------------- */




    /* Memery leak check */
    size_t mem_before_open = heap_caps_get_free_size(MALLOC_CAP_INTERNAL);
    _log("free mem: %d", mem_before_open);



    MOONCAKE::APP_BASE* app_ptr = nullptr;

    /* Create app */
    switch (selectedNum)
    {
        case 0:
            app_ptr = new MOONCAKE::USER_APP::BeehiveInspection;
            break;
        case 1:
            app_ptr = new MOONCAKE::USER_APP::YardManagement;
            break;
        case 2:
            app_ptr = new MOONCAKE::USER_APP::BleDataTransfer;
            break;
        case 3:
            app_ptr = new MOONCAKE::USER_APP::Settings;
            break;
        default:
            break;
    };

    /* If app created */
    if (app_ptr != nullptr)
    {
        /* Init if gui module exsit */
        if (app_ptr->getGui() != nullptr)
        {
            app_ptr->getGui()->setThemeColor(icon_list[selectedNum].color);
            app_ptr->getGui()->init(_data.hal->canvas, &icon_sprite_list[selectedNum]);
        }

        /* Run app */
        _simple_app_manager(app_ptr);

        /* Free app */
        delete app_ptr;
    }

    /* Check if we should auto-launch beehive inspection */
    while (YARD_MANAGEMENT::shouldLaunchInspection())
    {
        _log("Auto-launching beehive inspection");

        // Create and run beehive inspection app
        MOONCAKE::APP_BASE* inspection_app = new MOONCAKE::USER_APP::BeehiveInspection;
        if (inspection_app != nullptr)
        {
            if (inspection_app->getGui() != nullptr)
            {
                inspection_app->getGui()->setThemeColor(icon_list[0].color);  // Use beehive icon color
                inspection_app->getGui()->init(_data.hal->canvas, &icon_sprite_list[0]);
            }

            _simple_app_manager(inspection_app);
            delete inspection_app;
        }
    }

    /* Memery leak check */
    _log_mem();
    if (heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < mem_before_open)
    {
        _log_e("memory leak: %d", mem_before_open - heap_caps_get_free_size(MALLOC_CAP_INTERNAL));
    }
    
    

    /* Play app close anim */
    for (int i = 10; i > 1; i--)
    {
        _data.menu->update(millis());
        _data.hal->canvas->fillSmoothCircle(icon_list[selectedNum].x, icon_list[selectedNum].y, i * 24, theme_color);
        _canvas_update();
    }
}


void Launcher::_simple_app_manager(MOONCAKE::APP_BASE* app)
{
    app->setUserData((void*)_data.hal);
    app->onSetup();
    app->onCreate();
    while (1)
    {
        app->onRunning();
        if (app->isGoingDestroy())
        {
            app->resetGoingDestroyFlag();
            app->onDestroy();
            break;
        }

        // if (_data.hal->encoder.btn.pressed())
        // {
        //     /* Hold until button release */
        //     while (!_data.hal->encoder.btn.read());
        //     break;
        // }
    }
}


Launcher::~Launcher()
{
    delete _data.menu;
    delete _data.menu_render_cb;
}


void Launcher::_check_rfid()
{
    // Check if RFID reader detected a tag
    if (!_data.hal->rfid.isReady() || !_data.hal->rfid.wasTagDetected()) {
        return;
    }

    // Get the tag string
    char tagId[17];
    if (!_data.hal->rfid.getLastTagString(tagId, sizeof(tagId))) {
        return;
    }

    _log("RFID tag detected: %s", tagId);

    // First check if it's a hive tag
    uint32_t hiveNumber = YARD_MANAGEMENT::findHiveByRfid(tagId);
    if (hiveNumber != 0) {
        _log("Found hive %lu", (unsigned long)hiveNumber);
        _launch_hive_inspection(hiveNumber);
        return;
    }

    // Then check if it's a yard tag
    uint32_t yardNumber = YARD_MANAGEMENT::findYardByRfid(tagId);
    if (yardNumber != 0) {
        _log("Found yard %lu", (unsigned long)yardNumber);
        _show_yard_info(yardNumber);
        return;
    }

    // Unknown tag - show a brief message
    _log("Unknown RFID tag");
    _data.hal->buzz.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(50));
    _data.hal->buzz.tone(2000, 50);
}


void Launcher::_show_yard_info(uint32_t yardNumber)
{
    // Load yard data
    YARD_MANAGEMENT::YardRecord yard;
    if (!YARD_MANAGEMENT::loadYard(yardNumber, yard)) {
        _log("Failed to load yard %lu", (unsigned long)yardNumber);
        return;
    }

    // Buzz to indicate success
    _data.hal->buzz.tone(2000, 100);

    // Calculate stats
    size_t activeHives = YARD_MANAGEMENT::getActiveHiveCountInYard(yardNumber);

    // Display yard info overlay
    LGFX_Sprite* canvas = _data.hal->canvas;

    // Draw semi-transparent overlay
    canvas->fillScreen(0x000000);  // Black background

    // Draw yard info
    canvas->setTextColor(0xFFFFFF);  // White text
    canvas->setFont(&fonts::FreeSansBold18pt7b);
    canvas->setTextDatum(textdatum_t::top_center);
    canvas->drawString(yard.nickname, 120, 40);

    canvas->setFont(&fonts::FreeSans12pt7b);
    canvas->setTextDatum(textdatum_t::middle_center);

    char buf[64];
    snprintf(buf, sizeof(buf), "Yard #%lu", (unsigned long)yardNumber);
    canvas->drawString(buf, 120, 100);

    snprintf(buf, sizeof(buf), "Active Hives: %d", (int)activeHives);
    canvas->drawString(buf, 120, 140);

    // Show status
    canvas->setFont(&fonts::FreeSans9pt7b);
    canvas->setTextColor(yard.isActive() ? 0x00FF00 : 0xFF0000);
    canvas->drawString(yard.isActive() ? "ACTIVE" : "CLOSED", 120, 180);

    // Hint at bottom
    canvas->setTextColor(0x888888);
    canvas->setTextDatum(textdatum_t::bottom_center);
    canvas->drawString("tap to dismiss", 120, 220);

    canvas->pushSprite(0, 0);

    // Wait for button press or timeout
    uint32_t startTime = millis();
    while (millis() - startTime < 5000) {  // 5 second timeout
        if (!_data.hal->encoder.btn.read()) {
            // Wait for release
            while (!_data.hal->encoder.btn.read()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            break;
        }
        if (_data.hal->tp.isTouched()) {
            // Wait for release
            while (_data.hal->tp.isTouched()) {
                vTaskDelay(pdMS_TO_TICKS(10));
            }
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(50));
    }

    // Clear RFID detection to avoid re-triggering
    _data.hal->rfid.clearLastTag();
}


void Launcher::_launch_hive_inspection(uint32_t hiveNumber)
{
    // Buzz to indicate success
    _data.hal->buzz.tone(2000, 100);

    // Store the selected hive for the inspection app
    YARD_MANAGEMENT::storeSelectedHiveForInspection(hiveNumber);

    // Clear RFID detection
    _data.hal->rfid.clearLastTag();

    // Play app open anim (using beehive icon - index 0)
    uint32_t theme_color = icon_list[0].color;
    for (int i = 0; i < 10; i++)
    {
        _data.hal->canvas->fillSmoothCircle(icon_list[0].x, icon_list[0].y, i * 24, theme_color);
        _canvas_update();
    }

    // Create and run beehive inspection app
    MOONCAKE::APP_BASE* app = new MOONCAKE::USER_APP::BeehiveInspection;
    if (app != nullptr)
    {
        if (app->getGui() != nullptr)
        {
            app->getGui()->setThemeColor(icon_list[0].color);
            app->getGui()->init(_data.hal->canvas, &icon_sprite_list[0]);
        }

        _simple_app_manager(app);
        delete app;
    }

    // Check if we should auto-launch another inspection (chain from yard management)
    while (YARD_MANAGEMENT::shouldLaunchInspection())
    {
        _log("Auto-launching beehive inspection");

        MOONCAKE::APP_BASE* inspection_app = new MOONCAKE::USER_APP::BeehiveInspection;
        if (inspection_app != nullptr)
        {
            if (inspection_app->getGui() != nullptr)
            {
                inspection_app->getGui()->setThemeColor(icon_list[0].color);
                inspection_app->getGui()->init(_data.hal->canvas, &icon_sprite_list[0]);
            }

            _simple_app_manager(inspection_app);
            delete inspection_app;
        }
    }

    // Play app close anim
    for (int i = 10; i > 1; i--)
    {
        _data.menu->update(millis());
        _data.hal->canvas->fillSmoothCircle(icon_list[0].x, icon_list[0].y, i * 24, theme_color);
        _canvas_update();
    }
}


void Launcher::onSetup()
{
    setAppName("Launcher");
    setAllowBgRunning(false);

    /* Init with default value */
    LAUNCHER::Data_t default_data;
    _data = default_data;

    _data.hal = (HAL::HAL*)getUserData();
}


/* Life cycle */
void Launcher::onCreate()
{
    _log("onCreate");
    
    _launcher_init();
    _data.menu->getSelector()->reset(millis());
}


void Launcher::onRunning()
{
    _launcher_loop();
}

