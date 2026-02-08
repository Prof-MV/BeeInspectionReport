/**
 * @file app_beehive_inspection.cpp
 * @brief Beehive Inspection App - Main app implementation
 * @version 1.0
 * @date 2026-02-08
 */
#include "app_beehive_inspection.h"
#include "../common_define.h"

// Include all screen headers
#include "screens/screen_queen_right.h"
#include "screens/screen_queen_cells.h"
#include "screens/screen_supers.h"
#include "screens/screen_temperament.h"
#include "screens/screen_brood_size.h"
#include "screens/screen_treatment.h"
#include "screens/screen_pests.h"

using namespace MOONCAKE::USER_APP;
using namespace BEEHIVE_INSPECTION;

void BeehiveInspection::onSetup() {
    setAppName("Beehive");
    setAllowBgRunning(false);

    // Initialize data with defaults
    BEEHIVE_INSPECTION_NS::Data_t default_data;
    _data = default_data;
    _data.hal = (HAL::HAL*)getUserData();
}

void BeehiveInspection::onCreate() {
    _log("onCreate");

    // Reset inspection record
    _record.reset();

    // Create all screens
    createScreens();

    // Start at first screen
    _data.currentScreenIndex = 0;
    _screens[0]->onEnter();
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

    _screens[SCREEN_QUEEN_RIGHT] = new ScreenQueenRight(_data.hal, canvas, &_record);
    _screens[SCREEN_QUEEN_CELLS] = new ScreenQueenCells(_data.hal, canvas, &_record);
    _screens[SCREEN_SUPERS] = new ScreenSupers(_data.hal, canvas, &_record);
    _screens[SCREEN_TEMPERAMENT] = new ScreenTemperament(_data.hal, canvas, &_record);
    _screens[SCREEN_BROOD_SIZE] = new ScreenBroodSize(_data.hal, canvas, &_record);
    _screens[SCREEN_TREATMENT] = new ScreenTreatment(_data.hal, canvas, &_record);
    _screens[SCREEN_PESTS] = new ScreenPests(_data.hal, canvas, &_record);

    // Set theme color for all screens
    for (int i = 0; i < SCREEN_COUNT; i++) {
        if (_screens[i]) {
            _screens[i]->setThemeColor(_gui.getThemeColor());
        }
    }
}

void BeehiveInspection::destroyScreens() {
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

        if (_screens[_data.currentScreenIndex]) {
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

        // Check for long press (>800ms)
        uint32_t pressDuration = now - _data.buttonPressStart;
        if (pressDuration > BEEHIVE_INSPECTION_NS::LONG_PRESS_MS && !_data.isLongPressHandled) {
            _data.isLongPressHandled = true;
            saveAndExit();
            return;
        }

        // Show long press progress indicator
        if (pressDuration > 100 && !_data.isLongPressHandled) {
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

                    // Try to handle back within screen first
                    if (_screens[_data.currentScreenIndex]) {
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
            if (_screens[_data.currentScreenIndex]) {
                _screens[_data.currentScreenIndex]->render();
            }
        }
    }

    // Handle single click timeout (no double click came)
    if (_data.waitingForDoubleClick &&
        (now - _data.lastReleaseTime) > BEEHIVE_INSPECTION_NS::DOUBLE_CLICK_WINDOW_MS) {
        _data.waitingForDoubleClick = false;

        // Single click - confirm and potentially advance
        if (_screens[_data.currentScreenIndex]) {
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

    // TODO: Persist _record to storage (NVS or file)
    // For now, just log the data
    _log("Queen Right: %s", _record.queenRight ? "Yes" : "No");
    _log("Supersedure: %d, Swarm: %d",
         static_cast<int>(_record.supersedureCells),
         static_cast<int>(_record.swarmCells));
    _log("Supers: %d", _record.superCount);
    _log("Temperament: %s", getTemperamentString(_record.temperament));
    _log("Brood Frames: %d", _record.broodFrames);
    _log("Treatment: %s", getTreatmentString(_record.treatment));
    _log("Pests: 0x%02X", _record.pests);

    // Double beep to confirm save
    _data.hal->buzz.tone(2000, 100);
    delay(50);
    _data.hal->buzz.tone(2500, 100);

    destroyApp();
}

void BeehiveInspection::drawLongPressProgress(float progress) {
    LGFX_Sprite* canvas = _gui.getCanvas();

    // Draw a progress arc around the edge of the display
    int centerX = 120;
    int centerY = 120;
    int radius = 115;
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
