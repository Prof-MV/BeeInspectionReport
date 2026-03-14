/**
 * @file screen_base_ym.cpp
 * @brief Base class implementation for yard management screens
 * @version 1.0
 * @date 2026-02-12
 */
#include "screen_base_ym.h"
#include <cstdio>
#include <cstring>

namespace YARD_MANAGEMENT {

ScreenBaseYM::ScreenBaseYM(HAL::HAL* hal, LGFX_Sprite* canvas, AppContext* context)
    : _hal(hal)
    , _canvas(canvas)
    , _context(context)
    , _themeColor(COLOR_SELECTED)
{
}

void ScreenBaseYM::drawBackground() {
    _canvas->fillScreen(COLOR_BACKGROUND);
}

void ScreenBaseYM::drawHeader(const char* title) {
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextSize(1);
    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(title, DISPLAY_CENTER_X, CONTENT_TOP);
}

void ScreenBaseYM::drawSubheader(const char* text) {
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(text, DISPLAY_CENTER_X, CONTENT_TOP + 28);
}

void ScreenBaseYM::drawSixDigitInput(uint32_t value, int highlightDigit, int y) {
    // Calculate starting X to center all digits
    int totalWidth = NUM_DIGITS * DIGIT_WIDTH + (NUM_DIGITS - 1) * DIGIT_SPACING;
    int startX = (DISPLAY_WIDTH - totalWidth) / 2;

    // Extract individual digits
    char digits[NUM_DIGITS + 1];
    snprintf(digits, sizeof(digits), "%06lu", static_cast<unsigned long>(value));

    // Draw each digit
    for (int i = 0; i < NUM_DIGITS; i++) {
        int x = startX + i * (DIGIT_WIDTH + DIGIT_SPACING);

        // Draw digit background
        if (i == highlightDigit) {
            _canvas->fillRoundRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, 6, _themeColor);
            _canvas->setTextColor(COLOR_TEXT_PRIMARY);
        } else {
            _canvas->drawRoundRect(x, y, DIGIT_WIDTH, DIGIT_HEIGHT, 6, COLOR_TEXT_SECONDARY);
            _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        }

        // Draw digit
        _canvas->setFont(&fonts::FreeSansBold18pt7b);
        _canvas->setTextDatum(textdatum_t::middle_center);
        char digitStr[2] = {digits[i], '\0'};
        _canvas->drawString(digitStr, x + DIGIT_WIDTH / 2, y + DIGIT_HEIGHT / 2);
    }

    // Draw up/down arrows for highlighted digit
    if (highlightDigit >= 0 && highlightDigit < NUM_DIGITS) {
        int arrowX = startX + highlightDigit * (DIGIT_WIDTH + DIGIT_SPACING) + DIGIT_WIDTH / 2;
        // Up arrow
        _canvas->fillTriangle(arrowX, y - 10, arrowX - 8, y - 2, arrowX + 8, y - 2, _themeColor);
        // Down arrow
        _canvas->fillTriangle(arrowX, y + DIGIT_HEIGHT + 10, arrowX - 8, y + DIGIT_HEIGHT + 2, arrowX + 8, y + DIGIT_HEIGHT + 2, _themeColor);
    }
}

uint32_t ScreenBaseYM::adjustDigit(uint32_t value, int digitIndex, int direction) {
    // Calculate multiplier for this digit position
    uint32_t multiplier = 1;
    for (int i = 0; i < (NUM_DIGITS - 1 - digitIndex); i++) {
        multiplier *= 10;
    }

    // Extract current digit
    uint32_t digit = (value / multiplier) % 10;

    // Adjust digit with wrap-around
    if (direction > 0) {
        digit = (digit + 1) % 10;
    } else {
        digit = (digit + 9) % 10;  // Same as -1 with wrap
    }

    // Reconstruct value
    uint32_t newValue = value - ((value / multiplier) % 10) * multiplier + digit * multiplier;

    // Clamp to valid range
    if (newValue < MIN_YARD_NUMBER) {
        newValue = MIN_YARD_NUMBER;
    } else if (newValue > MAX_YARD_NUMBER) {
        newValue = MAX_YARD_NUMBER;
    }

    return newValue;
}

