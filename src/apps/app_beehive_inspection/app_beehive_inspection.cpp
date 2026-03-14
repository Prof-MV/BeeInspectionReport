/**
 * @file app_beehive_inspection.cpp
 * @brief Beehive Inspection App - Main app implementation
 * @version 1.0
 * @date 2026-02-08
 */
#include "app_beehive_inspection.h"
#include "../common_define.h"
#include "inspection_storage.h"
#include "../app_yard_management/yard_storage.h"
#include <esp_timer.h>
#include <cstring>

// Include all screen headers
#include "screens/screen_select_yard_hive.h"
#include "screens/screen_queen_right.h"
#include "screens/screen_queen_cells.h"
#include "screens/screen_supers.h"
#include "screens/screen_temperament.h"
#include "screens/screen_brood_size.h"
#include "screens/screen_treatment.h"
#include "screens/screen_pests.h"

using namespace MOONCAKE::USER_APP;
using namespace BEEHIVE_INSPECTION;

// Use display constants from screen_base.h

void BeehiveInspection::onSetup() {
    setAppName("Beehive");
    setAllowBgRunning(false);

    // Initialize data with defaults
    BEEHIVE_INSPECTION_NS::Data_t default_data;
    _data = default_data;
    _data.hal = (HAL::HAL*)getUserData();

    // Initialize NVS storage for inspection records
    if (!initNVSStorage()) {
        _log_e("Failed to initialize NVS storage");
    }
}

void BeehiveInspection::onCreate() {
    _log("onCreate");

    // Reset inspection record and context
    _record.reset();
    _context.reset();
    _inSelectionMode = false;

    // Try to load pre-selected hive from yard management
    uint32_t selectedHive = YARD_MANAGEMENT::getSelectedHiveForInspection();
    if (selectedHive != 0) {
        _log("Found pre-selected hive: %lu", static_cast<unsigned long>(selectedHive));

        // Load hive data to get yard number
        YARD_MANAGEMENT::HiveRecord hive;
        if (YARD_MANAGEMENT::loadHive(selectedHive, hive)) {
            _context.hiveNumber = selectedHive;
            _context.yardNumber = hive.yardNumber;

            // Load yard to get nickname
            YARD_MANAGEMENT::YardRecord yard;
            if (YARD_MANAGEMENT::loadYard(hive.yardNumber, yard)) {
                strncpy(_context.yardNickname, yard.nickname, sizeof(_context.yardNickname) - 1);
                _context.yardNickname[sizeof(_context.yardNickname) - 1] = '\0';
            }

            _log("Context loaded: Yard %lu (%s), Hive %lu",
                 static_cast<unsigned long>(_context.yardNumber),
                 _context.yardNickname,
                 static_cast<unsigned long>(_context.hiveNumber));

            // Set hive ID in record for storage
            _record.hiveId = selectedHive;
        }

        // Clear the selection after reading
        YARD_MANAGEMENT::clearSelectedHiveForInspection();
    }

    // Create all screens
    createScreens();

    // Check if we need to show selection screen first
    if (!_context.isValid()) {
        // No hive pre-selected, show selection screen
        LGFX_Sprite* canvas = _gui.getCanvas();
        _selectionScreen = new ScreenSelectYardHive(_data.hal, canvas, &_record, &_context);
        _selectionScreen->setThemeColor(_gui.getThemeColor());
        _inSelectionMode = true;
        _selectionScreen->onEnter();
    } else {
        // Start at first inspection screen
        _data.currentScreenIndex = 0;
        _screens[0]->onEnter();
    }
}

void BeehiveInspection::onRunning() {
    handleEncoder();
    handleButton();
}

void BeehiveInspection::onDestroy() {
    _log("onDestroy");
    destroyScreens();
}

