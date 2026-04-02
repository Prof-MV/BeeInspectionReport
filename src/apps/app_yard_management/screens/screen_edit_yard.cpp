/**
 * @file screen_edit_yard.cpp
 * @brief Screen: Edit Yard implementation
 * @version 1.0
 * @date 2026-03-28
 */
#include "screen_edit_yard.h"
#include "../yard_storage.h"
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

ScreenEditYard::ScreenEditYard(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : ScreenBaseYM(hal, canvas, context)
    , _currentStep(EditYardStep::SELECT_YARD)
    , _selectedIndex(0)
    , _scrollOffset(0)
    , _cursorPos(0)
    , _selectedChar('A')
    , _scanning(false)
    , _scanSuccess(false)
    , _scanError(false)
{
    memset(_nickname, 0, sizeof(_nickname));
    memset(_originalNickname, 0, sizeof(_originalNickname));
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_originalRfidTag, 0, sizeof(_originalRfidTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));
}

void ScreenEditYard::onEnter() {
    _currentStep = EditYardStep::SELECT_YARD;
    _selectedIndex = 0;
    _scrollOffset = 0;
    _cursorPos = 0;
    _selectedChar = 'A';
    _scanning = false;
    _scanSuccess = false;
    _scanError = false;
    memset(_nickname, 0, sizeof(_nickname));
    memset(_originalNickname, 0, sizeof(_originalNickname));
    memset(_scannedTag, 0, sizeof(_scannedTag));
    memset(_originalRfidTag, 0, sizeof(_originalRfidTag));
    memset(_errorMessage, 0, sizeof(_errorMessage));

    loadYardList();
    render();
}

void ScreenEditYard::onExit() {
    // Nothing to save until complete
}

void ScreenEditYard::loadYardList() {
    _yardNumbers.clear();
    _yardNumbers = getActiveYardNumbers();
}

void ScreenEditYard::loadSelectedYardData() {
    if (_selectedIndex >= 0 && _selectedIndex < static_cast<int>(_yardNumbers.size())) {
        uint32_t yardNum = _yardNumbers[_selectedIndex];
        YardRecord yard;
        if (loadYard(yardNum, yard)) {
            _context->selectedYardNumber = yardNum;
            strncpy(_nickname, yard.nickname, MAX_NICKNAME_LENGTH);
            strncpy(_originalNickname, yard.nickname, MAX_NICKNAME_LENGTH);
            strncpy(_scannedTag, yard.rfidTagId, RFID_TAG_LENGTH);
            strncpy(_originalRfidTag, yard.rfidTagId, RFID_TAG_LENGTH);
            _cursorPos = strlen(_nickname);
            if (_cursorPos > 0 && _cursorPos <= MAX_NICKNAME_LENGTH) {
                _selectedChar = _nickname[_cursorPos - 1];
            } else {
                _selectedChar = 'A';
            }
        }
    }
}

void ScreenEditYard::render() {
    drawBackground();

    switch (_currentStep) {
        case EditYardStep::SELECT_YARD:
            renderSelectYardStep();
            break;
        case EditYardStep::NICKNAME:
            renderNicknameStep();
            break;
        case EditYardStep::RFID_SCAN:
            renderRfidStep();
            break;
        case EditYardStep::COMPLETE:
            break;
    }

    pushToDisplay();
}

void ScreenEditYard::renderSelectYardStep() {
    drawHeader("EDIT YARD");

    if (_yardNumbers.empty()) {
        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::middle_center);
        _canvas->drawString("No active yards", DISPLAY_CENTER_X, 120);

        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Hold: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        drawSubheader("Select yard to edit");

        // Draw list items manually since we need custom formatting
        int startY = 75;
        int itemCount = static_cast<int>(_yardNumbers.size());
        int visibleCount = (itemCount < MAX_VISIBLE_ITEMS) ? itemCount : MAX_VISIBLE_ITEMS;

        for (int i = 0; i < visibleCount; i++) {
            int actualIndex = _scrollOffset + i;
            if (actualIndex >= itemCount) break;

            uint32_t yardNum = _yardNumbers[actualIndex];
            YardRecord yard;
            char displayStr[32];
            if (loadYard(yardNum, yard)) {
                snprintf(displayStr, sizeof(displayStr), "%06lu - %s",
                         static_cast<unsigned long>(yardNum), yard.nickname);
            } else {
                snprintf(displayStr, sizeof(displayStr), "%06lu",
                         static_cast<unsigned long>(yardNum));
            }

            bool selected = (actualIndex == _selectedIndex);
            drawListItem(displayStr, startY + i * LIST_ITEM_HEIGHT, selected, false);
        }

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        _canvas->drawString("Hold: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }
}

void ScreenEditYard::renderNicknameStep() {
    drawHeader("EDIT YARD");

    char subheader[32];
    snprintf(subheader, sizeof(subheader), "Yard %06lu",
             static_cast<unsigned long>(_context->selectedYardNumber));
    drawSubheader(subheader);

    drawNicknameInput(_nickname, _cursorPos, _selectedChar, 90);

    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::bottom_center);
    if (strlen(_nickname) > 0) {
        _canvas->drawString("Hold: Next step", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    } else {
        _canvas->drawString("Hold: Back", DISPLAY_CENTER_X, CONTENT_BOTTOM);
    }
}

void ScreenEditYard::renderRfidStep() {
    drawHeader("EDIT YARD");

    char subheader[48];
    snprintf(subheader, sizeof(subheader), "%06lu - %s",
             static_cast<unsigned long>(_context->selectedYardNumber), _nickname);
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
        drawRfidScanPrompt("Scan New Tag", _scanning);

        _canvas->setFont(&fonts::FreeSans9pt7b);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        _canvas->setTextDatum(textdatum_t::bottom_center);
        if (_scanning) {
            _canvas->drawString("Click: Keep current", DISPLAY_CENTER_X, CONTENT_BOTTOM);
        } else {
            _canvas->drawString("Click: Start scan", DISPLAY_CENTER_X, CONTENT_BOTTOM);
        }
    }
}

