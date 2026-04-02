/**
 * @file app_yard_management.cpp
 * @brief Yard Management App - Main app implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "app_yard_management.h"
#include "../common_define.h"
#include "yard_storage.h"

// Include all screen headers
#include "screens/screen_yard_list.h"
#include "screens/screen_mgmt_menu.h"
#include "screens/screen_create_yard.h"
#include "screens/screen_close_yard.h"
#include "screens/screen_yard_summary.h"
#include "screens/screen_yard_actions.h"
#include "screens/screen_hive_mgmt.h"
#include "screens/screen_add_hive.h"
#include "screens/screen_close_hive.h"
#include "screens/screen_select_hive.h"
#include "screens/screen_equipment_plan.h"
#include "screens/screen_clear_data.h"
#include "screens/screen_edit_yard.h"

using namespace MOONCAKE::USER_APP;
using namespace YARD_MANAGEMENT;

void YardManagement::onSetup() {
    setAppName("YardMgmt");
    setAllowBgRunning(false);

    // Initialize data with defaults
    YARD_MANAGEMENT_NS::Data_t default_data;
    _data = default_data;
    _data.hal = (HAL::HAL*)getUserData();

    // Initialize NVS storage for yard management
    if (!initYardManagementNVS()) {
        _log_e("Failed to initialize NVS storage");
    }
}

void YardManagement::onCreate() {
    _log("onCreate");

    // Reset context
    _context = AppContext();

    // Start at yard list screen
    _data.currentScreen = ScreenType::YARD_LIST;
    transitionToScreen(ScreenType::YARD_LIST);
}

void YardManagement::onRunning() {
    handleEncoder();
    handleButton();
    handleScreenUpdate();
}

void YardManagement::handleScreenUpdate() {
    if (_currentScreenPtr) {
        NavigationResult result = _currentScreenPtr->onUpdate();

        if (result.launchInspection) {
            // Set flag for launcher to auto-launch beehive inspection
            setLaunchInspectionFlag(true);
        }

        if (result.exitApp) {
            destroyApp();
        } else if (result.nextScreen != ScreenType::NONE) {
            transitionToScreen(result.nextScreen);
        }
    }
}

void YardManagement::onDestroy() {
    _log("onDestroy");
    destroyCurrentScreen();
}

ScreenBaseYM* YardManagement::createScreen(ScreenType type) {
    LGFX_Sprite* canvas = _gui.getCanvas();

    switch (type) {
        case ScreenType::YARD_LIST:
            return new ScreenYardList(_data.hal, canvas, &_context);

        case ScreenType::MGMT_MENU:
            return new ScreenMgmtMenu(_data.hal, canvas, &_context);

        case ScreenType::CREATE_YARD:
            return new ScreenCreateYard(_data.hal, canvas, &_context);

        case ScreenType::CLOSE_YARD:
            return new ScreenCloseYard(_data.hal, canvas, &_context);

        case ScreenType::YARD_SUMMARY:
            return new ScreenYardSummary(_data.hal, canvas, &_context);

        case ScreenType::YARD_ACTIONS:
            return new ScreenYardActions(_data.hal, canvas, &_context);

        case ScreenType::HIVE_MGMT:
            return new ScreenHiveMgmt(_data.hal, canvas, &_context);

        case ScreenType::ADD_HIVE:
            return new ScreenAddHive(_data.hal, canvas, &_context);

        case ScreenType::CLOSE_HIVE:
            return new ScreenCloseHive(_data.hal, canvas, &_context);

        case ScreenType::SELECT_HIVE:
            return new ScreenSelectHive(_data.hal, canvas, &_context);

        case ScreenType::EQUIPMENT_PLAN:
            return new ScreenEquipmentPlan(_data.hal, canvas, &_context);

        case ScreenType::CLEAR_DATA:
            return new ScreenClearData(_data.hal, canvas, &_context);

        case ScreenType::EDIT_YARD:
            return new ScreenEditYard(_data.hal, canvas, &_context);

        default:
            return nullptr;
    }
}

void YardManagement::destroyCurrentScreen() {
    if (_currentScreenPtr) {
        _currentScreenPtr->onExit();
        delete _currentScreenPtr;
        _currentScreenPtr = nullptr;
    }
}

void YardManagement::transitionToScreen(ScreenType newScreen) {
    _log("Transitioning to screen %d", static_cast<int>(newScreen));

    // Exit current screen
    if (_currentScreenPtr) {
        _currentScreenPtr->onExit();
        delete _currentScreenPtr;
        _currentScreenPtr = nullptr;
    }

    // Create and enter new screen
    _data.currentScreen = newScreen;
    _currentScreenPtr = createScreen(newScreen);

    if (_currentScreenPtr) {
        _currentScreenPtr->setThemeColor(_gui.getThemeColor());
        _currentScreenPtr->onEnter();
    }
}

void YardManagement::handleEncoder() {
    if (_data.hal->encoder.wasMoved(true)) {
        // Determine direction: -1 for CCW, +1 for CW
        int direction = (_data.hal->encoder.getDirection() < 1) ? 1 : -1;

        if (_currentScreenPtr) {
            _currentScreenPtr->onRotate(direction);
        }
    }
}

void YardManagement::handleButton() {
    bool isPressed = !_data.hal->encoder.btn.read();
    uint32_t now = millis();

    if (isPressed) {
        if (_data.buttonPressStart == 0) {
            // Button just pressed
            _data.buttonPressStart = now;
            _data.isLongPressHandled = false;
            _data.isBackHandled = false;
        }

        uint32_t pressDuration = now - _data.buttonPressStart;

        // Check for very long press (>1500ms) - exit app
        if (pressDuration > YARD_MANAGEMENT_NS::EXIT_PRESS_MS && !_data.isLongPressHandled) {
            _data.isLongPressHandled = true;
            saveAndExit();
            return;
        }

        // Check for medium press (>600ms) - go back
        if (pressDuration > YARD_MANAGEMENT_NS::BACK_PRESS_MS && !_data.isBackHandled) {
            _data.isBackHandled = true;

            // Trigger back action
            if (_currentScreenPtr) {
                NavigationResult result = _currentScreenPtr->onBack();

                if (result.exitApp) {
                    destroyApp();
                    return;
                } else if (result.nextScreen != ScreenType::NONE) {
                    transitionToScreen(result.nextScreen);
                } else {
                    // Back handled within screen, redraw
                    _currentScreenPtr->render();
                }
            }

            // Feedback beep for back action (use consistent frequency)
            _data.hal->buzz.tone(2000, 50);
        }

        // Show long press progress indicator
        if (pressDuration > 100) {
            // Show progress toward back action (0-100% during 100ms to 600ms)
            // Then show progress toward exit (100% at 600ms, filling second ring to 1500ms)
            if (pressDuration < YARD_MANAGEMENT_NS::BACK_PRESS_MS) {
                float progress = (float)(pressDuration - 100) / (YARD_MANAGEMENT_NS::BACK_PRESS_MS - 100);
                if (progress > 1.0f) progress = 1.0f;
                drawLongPressProgress(progress, false);  // Back progress (orange)
            } else if (!_data.isLongPressHandled) {
                float progress = (float)(pressDuration - YARD_MANAGEMENT_NS::BACK_PRESS_MS) /
                                (YARD_MANAGEMENT_NS::EXIT_PRESS_MS - YARD_MANAGEMENT_NS::BACK_PRESS_MS);
                if (progress > 1.0f) progress = 1.0f;
                drawLongPressProgress(progress, true);   // Exit progress (green)
            }
        }
    } else {
        // Button released
        if (_data.buttonPressStart > 0) {
            uint32_t pressDuration = now - _data.buttonPressStart;

            // Short press (< 600ms) - confirm/select action
            if (pressDuration < YARD_MANAGEMENT_NS::BACK_PRESS_MS && !_data.isBackHandled) {
                if (_currentScreenPtr) {
                    NavigationResult result = _currentScreenPtr->onConfirm();

                    if (result.launchInspection) {
                        // Set flag for launcher to auto-launch beehive inspection
                        setLaunchInspectionFlag(true);
                    }

                    if (result.exitApp) {
                        destroyApp();
                        return;
                    } else if (result.nextScreen != ScreenType::NONE) {
                        transitionToScreen(result.nextScreen);
                    }
                }
            }

            _data.buttonPressStart = 0;

            // Redraw screen to remove long press indicator
            if (_currentScreenPtr && !_data.isLongPressHandled) {
                _currentScreenPtr->render();
            }
        }
    }
}

void YardManagement::saveAndExit() {
    _log("Saving and exiting");

    // Exit current screen
    if (_currentScreenPtr) {
        _currentScreenPtr->onExit();
    }

    // Double beep to confirm (use consistent frequency)
    _data.hal->buzz.tone(2000, 100);
    delay(50);
    _data.hal->buzz.tone(2000, 150);

    destroyApp();
}

void YardManagement::drawLongPressProgress(float progress, bool isExit) {
    LGFX_Sprite* canvas = _gui.getCanvas();

    // Draw a progress arc around the edge of the visible circular display area
    int centerX = DISPLAY_CENTER_X;
    int centerY = DISPLAY_CENTER_Y;
    int radius = VISIBLE_RADIUS - 3;
    int thickness = 6;

    // Calculate arc angle based on progress
    int endAngle = (int)(progress * 360);

    // Use orange for back progress, green for exit progress
    uint16_t color = isExit ? COLOR_SUCCESS : COLOR_SELECTED;

    // If showing exit progress, first draw the completed back ring
    if (isExit) {
        for (int angle = 0; angle < 360; angle += 3) {
            float rad = angle * 3.14159f / 180.0f;
            int x = centerX + (int)(radius * sin(rad));
            int y = centerY - (int)(radius * cos(rad));
            canvas->fillCircle(x, y, thickness / 2, COLOR_SELECTED);
        }
        // Draw exit progress on inner ring
        radius -= 10;
    }

    // Draw progress arc
    for (int angle = 0; angle < endAngle; angle += 3) {
        float rad = angle * 3.14159f / 180.0f;
        int x = centerX + (int)(radius * sin(rad));
        int y = centerY - (int)(radius * cos(rad));
        canvas->fillCircle(x, y, thickness / 2, color);
    }

    canvas->pushSprite(0, 0);
}