void BeehiveInspection::createScreens() {
    LGFX_Sprite* canvas = _gui.getCanvas();

    _screens[SCREEN_QUEEN_RIGHT] = new ScreenQueenRight(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_QUEEN_CELLS] = new ScreenQueenCells(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_SUPERS] = new ScreenSupers(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_TEMPERAMENT] = new ScreenTemperament(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_BROOD_SIZE] = new ScreenBroodSize(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_TREATMENT] = new ScreenTreatment(_data.hal, canvas, &_record, &_context);
    _screens[SCREEN_PESTS] = new ScreenPests(_data.hal, canvas, &_record, &_context);

    // Set theme color for all screens
    for (int i = 0; i < SCREEN_COUNT; i++) {
        if (_screens[i]) {
            _screens[i]->setThemeColor(_gui.getThemeColor());
        }
    }
}

void BeehiveInspection::destroyScreens() {
    // Clean up selection screen
    if (_selectionScreen) {
        delete _selectionScreen;
        _selectionScreen = nullptr;
    }

    // Clean up inspection screens
    for (int i = 0; i < SCREEN_COUNT; i++) {
        if (_screens[i]) {
            delete _screens[i];
            _screens[i] = nullptr;
        }
    }
}

void BeehiveInspection::handleEncoder() {
    if (_data.hal->encoder.wasMoved(true)) {
        // Determine direction: -1 for CCW, +1 for CW
        int direction = (_data.hal->encoder.getDirection() < 1) ? 1 : -1;

        if (_inSelectionMode && _selectionScreen) {
            _selectionScreen->onRotate(direction);
        } else if (_screens[_data.currentScreenIndex]) {
            _screens[_data.currentScreenIndex]->onRotate(direction);
        }
    }
}

void BeehiveInspection::handleButton() {
    bool isPressed = !_data.hal->encoder.btn.read();
    uint32_t now = millis();

    if (isPressed) {
        if (_data.buttonPressStart == 0) {
            // Button just pressed
            _data.buttonPressStart = now;
            _data.isLongPressHandled = false;
            _data.waitingForDoubleClick = false;
        }

        // Check for long press (>800ms) - only for inspection screens, not selection
        uint32_t pressDuration = now - _data.buttonPressStart;
        if (!_inSelectionMode && pressDuration > BEEHIVE_INSPECTION_NS::LONG_PRESS_MS && !_data.isLongPressHandled) {
            _data.isLongPressHandled = true;
            saveAndExit();
            return;
        }

        // Show long press progress indicator (only for inspection screens)
        if (!_inSelectionMode && pressDuration > 100 && !_data.isLongPressHandled) {
            float progress = (float)(pressDuration - 100) / (BEEHIVE_INSPECTION_NS::LONG_PRESS_MS - 100);
            if (progress > 1.0f) progress = 1.0f;
            drawLongPressProgress(progress);
        }
    } else {
        // Button released
        if (_data.buttonPressStart > 0) {
            uint32_t pressDuration = now - _data.buttonPressStart;

            if (pressDuration < BEEHIVE_INSPECTION_NS::LONG_PRESS_MS && !_data.isLongPressHandled) {
                // Short press detected
                if (_data.waitingForDoubleClick &&
                    (now - _data.lastReleaseTime) < BEEHIVE_INSPECTION_NS::DOUBLE_CLICK_WINDOW_MS) {
                    // Double click detected
                    _data.waitingForDoubleClick = false;

                    if (_inSelectionMode && _selectionScreen) {
                        if (!_selectionScreen->onBack()) {
                            // Exit app if back not handled
                            destroyApp();
                            return;
                        }
                    } else if (_screens[_data.currentScreenIndex]) {
                        if (!_screens[_data.currentScreenIndex]->onBack()) {
                            goToPreviousScreen();
                        }
                    }
                } else {
                    // First click - wait for potential double click
                    _data.waitingForDoubleClick = true;
                    _data.lastReleaseTime = now;
                }
            }

            _data.buttonPressStart = 0;

            // Redraw screen to remove long press indicator
            if (_inSelectionMode && _selectionScreen) {
                _selectionScreen->render();
            } else if (_screens[_data.currentScreenIndex]) {
                _screens[_data.currentScreenIndex]->render();
            }
        }
    }

    // Handle single click timeout (no double click came)
    if (_data.waitingForDoubleClick &&
        (now - _data.lastReleaseTime) > BEEHIVE_INSPECTION_NS::DOUBLE_CLICK_WINDOW_MS) {
        _data.waitingForDoubleClick = false;

        if (_inSelectionMode && _selectionScreen) {
            // Single click in selection mode
            if (_selectionScreen->onConfirm()) {
                // Selection complete, transition to inspection screens
                _inSelectionMode = false;
                _log("Selection complete: Yard %lu (%s), Hive %lu",
                     static_cast<unsigned long>(_context.yardNumber),
                     _context.yardNickname,
                     static_cast<unsigned long>(_context.hiveNumber));

                // Clean up selection screen
                delete _selectionScreen;
                _selectionScreen = nullptr;

                // Start inspection
                _data.currentScreenIndex = 0;
                _screens[0]->onEnter();
            }
        } else if (_screens[_data.currentScreenIndex]) {
            if (_screens[_data.currentScreenIndex]->onConfirm()) {
                goToNextScreen();
            }
        }
    }
}