void ScreenEditYard::startRfidScan() {
    _scanning = true;
    _scanSuccess = false;
    _scanError = false;
    memset(_errorMessage, 0, sizeof(_errorMessage));

#if RFID_ENABLE
    if (_hal->rfid.isReady()) {
        _hal->rfid.clearLastTag();
        _hal->rfid.startScanning();
    }
#endif
}

void ScreenEditYard::checkRfidScan() {
#if RFID_ENABLE
    if (!_hal->rfid.isReady()) {
        _scanSuccess = false;
        _scanError = true;
        strcpy(_errorMessage, "RFID not ready");
        _scanning = false;
        return;
    }

    if (_hal->rfid.wasTagDetected()) {
        char tagBuffer[20] = {0};
        if (_hal->rfid.getLastTagString(tagBuffer, sizeof(tagBuffer))) {
            // Check if it's different from current tag
            if (strcmp(tagBuffer, _originalRfidTag) == 0) {
                _scanSuccess = false;
                _scanError = true;
                strcpy(_errorMessage, "Same as current");
                _scanning = false;
                return;
            }

            strncpy(_scannedTag, tagBuffer, RFID_TAG_LENGTH);
            _scannedTag[RFID_TAG_LENGTH] = '\0';

            // Check uniqueness (exclude current yard from check)
            if (isRfidUnique(_scannedTag, true, true)) {
                _scanSuccess = true;
                _scanError = false;
                _hal->buzz.tone(2000, 100);
            } else {
                _scanSuccess = false;
                _scanError = true;
                strcpy(_errorMessage, "Tag already in use");
            }
            _scanning = false;
        }
    }
#else
    _scanSuccess = false;
    _scanError = true;
    strcpy(_errorMessage, "RFID disabled");
    _scanning = false;
#endif
}

bool ScreenEditYard::updateYard() {
    YardRecord yard;
    if (!loadYard(_context->selectedYardNumber, yard)) {
        return false;
    }

    yard.setNickname(_nickname);
    yard.setRfidTag(_scannedTag);

    return saveYard(yard);
}

void ScreenEditYard::onRotate(int direction) {
    switch (_currentStep) {
        case EditYardStep::SELECT_YARD: {
            int itemCount = static_cast<int>(_yardNumbers.size());
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

        case EditYardStep::NICKNAME:
            _selectedChar = getNextChar(_selectedChar, direction);
            break;

        case EditYardStep::RFID_SCAN:
            break;

        default:
            break;
    }

    buzzNavigate();
    render();
}

NavigationResult ScreenEditYard::onConfirm() {
    NavigationResult result;

    switch (_currentStep) {
        case EditYardStep::SELECT_YARD:
            if (!_yardNumbers.empty()) {
                loadSelectedYardData();
                _currentStep = EditYardStep::NICKNAME;
                buzzConfirm();
                render();
            }
            break;

        case EditYardStep::NICKNAME:
            if (_selectedChar == '\b') {
                if (_cursorPos > 0) {
                    _cursorPos--;
                    _nickname[_cursorPos] = '\0';
                    _selectedChar = 'A';
                    buzzNavigate();
                }
            } else if (_cursorPos < MAX_NICKNAME_LENGTH) {
                _nickname[_cursorPos] = _selectedChar;
                _cursorPos++;
                buzzNavigate();
            }
            render();
            break;

        case EditYardStep::RFID_SCAN:
            if (_scanSuccess) {
                if (updateYard()) {
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
                startRfidScan();
                buzzNavigate();
                render();
            } else if (_scanning) {
                // Keep current RFID tag and save
                _scanning = false;
                strncpy(_scannedTag, _originalRfidTag, RFID_TAG_LENGTH);
                if (updateYard()) {
                    buzzSuccess();
                    result.nextScreen = ScreenType::YARD_LIST;
                } else {
                    _scanError = true;
                    strcpy(_errorMessage, "Save failed");
                    buzzError();
                    render();
                }
            } else {
                // Start scanning
                startRfidScan();
                buzzNavigate();
                render();
            }
            break;

        default:
            break;
    }

    return result;
}

NavigationResult ScreenEditYard::onBack() {
    NavigationResult result;

    switch (_currentStep) {
        case EditYardStep::SELECT_YARD:
            result.nextScreen = ScreenType::MGMT_MENU;
            break;

        case EditYardStep::NICKNAME:
            if (strlen(_nickname) > 0) {
                _context->yardNickname[0] = '\0';
                strncpy(_context->yardNickname, _nickname, MAX_NICKNAME_LENGTH);
                _currentStep = EditYardStep::RFID_SCAN;
                _scanning = false;
                _scanSuccess = false;
                _scanError = false;
                buzzConfirm();
                render();
            } else {
                _currentStep = EditYardStep::SELECT_YARD;
                render();
            }
            break;

        case EditYardStep::RFID_SCAN:
            _currentStep = EditYardStep::NICKNAME;
            _cursorPos = strlen(_nickname);
            render();
            break;

        default:
            result.nextScreen = ScreenType::MGMT_MENU;
            break;
    }

    return result;
}

NavigationResult ScreenEditYard::onUpdate() {
    NavigationResult result;

    if (_currentStep == EditYardStep::RFID_SCAN && _scanning && !_scanSuccess && !_scanError) {
        checkRfidScan();

        if (_scanSuccess || _scanError) {
            render();
            // User confirms save via onConfirm()
        }
    }

    return result;
}

} // namespace YARD_MANAGEMENT