void ScreenBaseYM::drawCounterInput(uint32_t value, uint32_t minValue, uint32_t maxValue, int y) {
    // Format the number - show only significant digits (right-aligned growth)
    char numStr[12];
    snprintf(numStr, sizeof(numStr), "%lu", static_cast<unsigned long>(value));
    int numLen = strlen(numStr);

    // Calculate box dimensions based on number of digits shown
    int charWidth = 22;
    int charSpacing = 2;
    int boxPadding = 16;
    int boxHeight = 50;
    int boxWidth = numLen * (charWidth + charSpacing) - charSpacing + boxPadding * 2;
    int minBoxWidth = 80;  // Minimum width for single digit
    if (boxWidth < minBoxWidth) boxWidth = minBoxWidth;

    int boxX = (DISPLAY_WIDTH - boxWidth) / 2;

    // Draw the input box
    _canvas->fillRoundRect(boxX, y, boxWidth, boxHeight, 8, COLOR_HIGHLIGHT_BG);
    _canvas->drawRoundRect(boxX, y, boxWidth, boxHeight, 8, _themeColor);

    // Draw the number (right-aligned within box)
    _canvas->setFont(&fonts::FreeSansBold18pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(numStr, DISPLAY_CENTER_X, y + boxHeight / 2);

    // Draw up arrow above
    int arrowY = y - 12;
    _canvas->fillTriangle(DISPLAY_CENTER_X, arrowY - 10,
                          DISPLAY_CENTER_X - 12, arrowY,
                          DISPLAY_CENTER_X + 12, arrowY, _themeColor);

    // Draw down arrow below
    arrowY = y + boxHeight + 12;
    _canvas->fillTriangle(DISPLAY_CENTER_X, arrowY + 10,
                          DISPLAY_CENTER_X - 12, arrowY,
                          DISPLAY_CENTER_X + 12, arrowY, _themeColor);

    // Show min/max hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::top_center);

    char rangeStr[24];
    snprintf(rangeStr, sizeof(rangeStr), "%lu - %lu",
             static_cast<unsigned long>(minValue), static_cast<unsigned long>(maxValue));
    _canvas->drawString(rangeStr, DISPLAY_CENTER_X, y + boxHeight + 28);
}

uint32_t ScreenBaseYM::adjustCounter(uint32_t value, int direction, uint32_t minValue, uint32_t maxValue) {
    if (direction > 0) {
        // Increment
        if (value < maxValue) {
            value++;
        } else {
            value = minValue;  // Wrap to minimum
        }
    } else {
        // Decrement
        if (value > minValue) {
            value--;
        } else {
            value = maxValue;  // Wrap to maximum
        }
    }
    return value;
}

void ScreenBaseYM::drawListItem(const char* text, int y, bool selected, bool isManagement) {
    int boxWidth = MAX_CONTENT_WIDTH;
    int boxHeight = LIST_ITEM_HEIGHT;
    int boxX = (DISPLAY_WIDTH - boxWidth) / 2;

    if (selected) {
        _canvas->fillRoundRect(boxX, y, boxWidth, boxHeight, 6, _themeColor);
        _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    } else {
        if (isManagement) {
            _canvas->drawRoundRect(boxX, y, boxWidth, boxHeight, 6, COLOR_WARNING);
            _canvas->setTextColor(COLOR_WARNING);
        } else {
            _canvas->drawRoundRect(boxX, y, boxWidth, boxHeight, 6, COLOR_TEXT_SECONDARY);
            _canvas->setTextColor(COLOR_TEXT_SECONDARY);
        }
    }

    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString(text, DISPLAY_CENTER_X, y + boxHeight / 2);
}

void ScreenBaseYM::drawScrollableList(const char** items, int itemCount, int selectedIndex, int scrollOffset, int startY) {
    int visibleCount = (itemCount < MAX_VISIBLE_ITEMS) ? itemCount : MAX_VISIBLE_ITEMS;

    for (int i = 0; i < visibleCount && (scrollOffset + i) < itemCount; i++) {
        int itemIndex = scrollOffset + i;
        int y = startY + i * (LIST_ITEM_HEIGHT + 4);
        bool isLast = (itemIndex == itemCount - 1);
        drawListItem(items[itemIndex], y, itemIndex == selectedIndex, isLast);
    }

    // Draw scroll indicators if needed
    if (scrollOffset > 0) {
        // Up arrow
        _canvas->fillTriangle(DISPLAY_CENTER_X, startY - 15,
                              DISPLAY_CENTER_X - 10, startY - 5,
                              DISPLAY_CENTER_X + 10, startY - 5,
                              COLOR_TEXT_SECONDARY);
    }
    if (scrollOffset + visibleCount < itemCount) {
        // Down arrow
        int bottomY = startY + visibleCount * (LIST_ITEM_HEIGHT + 4);
        _canvas->fillTriangle(DISPLAY_CENTER_X, bottomY + 10,
                              DISPLAY_CENTER_X - 10, bottomY,
                              DISPLAY_CENTER_X + 10, bottomY,
                              COLOR_TEXT_SECONDARY);
    }
}

void ScreenBaseYM::drawMenuOptions(const char* const* options, int optionCount, int selectedIndex, int startY) {
    int spacing = 45;

    for (int i = 0; i < optionCount; i++) {
        int y = startY + i * spacing;
        drawListItem(options[i], y, i == selectedIndex, false);
    }
}

void ScreenBaseYM::drawRfidScanPrompt(const char* message, bool scanning) {
    // Draw RFID icon (simplified NFC symbol)
    int iconX = DISPLAY_CENTER_X;
    int iconY = 100;
    int iconRadius = 25;

    // Draw concentric arcs to represent NFC
    for (int r = iconRadius; r >= 10; r -= 8) {
        _canvas->drawArc(iconX, iconY, r, r - 2, 225, 315, scanning ? _themeColor : COLOR_TEXT_SECONDARY);
    }

    // Draw central circle
    _canvas->fillCircle(iconX, iconY + 5, 6, scanning ? _themeColor : COLOR_TEXT_SECONDARY);

    // Draw message
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(message, DISPLAY_CENTER_X, 145);

    // Draw scanning animation dots if scanning
    if (scanning) {
        static int dotPhase = 0;
        dotPhase = (dotPhase + 1) % 3;
        char dots[4] = "   ";
        for (int i = 0; i <= dotPhase; i++) {
            dots[i] = '.';
        }
        _canvas->drawString(dots, DISPLAY_CENTER_X, 165);
    }
}

void ScreenBaseYM::drawRfidSuccess(const char* tagId) {
    // Draw checkmark
    int cx = DISPLAY_CENTER_X;
    int cy = 100;

    _canvas->fillCircle(cx, cy, 25, COLOR_SUCCESS);
    _canvas->drawLine(cx - 15, cy, cx - 5, cy + 10, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx - 5, cy + 10, cx + 15, cy - 10, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx - 14, cy + 1, cx - 4, cy + 11, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx - 4, cy + 11, cx + 16, cy - 9, COLOR_TEXT_PRIMARY);

    // Draw tag ID
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_SUCCESS);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString("Tag:", DISPLAY_CENTER_X, 140);

    _canvas->setFont(&fonts::FreeMono9pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    // Show truncated tag ID if too long
    char shortTag[12];
    if (strlen(tagId) > 10) {
        strncpy(shortTag, tagId, 4);
        shortTag[4] = '.';
        shortTag[5] = '.';
        strncpy(shortTag + 6, tagId + strlen(tagId) - 4, 4);
        shortTag[10] = '\0';
    } else {
        strncpy(shortTag, tagId, sizeof(shortTag) - 1);
        shortTag[sizeof(shortTag) - 1] = '\0';
    }
    _canvas->drawString(shortTag, DISPLAY_CENTER_X, 160);
}

void ScreenBaseYM::drawRfidError(const char* message) {
    // Draw X mark
    int cx = DISPLAY_CENTER_X;
    int cy = 100;

    _canvas->fillCircle(cx, cy, 25, COLOR_ERROR);
    _canvas->drawLine(cx - 12, cy - 12, cx + 12, cy + 12, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx + 12, cy - 12, cx - 12, cy + 12, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx - 11, cy - 12, cx + 13, cy + 12, COLOR_TEXT_PRIMARY);
    _canvas->drawLine(cx + 13, cy - 12, cx - 11, cy + 12, COLOR_TEXT_PRIMARY);

    // Draw error message
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_ERROR);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(message, DISPLAY_CENTER_X, 145);
}

