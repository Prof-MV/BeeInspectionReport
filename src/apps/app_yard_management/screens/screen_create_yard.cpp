/**
 * @file screen_create_yard.cpp
 * @brief Screen 3: Create Yard implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_create_yard.h"
#include "../yard_storage.h"
#include <esp_timer.h>
#include <cstdio>
#include <cstring>
#include <cstdlib>

namespace YARD_MANAGEMENT {

ScreenCreateYard::ScreenCreateYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(CreateYardStep::NUMBER)
    , _cursorPos(0)
    , _selectedChar('A')
    , _scanning(false)
    , _scanSuccess(false)
    , _scanError(false)
    , _numberValid(true)
{
    memset(_nickname, 0, sizeof(_nickname));
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));
}

void ScreenCreateYard::onEnter() {
    _currentStep = CreateYardStep::NUMBER;
    _cursorPos = 0;
    _selectedChar = 'A';
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    _numberValid = true;
    _context->newYardNumber = 1;  // Start from 1 for counter-style input
    memset(_nickname, 0, sizeof(_nickname));
    memset(_scannedTag, 0, sizeof(_scannedTag));

    render();
}

void ScreenCreateYard::onExit() {
    // Nothing to save until RFID step completes
}

void ScreenCreateYard::render() {
    drawBackground();

    switch (_currentStep) {
        case CreateYardStep::NUMBER:
            renderNumberStep();
            break;
        case CreateYardStep::NICKNAME:
            renderNicknameStep();
            break;
        case CreateYardStep::RFID_SCAN:
            renderRfidStep();
            break;
        case CreateYardStep::COMPLETE:
            // Should navigate away before this
            break;
    }

    pushToDisplay();
}

void ScreenCreateYard::renderNumberStep() {
    drawHeader("CREATE YARD");
    drawSubheader("Enter yard number");

    drawCounterInput(_context->newYardNumber, 1, MAX_YARD_NUMBER, 80);

    // Show validation status
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::bottom_center);

    if (!_numberValid) {
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->drawString("Number exists!", DISPLAY_CENTER_X, CONTENT_BOTTOM - 20);
    }

    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->drawString("Click: Confirm", DISPLAY_CENTER_X, CONTENT_BOTTOM);
}

void ScreenCreateYard::renderNicknameStep() {
    drawHeader("CREATE YARD");

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Yard %06lu", static_cast<unsigned long>(_context->newYardNumber));
    drawSubheader(subheader);

    drawNicknameInput(_nickname, _cursorPos, _selectedChar, 90);

    // Show hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    if (strlen(_nickname) > 0) {
        _canvas->drawString("Hold: Next step", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        _canvas->drawString("Hold: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }
}

void ScreenCreateYard::renderRfidStep() {
    drawHeader("CREATE YARD");

    char subheader[48];
    snprintf(subheader, sizeof(subheader), "%06lu - %s",
             static_cast<unsigned long>(_context->newYardNumber), _nickname);
    drawSubheader(subheader);

    if (_scanSuccess) {
        drawRfidSuccess(_scannedTag);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_SUCCESS);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click: Save yard", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else if (_scanError) {
        drawRfidError(_errorMessage);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click to retry", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        drawRfidScanPrompt("Scan Gate Tag", _scanning);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click to cancel", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }
}

bool ScreenCreateYard::validateNumber() {
    _numberValid = isYardNumberUnique(_context->newYardNumber);
    return _numberValid;
}

void ScreenCreateYard::startRfidScan() {
    _scanning = true;
    _scanSuccess = false;
    _scanError = false;
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));

    // Clear any previous tag detection and start scanning
#if RFID_ENABLE
    if (_hal->rfid.isReady()) {
        _hal->rfid.clearLastTag();
        _hal->rfid.startScanning();
    }
#endif
}

void ScreenCreateYard::simulateRfidScan() {
#if RFID_ENABLE
    // Check if a tag was detected (non-blocking)
    if (!_hal->rfid.isReady()) {
        _scanSuccess = false;
        _scanError = true;
        strcpy(_errorMessage, "RFID not ready");
        _scanning = false;
        return;
    }

    // Check if tag was detected
    if (_hal->rfid.wasTagDetected()) {
        // Get the tag serial as hex string
        char tagBuffer[20] = {0};
        if (_hal->rfid.getLastTagString(tagBuffer, sizeof(tagBuffer))) {
            // Copy to scannedTag (truncate to RFID_TAG_LENGTH if needed)
            strncpy(_scannedTag, tagBuffer, RFID_TAG_LENGTH);
            _scannedTag[RFID_TAG_LENGTH] = '\0';

            // Check uniqueness
            if (isRfidUnique(_scannedTag, true, true)) {
                _scanSuccess = true;
                _scanError = false;
                _hal->buzz.tone(2000, 100);  // Success beep
            } else {
                _scanSuccess = false;
                _scanError = true;
                strcpy(_errorMessage, "Tag already in use");
            }
            _scanning = false;
        }
    }
    // If no tag detected yet, keep scanning (don't change state)
#else
    // RFID disabled
    _scanSuccess = false;
    _scanError = true;
    strcpy(_errorMessage, "RFID disabled");
    _scanning = false;
#endif
}

bool ScreenCreateYard::createYard() {
    YardRecord yard;
    yard.yardNumber = _context->newYardNumber;
    yard.setNickname(_nickname);
    yard.setRfidTag(_scannedTag);
    yard.creationTimestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    yard.closureTimestamp = 0;
    yard.status = static_cast<char>(YardStatus::ACTIVE);
    yard.activeHiveCount = 0;

    return saveYard(yard);
}

void ScreenCreateYard::onRotate(int direction) {
    switch (_currentStep) {
        case CreateYardStep::NUMBER:
            _context->newYardNumber = adjustCounter(_context->newYardNumber, direction, 1, MAX_YARD_NUMBER);
            _numberValid = true;  // Reset validation on change
            break;

        case CreateYardStep::NICKNAME:
            _selectedChar = getNextChar(_selectedChar, direction);
            break;

        case CreateYardStep::RFID_SCAN:
            // No rotation action during RFID scan
            break;

        default:
            break;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenCreateYard::onConfirm() {
    NavigationResult result;

    switch (_currentStep) {
        case CreateYardStep::NUMBER:
            // Validate and move to nickname step
            if (validateNumber()) {
                _currentStep = CreateYardStep::NICKNAME;
                buzzConfirm();
            } else {
                buzzError();
            }
            render();
            break;

        case CreateYardStep::NICKNAME:
            // Add character and move cursor
            if (_cursorPos < MAX_NICKNAME_LENGTH) {
                _nickname[_cursorPos] = _selectedChar;
                _cursorPos++;

                // Check if nickname is complete (user can click multiple times)
                // Long press will trigger onBack which we handle as "done with nickname"
                buzzNavigate();
            }
            render();
            break;

        case CreateYardStep::RFID_SCAN:
            if (_scanSuccess) {
                // Create the yard
                if (createYard()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_LIST;
                } else {
                    _scanError = true;
                    strcpy(_errorMessage, "Save failed");
                    _scanSuccess = false;
                    buzzError();
                    render();
                }
            } else if (_scanError) {
                // Retry - restart scanning
                startRfidScan();
                buzzNavigate();
                render();
            } else {
                // Click while scanning = abort, go back to nickname
                _currentStep = CreateYardStep::NICKNAME;
                _cursorPos = strlen(_nickname);
                buzzNavigate();
                render();
            }
            break;

        default:
            break;
    }

    return result;
}

NavigationResult ScreenCreateYard::onBack() {
    NavigationResult result;

    switch (_currentStep) {
        case CreateYardStep::NUMBER:
            result.nextScreen = ScreenType::MGMT_MENU;
            break;

        case CreateYardStep::NICKNAME:
            if (strlen(_nickname) > 0) {
                // Nickname has content, move to RFID step
                _context->yardNickname[0] = '\0';
                strncpy(_context->yardNickname, _nickname, MAX_NICKNAME_LENGTH);
                _currentStep = CreateYardStep::RFID_SCAN;
                startRfidScan();
                buzzConfirm();
                render();
            } else {
                // No nickname entered, go back to number step
                _currentStep = CreateYardStep::NUMBER;
                render();
            }
            break;

        case CreateYardStep::RFID_SCAN:
            // Go back to nickname
            _currentStep = CreateYardStep::NICKNAME;
            _cursorPos = strlen(_nickname);
            render();
            break;

        default:
            result.nextScreen = ScreenType::MGMT_MENU;
            break;
    }

    return result;
}

NavigationResult ScreenCreateYard::onUpdate() {
    NavigationResult result;

    // Only poll for RFID when on the RFID scan step and actively scanning
    if (_currentStep == CreateYardStep::RFID_SCAN && _scanning && !_scanSuccess && !_scanError) {
        // Check for tag detection
        simulateRfidScan();

        // If scan completed (success or error), re-render
        if (_scanSuccess || _scanError) {
            render();

            // If success, auto-advance to save the yard
            if (_scanSuccess) {
                if (createYard()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_LIST;
                } else {
                    _scanError = true;
                    strcpy(_errorMessage, "Save failed");
                    _scanSuccess = false;
                    buzzError();
                    render();
                }
            }
        }
    }

    return result;
}

} // namespace YARD_MANAGEMENT