void BeehiveInspection::goToNextScreen() {
    if (_data.currentScreenIndex < SCREEN_COUNT - 1) {
        _screens[_data.currentScreenIndex]->onExit();
        _data.currentScreenIndex++;
        _screens[_data.currentScreenIndex]->onEnter();
    } else {
        // On last screen - finish inspection
        saveAndExit();
    }
}

void BeehiveInspection::goToPreviousScreen() {
    if (_data.currentScreenIndex > 0) {
        _screens[_data.currentScreenIndex]->onExit();
        _data.currentScreenIndex--;
        _screens[_data.currentScreenIndex]->onEnter();
    }
    // On first screen, double-click does nothing
}

void BeehiveInspection::saveAndExit() {
    _log("Saving inspection and exiting");

    // Save current screen's data
    if (_screens[_data.currentScreenIndex]) {
        _screens[_data.currentScreenIndex]->onExit();
    }

    // Set record metadata
    _record.recordId = getNextRecordID();
    _record.timestamp = (uint32_t)(esp_timer_get_time() / 1000000);  // Unix timestamp in seconds
    _record.isComplete = (_data.currentScreenIndex == SCREEN_COUNT - 1);

    // Log the inspection data
    _log("Saving record ID: %lu", (unsigned long)_record.recordId);
    _log("Queen Right: %s", _record.queenRight ? "Yes" : "No");
    _log("Supersedure: %d, Swarm: %d",
         static_cast<int>(_record.supersedureCells),
         static_cast<int>(_record.swarmCells));
    _log("Supers: %d", _record.superCount);
    _log("Temperament: %s", getTemperamentString(_record.temperament));
    _log("Brood Frames: %d", _record.broodFrames);
    _log("Treatment: %s", getTreatmentString(_record.treatment));
    _log("Pests: 0x%02X", _record.pests);

    // Persist to NVS
    if (saveInspectionToNVS(_record)) {
        _log("Inspection saved to NVS successfully");
        // Double beep to confirm save (use consistent frequency)
        _data.hal->buzz.tone(2000, 100);
        delay(50);
        _data.hal->buzz.tone(2000, 150);
    } else {
        _log_e("Failed to save inspection to NVS");
        // Error beep pattern (use consistent frequency)
        _data.hal->buzz.tone(2000, 50);
        delay(50);
        _data.hal->buzz.tone(2000, 50);
        delay(50);
        _data.hal->buzz.tone(2000, 50);
    }

    destroyApp();
}

void BeehiveInspection::drawLongPressProgress(float progress) {
    LGFX_Sprite* canvas = _gui.getCanvas();

    // Draw a progress arc around the edge of the visible circular display area
    int centerX = DISPLAY_CENTER_X;
    int centerY = DISPLAY_CENTER_Y;
    int radius = VISIBLE_RADIUS - 3;  // Stay within visible circular area
    int thickness = 6;

    // Calculate arc angle based on progress
    int endAngle = (int)(progress * 360);

    // Draw arc
    for (int angle = 0; angle < endAngle; angle += 3) {
        float rad = angle * 3.14159f / 180.0f;
        int x = centerX + (int)(radius * sin(rad));
        int y = centerY - (int)(radius * cos(rad));
        canvas->fillCircle(x, y, thickness / 2, COLOR_SUCCESS);
    }

    canvas->pushSprite(0, 0);
}