void ScreenBaseYM::drawConfirmDialog(const char* title, const char* message, bool yesSelected) {
    // Draw semi-transparent overlay (darker background)
    _canvas->fillRect(20, 60, 200, 120, COLOR_BACKGROUND);
    _canvas->drawRoundRect(20, 60, 200, 120, 10, _themeColor);

    // Draw title
    _canvas->setFont(&fonts::FreeSansBold12pt7b);
    _canvas->setTextColor(_themeColor);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString(title, DISPLAY_CENTER_X, 70);

    // Draw message
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->drawString(message, DISPLAY_CENTER_X, 100);

    // Draw Yes/No buttons
    int buttonY = 140;
    int buttonWidth = 70;
    int buttonHeight = 28;

    // Yes button
    int yesX = DISPLAY_CENTER_X - buttonWidth - 10;
    if (yesSelected) {
        _canvas->fillRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 6, COLOR_SUCCESS);
        _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    } else {
        _canvas->drawRoundRect(yesX, buttonY, buttonWidth, buttonHeight, 6, COLOR_TEXT_SECONDARY);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    }
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString("Yes", yesX + buttonWidth / 2, buttonY + buttonHeight / 2);

    // No button
    int noX = DISPLAY_CENTER_X + 10;
    if (!yesSelected) {
        _canvas->fillRoundRect(noX, buttonY, buttonWidth, buttonHeight, 6, COLOR_ERROR);
        _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    } else {
        _canvas->drawRoundRect(noX, buttonY, buttonWidth, buttonHeight, 6, COLOR_TEXT_SECONDARY);
        _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    }
    _canvas->setTextDatum(textdatum_t::middle_center);
    _canvas->drawString("No", noX + buttonWidth / 2, buttonY + buttonHeight / 2);
}

