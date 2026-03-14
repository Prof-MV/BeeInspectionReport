/**
 * @file screen_close_hive.cpp
 * @brief Screen 8: Close Hive implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_close_hive.h"
#include <esp_timer.h>
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

constexpr const char* ScreenCloseHive::REASONS[REASON_COUNT];
constexpr char ScreenCloseHive::REASON_CODES[REASON_COUNT];

ScreenCloseHive::ScreenCloseHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(CloseHiveStep::SELECT)
    , _selectedIndex(0)
    , _scrollOffset(0)
    , _reasonSelectedIndex(0)
    , _scanning(false)
    , _scanSuccess(false)
    , _scanError(false)
    , _hiveToClose(0)
{
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_expectedTag, 0, sizeof(_expectedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));
}

ScreenCloseHive::~ScreenCloseHive() {
    freeItemStrings();
}

void ScreenCloseHive::freeItemStrings() {
    for (char* str : _itemStrings) {
        delete[] str;
    }
    _itemStrings.clear();
    _displayItems.clear();
}

void ScreenCloseHive::loadHiveList() {
    freeItemStrings();
    _hiveNumbers.clear();

    // Get active hives in this yard
    _hiveNumbers = getActiveHivesInYard(_context->selectedYardNumber);

    // Create display strings
    for (uint32_t hiveNum : _hiveNumbers) {
        char* str = new char[16];
        snprintf(str, 16, "Hive %06lu", static_cast<unsigned long>(hiveNum));
        _itemStrings.push_back(str);
        _displayItems.push_back(str);
    }
}

void ScreenCloseHive::onEnter() {
    _currentStep = CloseHiveStep::SELECT;
    _selectedIndex = 0;
    _scrollOffset = 0;
    _reasonSelectedIndex = 0;
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    _hiveToClose = 0;
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_expectedTag, 0, sizeof(_expectedTag));

    loadHiveList();
    render();
}

void ScreenCloseHive::onExit() {
    freeItemStrings();
}

void ScreenCloseHive::render() {
    drawBackground();

    switch (_currentStep) {
        case CloseHiveStep::SELECT:
            renderSelectStep();
            break;
        case CloseHiveStep::REASON:
            renderReasonStep();
            break;
        case CloseHiveStep::RFID_SCAN:
            renderRfidStep();
            break;
        case CloseHiveStep::COMPLETE:
            break;
    }

    pushToDisplay();
}

void ScreenCloseHive::renderSelectStep() {
    drawHeader("CLOSE HIVE");

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Yard %06lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawSubheader(subheader);

    if (_hiveNumbers.empty()) {
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No active hives", DISPLAY_CENTER_X, 120);
    } else {
        drawScrollableList(_displayItems.data(), static_cast<int>(_displayItems.size()),
                          _selectedIndex, _scrollOffset, 75);
    }
}

void ScreenCloseHive::renderReasonStep() {
    char header[32];
    snprintf(header, sizeof(header), "CLOSE %06lu", static_cast<unsigned long>(_hiveToClose));
    drawHeader(header);
    drawSubheader("Select reason");

    drawMenuOptions(REASONS, REASON_COUNT, _reasonSelectedIndex, 90);
}

void ScreenCloseHive::renderRfidStep() {
    char header[32];
    snprintf(header, sizeof(header), "CLOSE %06lu", static_cast<unsigned long>(_hiveToClose));
    drawHeader(header);

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Reason: %s", REASONS[_reasonSelectedIndex]);
    drawSubheader(subheader);

    if (_scanSuccess) {
        drawRfidSuccess(_scannedTag);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_SUCCESS);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click: Close hive", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else if (_scanError) {
        drawRfidError(_errorMessage);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click to retry", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        drawRfidScanPrompt("Scan Hive Tag", _scanning);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Confirm with tag", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }
}

void ScreenCloseHive::startRfidScan() {
    _scanning = true;
    _scanSuccess = false;
    _scanError = false;
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));

    // Get expected tag from hive record
    HiveRecord hive;
    if (loadHive(_hiveToClose, hive)) {
        strncpy(_expectedTag, hive.rfidTagId, RFID_TAG_LENGTH);
    }

    // Clear any previous tag detection and start scanning
    if (_hal->rfid.isReady()) {
        _hal->rfid.clearLastTag();
        _hal->rfid.startScanning();
    }
}

void ScreenCloseHive::simulateRfidScan() {
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
        char tagBuffer[20] = {0};
        if (_hal->rfid.getLastTagString(tagBuffer, sizeof(tagBuffer))) {
            strncpy(_scannedTag, tagBuffer, RFID_TAG_LENGTH);
            _scannedTag[RFID_TAG_LENGTH] = '\0';

            if (verifyRfidTag()) {
                _scanSuccess = true;
                _scanError = false;
                _hal->buzz.tone(2000, 100);  // Success beep
            } else {
                _scanSuccess = false;
                _scanError = true;
                strcpy(_errorMessage, "Wrong tag!");
            }
            _scanning = false;
        }
    }
    // If no tag detected yet, keep scanning
}

bool ScreenCloseHive::verifyRfidTag() {
    // Check if scanned tag matches expected tag
    return strcmp(_scannedTag, _expectedTag) == 0;
}

bool ScreenCloseHive::closeSelectedHive() {
    char reason = REASON_CODES[_reasonSelectedIndex];

    if (!closeHive(_hiveToClose, reason)) {
        return false;
    }

    // Update yard hive count
    updateYardHiveCount(_context->selectedYardNumber);

    return true;
}

void ScreenCloseHive::onRotate(int direction) {
    switch (_currentStep) {
        case CloseHiveStep::SELECT: {
            int itemCount = static_cast<int>(_displayItems.size());
            if (itemCount == 0) return;

            _selectedIndex += direction;

            if (_selectedIndex < 0) {
                _selectedIndex = 0;
            } else if (_selectedIndex >= itemCount) {
                _selectedIndex = itemCount - 1;
            }

            if (_selectedIndex < _scrollOffset) {
                _scrollOffset = _selectedIndex;
            } else if (_selectedIndex >= _scrollOffset + MAX_VISIBLE_ITEMS) {
                _scrollOffset = _selectedIndex - MAX_VISIBLE_ITEMS + 1;
            }
            break;
        }

        case CloseHiveStep::REASON:
            _reasonSelectedIndex += direction;

            if (_reasonSelectedIndex < 0) {
                _reasonSelectedIndex = REASON_COUNT - 1;
            } else if (_reasonSelectedIndex >= REASON_COUNT) {
                _reasonSelectedIndex = 0;
            }
            break;

        case CloseHiveStep::RFID_SCAN:
            // No rotation during RFID scan
            break;

        default:
            break;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenCloseHive::onConfirm() {
    NavigationResult result;

    switch (_currentStep) {
        case CloseHiveStep::SELECT:
            if (!_hiveNumbers.empty()) {
                _hiveToClose = _hiveNumbers[_selectedIndex];
                _currentStep = CloseHiveStep::REASON;
                buzzConfirm();
                render();
            }
            break;

        case CloseHiveStep::REASON:
            _context->closeReason = REASON_CODES[_reasonSelectedIndex];
            _currentStep = CloseHiveStep::RFID_SCAN;
            startRfidScan();
            buzzConfirm();
            render();
            break;

        case CloseHiveStep::RFID_SCAN:
            if (_scanSuccess) {
                if (closeSelectedHive()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_SUMMARY;
                } else {
                    _scanError = true;
                    strcpy(_errorMessage, "Close failed");
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
                // Click while scanning = abort, go back to reason
                _currentStep = CloseHiveStep::REASON;
                buzzNavigate();
                render();
            }
            break;

        default:
            break;
    }

    return result;
}

NavigationResult ScreenCloseHive::onBack() {
    NavigationResult result;

    switch (_currentStep) {
        case CloseHiveStep::SELECT:
            result.nextScreen = ScreenType::HIVE_MGMT;
            break;

        case CloseHiveStep::REASON:
            _currentStep = CloseHiveStep::SELECT;
            render();
            break;

        case CloseHiveStep::RFID_SCAN:
            _currentStep = CloseHiveStep::REASON;
            render();
            break;

        default:
            result.nextScreen = ScreenType::HIVE_MGMT;
            break;
    }

    return result;
}

NavigationResult ScreenCloseHive::onUpdate() {
    NavigationResult result;

    // Check for RFID tag detection during scanning
    if (_currentStep == CloseHiveStep::RFID_SCAN && _scanning && !_scanSuccess && !_scanError) {
        simulateRfidScan();

        if (_scanSuccess || _scanError) {
            render();

            // Auto-advance on successful scan
            if (_scanSuccess) {
                if (closeSelectedHive()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_SUMMARY;
                } else {
                    _scanError = true;
                    strcpy(_errorMessage, "Close failed");
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
