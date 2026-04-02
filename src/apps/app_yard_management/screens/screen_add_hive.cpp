/**
 * @file screen_add_hive.cpp
 * @brief Screen 7: Add Hive implementation
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_add_hive.h"
#include <esp_timer.h>
#include <cstdio>
#include <cstring>
#include <algorithm>

namespace YARD_MANAGEMENT {

ScreenAddHive::ScreenAddHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(AddHiveStep::NUMBER)
    , _originSelectedIndex(0)
    , _originScrollOffset(0)
    , _scanning(false)
    , _scanSuccess(false)
    , _scanError(false)
    , _numberValid(true)
{
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));
    memset(_yardName, 0, sizeof(_yardName));
}

ScreenAddHive::~ScreenAddHive() {
    freeOptionStrings();
}

void ScreenAddHive::freeOptionStrings() {
    for (char* str : _optionStrings) {
        delete[] str;
    }
    _optionStrings.clear();
    _originOptions.clear();
}

void ScreenAddHive::loadOriginOptions() {
    freeOptionStrings();
    _originHives.clear();

    // First option is always "Swarm"
    char* swarmStr = new char[8];
    strcpy(swarmStr, "Swarm");
    _optionStrings.push_back(swarmStr);
    _originOptions.push_back(swarmStr);

    // Get active hives in this yard for split origin
    std::vector<uint32_t> hives = getActiveHivesInYard(_context->selectedYardNumber);

    for (uint32_t hiveNum : hives) {
        char* str = new char[16];
        snprintf(str, 16, "%06lu", static_cast<unsigned long>(hiveNum));
        _optionStrings.push_back(str);
        _originOptions.push_back(str);
        _originHives.push_back(hiveNum);
    }
}

void ScreenAddHive::onEnter() {
    _currentStep = AddHiveStep::NUMBER;
    _originSelectedIndex = 0;
    _originScrollOffset = 0;
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    _numberValid = true;
    // Start from the next available hive number
    {
        std::vector<uint32_t> existing = getAllHiveNumbers();
        uint32_t next = 1;
        if (!existing.empty()) {
            next = *std::max_element(existing.begin(), existing.end()) + 1;
        }
        _context->newHiveNumber = next;
    }
    _context->originType = HiveOriginType::SWARM;
    _context->originHiveNumber = 0;
    memset(_scannedTag, 0, sizeof(_scannedTag));

    // Load yard name
    YardRecord yard;
    if (loadYard(_context->selectedYardNumber, yard)) {
        strncpy(_yardName, yard.nickname, MAX_NICKNAME_LENGTH);
        _yardName[MAX_NICKNAME_LENGTH] = '\0';
    } else {
        snprintf(_yardName, sizeof(_yardName), "Yard %lu",
                 static_cast<unsigned long>(_context->selectedYardNumber));
    }

    render();
}

void ScreenAddHive::onExit() {
    freeOptionStrings();
}

void ScreenAddHive::render() {
    drawBackground();

    switch (_currentStep) {
        case AddHiveStep::NUMBER:
            renderNumberStep();
            break;
        case AddHiveStep::ORIGIN:
            renderOriginStep();
            break;
        case AddHiveStep::RFID_SCAN:
            renderRfidStep();
            break;
        case AddHiveStep::COMPLETE:
            break;
    }

    pushToDisplay();
}

void ScreenAddHive::renderNumberStep() {
    drawHeader("ADD HIVE");

    drawSubheader(_yardName);

    drawCounterInput(_context->newHiveNumber, 1, MAX_HIVE_NUMBER, 80);

    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::bottom_center);

    if (!_numberValid) {
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->drawString("Number exists!", DISPLAY_CENTER_X, CONTENT_BOTTOM - 20);
    }

    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->drawString("Click: Confirm", DISPLAY_CENTER_X, CONTENT_BOTTOM);
}

void ScreenAddHive::renderOriginStep() {
    drawHeader("ADD HIVE");

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Hive %06lu - Origin", static_cast<unsigned long>(_context->newHiveNumber));
    drawSubheader(subheader);

    if (_originOptions.empty()) {
        loadOriginOptions();
    }

    drawScrollableList(_originOptions.data(), static_cast<int>(_originOptions.size()),
                      _originSelectedIndex, _originScrollOffset, 75);
}

void ScreenAddHive::renderRfidStep() {
    drawHeader("ADD HIVE");

    const char* originStr = (_context->originType == HiveOriginType::SWARM) ?
                            "Swarm" : "Split";
    char subheader[48];
    snprintf(subheader, sizeof(subheader), "Hive %06lu from %s",
             static_cast<unsigned long>(_context->newHiveNumber), originStr);
    drawSubheader(subheader);

    if (_scanSuccess) {
        drawRfidSuccess(_scannedTag);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_SUCCESS);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click: Save hive", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else if (_scanError) {
        drawRfidError(_errorMessage);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Click to retry", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        drawRfidScanPrompt("Scan Hive Tag", _scanning);
    }
}

bool ScreenAddHive::validateNumber() {
    _numberValid = isHiveNumberUnique(_context->newHiveNumber);
    return _numberValid;
}

void ScreenAddHive::startRfidScan() {
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

void ScreenAddHive::simulateRfidScan() {
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
        char tagBuffer[20] = {0};
        if (_hal->rfid.getLastTagString(tagBuffer, sizeof(tagBuffer))) {
            strncpy(_scannedTag, tagBuffer, RFID_TAG_LENGTH);
            _scannedTag[RFID_TAG_LENGTH] = '\0';

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
    // If no tag detected yet, keep scanning
#else
    // RFID disabled
    _scanSuccess = false;
    _scanError = true;
    strcpy(_errorMessage, "RFID disabled");
    _scanning = false;
#endif
}

bool ScreenAddHive::createHive() {
    HiveRecord hive;
    hive.hiveNumber = _context->newHiveNumber;
    hive.yardNumber = _context->selectedYardNumber;
    hive.originType = _context->originType;
    hive.originHiveNumber = _context->originHiveNumber;
    hive.setRfidTag(_scannedTag);
    hive.creationTimestamp = static_cast<uint32_t>(esp_timer_get_time() / 1000000);
    hive.closureTimestamp = 0;
    hive.status = static_cast<char>(HiveStatus::ACTIVE);

    if (!saveHive(hive)) {
        return false;
    }

    // Update yard hive count
    updateYardHiveCount(_context->selectedYardNumber);

    return true;
}

void ScreenAddHive::onRotate(int direction) {
    switch (_currentStep) {
        case AddHiveStep::NUMBER:
            _context->newHiveNumber = adjustCounter(_context->newHiveNumber, direction, 1, MAX_HIVE_NUMBER);
            _numberValid = true;
            break;

        case AddHiveStep::ORIGIN: {
            int optionCount = static_cast<int>(_originOptions.size());
            _originSelectedIndex += direction;

            if (_originSelectedIndex < 0) {
                _originSelectedIndex = 0;
            } else if (_originSelectedIndex >= optionCount) {
                _originSelectedIndex = optionCount - 1;
            }

            if (_originSelectedIndex < _originScrollOffset) {
                _originScrollOffset = _originSelectedIndex;
            } else if (_originSelectedIndex >= _originScrollOffset + MAX_VISIBLE_ITEMS) {
                _originScrollOffset = _originSelectedIndex - MAX_VISIBLE_ITEMS + 1;
            }
            break;
        }

        case AddHiveStep::RFID_SCAN:
            // No rotation action during RFID scan
            break;

        default:
            break;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenAddHive::onConfirm() {
    NavigationResult result;

    switch (_currentStep) {
        case AddHiveStep::NUMBER:
            // Validate and move to origin step
            if (validateNumber()) {
                _currentStep = AddHiveStep::ORIGIN;
                loadOriginOptions();
                buzzConfirm();
            } else {
                buzzError();
            }
            render();
            break;

        case AddHiveStep::ORIGIN:
            // Set origin based on selection
            if (_originSelectedIndex == 0) {
                _context->originType = HiveOriginType::SWARM;
                _context->originHiveNumber = 0;
            } else {
                _context->originType = HiveOriginType::SPLIT;
                _context->originHiveNumber = _originHives[_originSelectedIndex - 1];
            }

            _currentStep = AddHiveStep::RFID_SCAN;
            startRfidScan();
            buzzConfirm();
            render();
            break;

        case AddHiveStep::RFID_SCAN:
            if (_scanSuccess) {
                if (createHive()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_SUMMARY;
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
                // Click while scanning = abort, go back to origin
                _currentStep = AddHiveStep::ORIGIN;
                buzzNavigate();
                render();
            }
            break;

        default:
            break;
    }

    return result;
}

NavigationResult ScreenAddHive::onBack() {
    NavigationResult result;

    switch (_currentStep) {
        case AddHiveStep::NUMBER:
            result.nextScreen = ScreenType::HIVE_MGMT;
            break;

        case AddHiveStep::ORIGIN:
            _currentStep = AddHiveStep::NUMBER;
            render();
            break;

        case AddHiveStep::RFID_SCAN:
            _currentStep = AddHiveStep::ORIGIN;
            render();
            break;

        default:
            result.nextScreen = ScreenType::HIVE_MGMT;
            break;
    }

    return result;
}

NavigationResult ScreenAddHive::onUpdate() {
    NavigationResult result;

    // Only poll for RFID when on the RFID scan step and actively scanning
    if (_currentStep == AddHiveStep::RFID_SCAN && _scanning && !_scanSuccess && !_scanError) {
        simulateRfidScan();

        if (_scanSuccess || _scanError) {
            render();

            if (_scanSuccess) {
                if (createHive()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_SUMMARY;
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