void ScreenBaseYM::drawStatLine(const char* label, const char* value, int y) {
    _canvas->setFont(&fonts::FreeSans9pt7b);

    // Draw label (left aligned)
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::middle_left);
    _canvas->drawString(label, CONTENT_LEFT + 10, y);

    // Draw value (right aligned)
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::middle_right);
    _canvas->drawString(value, CONTENT_RIGHT - 10, y);
}

void ScreenBaseYM::drawStatLine(const char* label, int value, int y) {
    char valueStr[16];
    snprintf(valueStr, sizeof(valueStr), "%d", value);
    drawStatLine(label, valueStr, y);
}

void ScreenBaseYM::drawNicknameInput(const char* current, int cursorPos, char selectedChar, int y) {
    int boxWidth = MAX_CONTENT_WIDTH;
    int boxHeight = 36;
    int boxX = (DISPLAY_WIDTH - boxWidth) / 2;

    // Draw input box
    _canvas->drawRoundRect(boxX, y, boxWidth, boxHeight, 6, _themeColor);

    // Draw current text
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_PRIMARY);
    _canvas->setTextDatum(textdatum_t::middle_left);

    char displayText[MAX_NICKNAME_LENGTH + 2];
    strncpy(displayText, current, MAX_NICKNAME_LENGTH);
    displayText[MAX_NICKNAME_LENGTH] = '\0';

    // Add cursor character
    if (cursorPos < MAX_NICKNAME_LENGTH) {
        int len = strlen(displayText);
        if (cursorPos >= len) {
            // Pad with spaces if needed
            while (len < cursorPos) {
                displayText[len++] = ' ';
            }
            displayText[len] = selectedChar;
            displayText[len + 1] = '\0';
        }
    }

    _canvas->drawString(displayText, boxX + 8, y + boxHeight / 2);

    // Draw cursor underline
    int charWidth = 10;
    int cursorX = boxX + 8 + cursorPos * charWidth;
    _canvas->drawLine(cursorX, y + boxHeight - 6, cursorX + charWidth, y + boxHeight - 6, _themeColor);

    // Draw character selector hints
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::top_center);

    char hint[32];
    snprintf(hint, sizeof(hint), "< %c >", selectedChar);
    _canvas->drawString(hint, DISPLAY_CENTER_X, y + boxHeight + 10);

    // Draw instructions
    _canvas->setFont(&fonts::FreeSans9pt7b);
    _canvas->setTextColor(COLOR_TEXT_SECONDARY);
    _canvas->setTextDatum(textdatum_t::top_center);
    _canvas->drawString("Rotate: char  Click: next", DISPLAY_CENTER_X, y + boxHeight + 35);
}

char ScreenBaseYM::getNextChar(char current, int direction) {
    // Character set: A-Z, a-z, 0-9, space, dash, underscore
    static const char charSet[] = " ABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789-_";
    static const int charSetLen = sizeof(charSet) - 1;

    // Find current position
    int pos = 0;
    for (int i = 0; i < charSetLen; i++) {
        if (charSet[i] == current) {
            pos = i;
            break;
        }
    }

    // Move to next/prev
    pos = (pos + direction + charSetLen) % charSetLen;
    return charSet[pos];
}

void ScreenBaseYM::pushToDisplay() {
    _canvas->pushSprite(0, 0);
}

void ScreenBaseYM::buzzConfirm() {
    // Use consistent frequency to avoid LEDC clock source conflicts
    _hal->buzz.tone(2000, 30);
}

void ScreenBaseYM::buzzNavigate() {
    // Use same frequency, shorter duration
    _hal->buzz.tone(2000, 15);
}

void ScreenBaseYM::buzzError() {
    // Use same frequency, double beep pattern
    _hal->buzz.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(60));
    _hal->buzz.tone(2000, 50);
}

void ScreenBaseYM::buzzSuccess() {
    // Use same frequency, triple beep pattern with varying durations
    _hal->buzz.tone(2000, 50);
    vTaskDelay(pdMS_TO_TICKS(50));
    _hal->buzz.tone(2000, 75);
    vTaskDelay(pdMS_TO_TICKS(50));
    _hal->buzz.tone(2000, 100);
}

} // namespace YARD_MANAGEMENT
