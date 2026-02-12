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

namespace YARD_MANAGEMENT {

ScreenAddHive::ScreenAddHive(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(AddHiveStep::NUMBER)
    , _currentDigit(0)
    , _originSelectedIndex(0)
    , _originScrollOffset(0)
    , _scanning(false)
    , _scanSuccess(false)
    , _scanError(false)
    , _numberValid(true)
{
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));
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
    _currentDigit = 0;
    _originSelectedIndex = 0;
    _originScrollOffset = 0;
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    _numberValid = true;
    _context->newHiveNumber = MIN_HIVE_NUMBER;
    _context->originType = HiveOriginType::SWARM;
    _context->originHiveNumber = 0;
    memset(_scannedTag, 0, sizeof(_scannedTag));

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

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Yard %06lu", static_cast<unsigned long>(_context->selectedYardNumber));
    drawSubheader(subheader);

    drawSixDigitInput(_context->newHiveNumber, _currentDigit, 85);

    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);

    if (!_numberValid) {
        _canvas->setTextColor(COLOR_ERROR);
        _canvas->drawString("Number exists!", DISPLAY_CENTER_X, 150);
    } else {
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->drawString("Enter 6-digit number", DISPLAY_CENTER_X, 150);
    }
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
        _canvas->drawString("Click to save", DISPLAY_CENTER_X, CONTENT_BOTTOM);
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
}

void ScreenAddHive::simulateRfidScan() {
    // Generate mock RFID tag for testing
    uint32_t mockId = static_cast<uint32_t>(esp_timer_get_time() & 0xFFFFFFFF);
    snprintf(_scannedTag, sizeof(_scannedTag), "%08lX%08lX",
             static_cast<unsigned long>(mockId),
             static_cast<unsigned long>(mockId ^ 0x5A5A5A5A));

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
            _context->newHiveNumber = adjustDigit(_context->newHiveNumber, _currentDigit, direction);
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
            _currentDigit++;
            if (_currentDigit >= NUM_DIGITS) {
                if (validateNumber()) {
                    _currentStep = AddHiveStep::ORIGIN;
                    loadOriginOptions();
                    _currentDigit = 0;
                    buzzConfirm();
                } else {
                    _currentDigit = 0;
                    buzzError();
                }
            } else {
                buzzNavigate();
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
                startRfidScan();
                simulateRfidScan();
                buzzNavigate();
                render();
            } else {
                simulateRfidScan();
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
            if (_currentDigit > 0) {
                _currentDigit--;
                render();
            } else {
                result.nextScreen = ScreenType::HIVE_MGMT;
            }
            break;

        case AddHiveStep::ORIGIN:
            _currentStep = AddHiveStep::NUMBER;
            _currentDigit = NUM_DIGITS - 1;
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

} // namespace YARD_MANAGEMENT
