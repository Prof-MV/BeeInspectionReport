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
    , _currentDigit(0)
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
    _currentDigit = 0;
    _cursorPos = 0;
    _selectedChar = 'A';
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    _numberValid = true;
    _context->newYardNumber = MIN_YARD_NUMBER;
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
    drawSubheader("Enter 6-digit number");

    drawSixDigitInput(_context->newYardNumber, _currentDigit, 85);

    // Show validation status
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);

    if (!_numberValid) {
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->drawString("Number exists!", DISPLAY_CENTER_X, 150);
    } else {
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->drawString("Click to advance digit", DISPLAY_CENTER_X, 150);
    }

    // Show navigation hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    _canvas->drawString("Dbl-click: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
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
    _canvas->drawString("Long-click: Done", DISPLAY_CENTER_X, CONTENT_BOTTOM);
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
        _canvas->drawString("Click to save", DISPLAY_CENTER_X, CONTENT_BOTTOM);
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
        _canvas->drawString("Present tag to reader", DISPLAY_CENTER_X, CONTENT_BOTTOM);
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
}

void ScreenCreateYard::simulateRfidScan() {
    // Generate a mock RFID tag for testing
    // In production, this would read from actual RFID hardware
    uint32_t mockId = static_cast<uint32_t>(esp_timer_get_time() & 0xFFFFFFFF);
    snprintf(_scannedTag, sizeof(_scannedTag), "%08lX%08lX",
             static_cast<unsigned long>(mockId),
             static_cast<unsigned long>(mockId ^ 0xA5A5A5A5));

    // Check uniqueness
    if (isRfidUnique(_scannedTag, true, true)) {
        _scanSuccess = true;
        _scanError = false;
    } else {
        _scanSuccess = false;
        _scanError = true;
        strcpy(_errorMessage, "Tag already in use");
    }

    _scanning = false;
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
            _context->newYardNumber = adjustDigit(_context->newYardNumber, _currentDigit, direction);
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
            // Move to next digit
            _currentDigit++;
            if (_currentDigit >= NUM_DIGITS) {
                // All digits entered, validate and move to nickname
                if (validateNumber()) {
                    _currentStep = CreateYardStep::NICKNAME;
                    _currentDigit = 0;
                    buzzConfirm();
                } else {
                    _currentDigit = 0;  // Reset to first digit
                    buzzError();
                }
            } else {
                buzzNavigate();
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
                // Retry scan
                startRfidScan();
                simulateRfidScan();  // Replace with real scan
                buzzNavigate();
                render();
            } else {
                // Start/continue scan
                simulateRfidScan();  // Replace with real scan
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
            if (_currentDigit > 0) {
                _currentDigit--;
                render();
            } else {
                result.nextScreen = ScreenType::MGMT_MENU;
            }
            break;

        case CreateYardStep::NICKNAME:
            if (_cursorPos > 0) {
                // Delete last character
                _cursorPos--;
                _nickname[_cursorPos] = '\0';
                render();
            } else if (strlen(_nickname) > 0) {
                // Nickname entered, move to RFID
                _context->yardNickname[0] = '\0';
                strncpy(_context->yardNickname, _nickname, MAX_NICKNAME_LENGTH);
                _currentStep = CreateYardStep::RFID_SCAN;
                startRfidScan();
                render();
            } else {
                // Go back to number step
                _currentStep = CreateYardStep::NUMBER;
                _currentDigit = NUM_DIGITS - 1;
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

} // namespace YARD_MANAGEMENT
