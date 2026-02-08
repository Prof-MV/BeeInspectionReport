# Product Requirements Document
## Beehive Inspection App - Complete Inspection Flow

---

**Project:** M5Dial Beehive Inspection Recording System  
**Feature:** Complete Inspection Input Flow (App Module)  
**Version:** 1.0  
**Date:** February 8, 2026  
**Status:** Draft  

---

## 1. Executive Summary

This document defines the requirements for the complete inspection flow of the Beehive Inspection app on the M5Stack Dial device. The inspection flow consists of multiple sequential screens that guide beekeepers through recording essential hive health data during inspections. The workflow includes: Queen Right status, Queen Cells (supersedure/swarm), Supers count and fill, Temperament, Brood size, Treatment applied, and Pests identified. Each screen follows consistent interaction patterns optimized for single-handed field use with the M5Dial's rotary interface.

---

## 2. Objectives

### 2.1 Primary Goals
- Enable beekeepers to complete comprehensive hive inspections using the M5Dial's rotary interface
- Provide intuitive single-handed operation suitable for field use with gloves
- Establish consistent interaction patterns (rotate-click-save) across all inspection screens
- Ensure complete inspection data can be saved and integrated with RFID/database systems
- Support efficient data collection workflow that matches natural inspection sequence

### 2.2 Success Metrics
- Users can complete full inspection in < 2 minutes
- Zero failed saves due to interaction errors
- 100% consistency with defined interaction patterns across all screens
- Successful integration with MOONCAKE app lifecycle
- User satisfaction rating > 4.5/5 for workflow efficiency

### 2.3 Inspection Screens Overview
1. **Queen Right** - Is the hive queen right? (Yes/No)
2. **Queen Cells** - Supersedure and swarm cell counts
3. **Supers** - Number of supers and fill percentage
4. **Temperament** - Bee behavior observation
5. **Brood Size** - Number of brood frames (0-40)
6. **Treatment** - Mite control treatment applied
7. **Pests** - Identified pest problems

---

## 3. User Stories

### US-001: Record Queen Right Status
**As a** beekeeper conducting a hive inspection  
**I want to** quickly indicate whether the hive is queen right  
**So that** I can track queen health across inspections

**Acceptance Criteria:**
- Screen displays "Queen Right" as header
- User can rotate dial to toggle between "Yes" and "No"
- Current selection is clearly highlighted
- Single click confirms selection and advances to next screen
- Selection is stored in inspection record

### US-002: Record Queen Cell Counts
**As a** beekeeper monitoring swarm behavior  
**I want to** record the number of supersedure and swarm cells  
**So that** I can track colony reproduction plans and prevent swarming

**Acceptance Criteria:**
- Screen displays options for "Supersedure Cells" and "Swarm Cells"
- User can rotate to select which type to record
- Single click opens count selection for chosen type
- Count options: No, Yes, 1-5, 5-10, 10+
- User can record both types independently
- "Next" button advances to next screen

### US-003: Record Super Configuration
**As a** beekeeper managing honey production  
**I want to** record the number of supers and their fill levels  
**So that** I know when to harvest or add capacity

**Acceptance Criteria:**
- User can select number of supers (0-5)
- For each super, user can optionally record fill percentage (0%, 25%, 50%, 75%, 100%)
- User can skip fill percentage recording if desired
- "Next" button advances to next screen

### US-004: Record Bee Temperament
**As a** beekeeper tracking colony behavior  
**I want to** record bee temperament observations  
**So that** I can identify aggressive colonies or unusual behavior patterns

**Acceptance Criteria:**
- Options: Calm, Angry, Sticky, Festooning, Bearding, Washboarding
- User rotates to select temperament
- Single click confirms selection
- "Next" button advances to next screen

### US-005: Record Brood Frame Count
**As a** beekeeper assessing colony strength  
**I want to** count the number of brood frames  
**So that** I can monitor colony growth and health

**Acceptance Criteria:**
- User can select from 0 to 40 frames
- Rotation increments/decrements count by 1
- Current count is prominently displayed
- "Next" button advances to next screen

### US-006: Record Treatment Applied
**As a** beekeeper managing varroa mites  
**I want to** record which treatment I applied  
**So that** I can track treatment schedules and effectiveness

**Acceptance Criteria:**
- Options: Oxalic Acid, Hopguard, Formic Acid, Empty Drone Frame In, Full Drone Frame Out
- User rotates to select treatment
- Single click confirms selection
- "Next" button advances to next screen

### US-007: Record Pest Identification
**As a** beekeeper monitoring hive health  
**I want to** record any pests identified during inspection  
**So that** I can track pest pressure and take corrective action

**Acceptance Criteria:**
- Options: Wax Moth, Hive Beetle, American Foul Brood, European Foul Brood
- All default to "No"
- User can toggle each pest independently to "Yes"
- "Next" or "Save" button completes inspection

### US-008: Navigate Inspection Flow
**As a** beekeeper conducting an inspection  
**I want to** move forward and backward through inspection screens  
**So that** I can correct mistakes or review entered data

**Acceptance Criteria:**
- Single click advances to next screen (where applicable)
- Double-click returns to previous screen
- Long press saves all data and returns to main menu
- Data persists when navigating backward
- Visual feedback indicates current position in flow

### US-009: Save and Resume Inspection
**As a** beekeeper who may be interrupted  
**I want to** save my progress and return to main menu  
**So that** I can resume later or handle urgent matters

**Acceptance Criteria:**
- Long press (>800ms) from any screen triggers save
- All captured data is persisted
- User returns to main menu
- Visual/haptic feedback confirms save success
- Can resume inspection from last saved screen

---

## 4. Functional Requirements

### 4.0 Global Interaction Patterns

#### FR-000: Consistent Navigation
**Priority:** MUST HAVE  
**Description:** All screens follow consistent interaction patterns
- **Rotate dial**: Navigate options or adjust values
- **Single click**: Confirm selection / Advance to next screen
- **Double click**: Return to previous screen
- **Long press (>800ms)**: Save all data and return to main menu
- Visual feedback for all interactions
- Buzzer feedback for confirmations

---

### 4.1 Screen 1: Queen Right Status

#### FR-101: Header Display
**Priority:** MUST HAVE  
**Description:** Screen displays "Queen Right" as title text
- Position: Upper 25% of circular display
- Font: Bold, size appropriate for 240x240px display
- Color: High contrast against background
- Centered horizontally

#### FR-102: Selection Display
**Priority:** MUST HAVE  
**Description:** Current selection state is displayed
- Position: Center-lower 50% of circular display
- Options: "Yes" / "No"
- Active selection: Highlighted with distinct background color
- Inactive option: Dimmed or grayed out
- Font size: Large enough to read at arm's length (minimum 24pt equivalent)

#### FR-103: Queen Right Input Handling
**Priority:** MUST HAVE  
**Description:** Dial rotation toggles between Yes/No
- Rotation direction: Clockwise = next option, Counter-clockwise = previous option
- Wrap-around: Yes (rotate past "No" returns to "Yes")
- Visual feedback: Immediate update on screen
- Single click: Confirm and advance to Queen Cells screen

---

### 4.2 Screen 2: Queen Cells (Supersedure & Swarm)

#### FR-201: Two-Level Selection Interface
**Priority:** MUST HAVE  
**Description:** Screen allows selection of cell type, then count
- Header: "Queen Cells"
- Two main options displayed:
  - "Supersedure Cells"
  - "Swarm Cells"
- Layout: Vertical list format

#### FR-202: Cell Type Selection
**Priority:** MUST HAVE  
**Description:** First level - select which cell type to record
- Rotate dial: Highlight "Supersedure Cells" or "Swarm Cells"
- Visual indicator: Arrow or highlight box
- Current recorded values displayed next to each option (e.g., "Supersedure Cells: 1-5")

#### FR-203: Count Selection Sub-Menu
**Priority:** MUST HAVE  
**Description:** Second level - select count for chosen cell type
- Single click on highlighted cell type: Opens count selection
- Count options displayed to the right: "No", "Yes", "1-5", "5-10", "10+"
- Rotate dial: Navigate through count options
- Single click: Confirm count and return to cell type selection
- Value persists and displays next to cell type name

#### FR-204: Count Options Definition
**Priority:** MUST HAVE  
**Description:** Count selection values and their meanings
- **"No"**: No cells observed (default)
- **"Yes"**: Cells present but not counted
- **"1-5"**: Between 1 and 5 cells
- **"5-10"**: Between 5 and 10 cells
- **"10+"**: More than 10 cells

#### FR-205: Next Button
**Priority:** MUST HAVE  
**Description:** Navigation to next screen
- "Next" button displayed at bottom of screen
- Rotate dial to highlight "Next"
- Single click: Advance to Supers screen
- Can proceed even if no values recorded (allows skipping)

#### FR-206: Independent Cell Type Recording
**Priority:** MUST HAVE  
**Description:** Both cell types can be recorded independently
- User can record supersedure only, swarm only, both, or neither
- Each type retains its value independently
- Default: Both set to "No"

---

### 4.3 Screen 3: Supers Configuration

#### FR-301: Super Count Selection
**Priority:** MUST HAVE  
**Description:** Select number of supers on hive
- Header: "Supers"
- Primary display: Large number showing super count (0-5)
- Rotate dial: Increment/decrement super count
- Range: 0, 1, 2, 3, 4, 5
- Wrap-around: Yes (after 5, returns to 0)

#### FR-302: Fill Percentage Sub-Menu
**Priority:** MUST HAVE  
**Description:** Optional fill percentage for each super
- When super count > 0: Option to "Set Fill %" appears
- Rotate to highlight "Set Fill %"
- Single click: Opens fill percentage menu

#### FR-303: Fill Percentage Selection
**Priority:** MUST HAVE  
**Description:** Record fill level for each super
- Display: List of supers (Super 1, Super 2, etc.)
- Rotate dial: Select which super to set fill for
- Single click on super: Opens percentage options
- Percentage options: 0%, 25%, 50%, 75%, 100%
- Rotate through percentages, click to confirm
- Returns to super list
- Can set fill for all supers or skip

#### FR-304: Skip Fill Option
**Priority:** MUST HAVE  
**Description:** User can record count without fill
- "Skip Fill" option in menu
- Proceeds to next screen with only count recorded
- Fill values default to null/unset

#### FR-305: Next Button
**Priority:** MUST HAVE  
**Description:** Advance to temperament screen
- "Next" button always available
- Click to proceed to Temperament screen

---

### 4.4 Screen 4: Temperament

#### FR-401: Temperament Selection Display
**Priority:** MUST HAVE  
**Description:** Display behavior options
- Header: "Temperament"
- Options displayed in circular or list format:
  - Calm
  - Angry
  - Sticky
  - Festooning
  - Bearding
  - Washboarding
- Icons or color coding for each temperament (optional)

#### FR-402: Temperament Selection Input
**Priority:** MUST HAVE  
**Description:** Select observed bee behavior
- Rotate dial: Navigate through temperament options
- Current option highlighted
- Single click: Confirm selection
- Selection can be changed by rotating again before advancing

#### FR-403: Visual Indicators
**Priority:** SHOULD HAVE  
**Description:** Visual cues for temperament types
- Color scheme:
  - Calm: Green
  - Angry: Red
  - Others: Neutral/yellow tones
- Small icons representing each behavior
- Emoji-style faces (calm 😊, angry 😠, etc.)

#### FR-404: Next Button
**Priority:** MUST HAVE  
**Description:** Advance to brood size screen
- "Next" button at bottom
- Click to proceed to Brood Size screen

---

### 4.5 Screen 5: Brood Size

#### FR-501: Frame Count Display
**Priority:** MUST HAVE  
**Description:** Display current frame count prominently
- Header: "Brood Size" or "Brood Frames"
- Large central display: Current count (0-40)
- Font: Very large, easily readable
- Units: "frames" displayed

#### FR-502: Frame Count Adjustment
**Priority:** MUST HAVE  
**Description:** Increment/decrement frame count
- Rotate dial: Adjust count by 1 frame per click
- Range: 0 to 40 frames
- Clockwise: Increment, Counter-clockwise: Decrement
- Stops at 0 and 40 (no wrap-around)
- Visual feedback: Number changes immediately

#### FR-503: Fast Scroll Option
**Priority:** SHOULD HAVE  
**Description:** Faster navigation for large counts
- Fast rotation (>2 clicks per second): Increment by 5
- Allows quick movement to approximate value
- User can fine-tune with slower rotation

#### FR-504: Next Button
**Priority:** MUST HAVE  
**Description:** Advance to treatment screen
- "Next" button at bottom
- Click to proceed to Treatment screen

---

### 4.6 Screen 6: Treatment Applied

#### FR-601: Treatment Selection Display
**Priority:** MUST HAVE  
**Description:** Display treatment options
- Header: "Treatment Applied"
- Options displayed:
  - None
  - Oxalic Acid
  - Hopguard
  - Formic Acid
  - Empty Drone Frame In
  - Full Drone Frame Out

#### FR-602: Treatment Selection Input
**Priority:** MUST HAVE  
**Description:** Select treatment type
- Rotate dial: Navigate through treatment options
- Default: "None"
- Single click: Confirm treatment selection
- Current selection highlighted

#### FR-603: Drone Frame Timer Notification
**Priority:** SHOULD HAVE  
**Description:** Set reminder for drone frame removal
- If "Empty Drone Frame In" selected: Set 21-day calendar notification
- Visual confirmation: "Reminder set for [date]"
- Integration with RTC8563 for date calculation
- Notification stored for future calendar/alert feature

#### FR-604: Treatment Icons
**Priority:** SHOULD HAVE  
**Description:** Visual representation of treatments
- Small icon next to each treatment name
- Chemical treatments: Flask/beaker icon
- Drone frame: Frame icon
- Color coding for treatment categories

#### FR-605: Next Button
**Priority:** MUST HAVE  
**Description:** Advance to pests screen
- "Next" button at bottom
- Click to proceed to Pests screen

---

### 4.7 Screen 7: Pests Identified

#### FR-701: Pest Checklist Display
**Priority:** MUST HAVE  
**Description:** Display all pest options with status
- Header: "Pests Identified"
- Pest options with Yes/No toggles:
  - Wax Moth: No
  - Hive Beetle: No
  - American Foul Brood: No
  - European Foul Brood: No
- Checkbox or toggle indicator next to each

#### FR-702: Pest Selection Input
**Priority:** MUST HAVE  
**Description:** Toggle pest presence
- Rotate dial: Highlight pest option
- Single click: Toggle between "No" and "Yes"
- Visual feedback: Checkbox fills or toggle switches
- All default to "No"
- Multiple pests can be set to "Yes" independently

#### FR-703: Pest Visual Indicators
**Priority:** SHOULD HAVE  
**Description:** Visual warnings for serious pests
- American/European Foul Brood: Red warning icon if "Yes"
- Wax Moth/Hive Beetle: Yellow caution icon if "Yes"
- Alert message for foul brood: "CRITICAL - Requires immediate action"

#### FR-704: Completion Options
**Priority:** MUST HAVE  
**Description:** Finish or save inspection
- Two options at bottom:
  - "Finish": Save and return to main menu (long press alternative)
  - "Back": Return to previous screen (double-click alternative)
- Rotate to select, click to execute
- "Finish" triggers full inspection save

---

### 4.8 Global Navigation and Persistence

#### FR-801: Double-Click Back Navigation
**Priority:** MUST HAVE  
**Description:** Return to previous screen from any screen
- Double-click detected: Return to previous screen in flow
- Data entered on current screen is NOT saved
- Previous screen's data is restored
- Visual feedback: Brief reverse animation

#### FR-802: Long-Press Save and Exit
**Priority:** MUST HAVE  
**Description:** Save from any screen and return to main menu
- Long press (>800ms) from any screen
- Progress indicator during hold (circular fill animation)
- Saves all data entered up to current screen
- Haptic feedback: Buzzer beep on save confirmation
- Returns to main menu
- Incomplete screens default to null/unset values

#### FR-803: Screen Flow Order
**Priority:** MUST HAVE  
**Description:** Defined order of screens
1. Queen Right → 
2. Queen Cells → 
3. Supers → 
4. Temperament → 
5. Brood Size → 
6. Treatment → 
7. Pests → 
8. Main Menu (on completion)

#### FR-804: Progress Indicator
**Priority:** SHOULD HAVE  
**Description:** Show position in inspection flow
- Small dots or progress bar at edge of screen
- Shows: Current screen / Total screens (e.g., 3/7)
- Updates as user navigates
- Helps user understand how much remains

---

### 4.9 Data Management

#### FR-901: Comprehensive Data Structure
**Priority:** MUST HAVE  
**Description:** All inspection data is stored in unified structure
```cpp
struct InspectionRecord {
    uint32_t recordId;
    uint32_t hiveId;
    uint32_t timestampStart;
    uint32_t timestampEnd;
    
    // Screen 1: Queen Right
    QueenRightData queenRight;
    
    // Screen 2: Queen Cells
    QueenCellData queenCells;
    
    // Screen 3: Supers
    SupersData supers;
    
    // Screen 4: Temperament
    TemperamentData temperament;
    
    // Screen 5: Brood Size
    BroodSizeData broodSize;
    
    // Screen 6: Treatment
    TreatmentData treatment;
    
    // Screen 7: Pests
    PestsData pests;
    
    bool isComplete;
};
```

#### FR-902: Incremental Data Saving
**Priority:** MUST HAVE  
**Description:** Data persists as user navigates
- Each screen saves its data when advancing (single click)
- Data remains in memory during navigation
- Long press commits all data to persistent storage
- Incomplete inspection can be resumed

#### FR-903: Data Validation
**Priority:** MUST HAVE  
**Description:** Validate data before save
- Check for required fields (if any defined)
- Validate data ranges (e.g., supers 0-5, brood 0-40)
- Prevent impossible combinations
- Display error if validation fails

#### FR-904: RFID Integration Ready
**Priority:** SHOULD HAVE  
**Description:** Data structure compatible with RFID write
- Inspection record can be serialized to RFID format
- Timestamp from RTC8563 included
- Hive ID linkage maintained
- Follows format compatible with WS1850S RFID module

---

## 5. Technical Requirements

### 5.1 Hardware Interfaces

#### TR-001: Rotary Encoder
**Component:** Rotary encoder  
**Specifications:**
- Resolution: 16 positions, 64 pulses/revolution
- Pins: GPIO40 (B), GPIO41 (A)
- Pull-up: Internal pull-ups enabled
- Interrupt: Pin change interrupt on both pins
- Sampling: 1kHz minimum

#### TR-002: Display Driver
**Component:** GC9A01 LCD driver  
**Specifications:**
- Resolution: 240x240 pixels
- Interface: SPI
- Pins: G4 (RS), G5 (MOSI), G6 (SCK), G7 (CS), G8 (RESET), G9 (BL)
- Refresh rate: Minimum 30fps for smooth updates
- Color depth: 16-bit (RGB565)

#### TR-003: Touch Input
**Component:** FT3267 touch controller  
**Specifications:**
- Interface: I2C (address 0x38)
- Pins: G11 (TP_SDA), G12 (SCL)
- Touch detection: Capacitive
- Response time: < 50ms

#### TR-004: RTC Integration
**Component:** BM8563 RTC  
**Specifications:**
- Interface: I2C (address 0x51)
- Pins: G12 (SCL), G11 (SDA)
- Purpose: Timestamp inspection records
- Accuracy: ±5 ppm

#### TR-005: Buzzer Feedback
**Component:** Onboard buzzer  
**Specifications:**
- Pin: GPIO3
- Volume: 80dB
- Patterns:
  - Single click: Short beep (50ms)
  - Long press complete: Double beep (100ms, 50ms gap, 100ms)
  - Error: Triple beep (100ms, 50ms gap pattern)

### 5.2 Software Architecture

#### TR-006: MOONCAKE Framework Integration
**Framework:** MOONCAKE APP_BASE  
**Requirements:**
- Inherit from MOONCAKE::APP_BASE class
- Implement required lifecycle methods:
  - `onSetup()`: Initialize hardware, load resources, setup all screens
  - `onCreate()`: Setup initial screen state (Screen 1: Queen Right)
  - `onResume()`: Restore state if returning from background
  - `onRunning()`: Main loop for input handling, screen rendering, and flow control
  - `onPause()`: Save state if backgrounded
  - `onDestroy()`: Cleanup resources, save incomplete inspection
- Set app name: "Beehive Inspection"
- Set app icon: Beehive or bee icon
- Background running: Not allowed (`setAllowBgRunning(false)`)
- Screen management: Navigate through 7 screens in sequence
- Data persistence: Maintain InspectionRecord throughout lifecycle

#### TR-007: File Structure
**Location:** `/apps/beehive_inspection/`  
**Files:**
```
apps/
└── beehive_inspection/
    ├── beehive_inspection.h              // Main app class declaration
    ├── beehive_inspection.cpp            // App implementation & flow control
    ├── inspection_data.h                 // All data structures
    ├── inspection_data.cpp               // Data management functions
    │
    ├── screens/
    │   ├── screen_queen_right.h          // Screen 1: Queen Right
    │   ├── screen_queen_right.cpp
    │   ├── screen_queen_cells.h          // Screen 2: Queen Cells
    │   ├── screen_queen_cells.cpp
    │   ├── screen_supers.h               // Screen 3: Supers
    │   ├── screen_supers.cpp
    │   ├── screen_temperament.h          // Screen 4: Temperament
    │   ├── screen_temperament.cpp
    │   ├── screen_brood_size.h           // Screen 5: Brood Size
    │   ├── screen_brood_size.cpp
    │   ├── screen_treatment.h            // Screen 6: Treatment
    │   ├── screen_treatment.cpp
    │   ├── screen_pests.h                // Screen 7: Pests
    │   └── screen_pests.cpp
    │
    ├── ui/
    │   ├── inspection_ui_base.h          // Common UI drawing functions
    │   ├── inspection_ui_base.cpp
    │   ├── ui_colors.h                   // Color definitions
    │   ├── ui_animations.h               // Animation helpers
    │   └── ui_animations.cpp
    │
    └── assets/
        ├── icon_queen.png                // Queen bee icon
        ├── icon_cells.png                // Queen cells icon
        ├── icon_supers.png               // Supers icon
        ├── icon_temperament.png          // Bee behavior icon
        ├── icon_brood.png                // Brood icon
        ├── icon_treatment.png            // Treatment icon
        └── icon_pests.png                // Pests icon
```

#### TR-008: State Management
**State Machine:**
```
App-Level States:
- SCREEN_QUEEN_RIGHT (1)
- SCREEN_QUEEN_CELLS (2)
- SCREEN_SUPERS (3)
- SCREEN_TEMPERAMENT (4)
- SCREEN_BROOD_SIZE (5)
- SCREEN_TREATMENT (6)
- SCREEN_PESTS (7)

Screen-Specific States:
- IDLE: Waiting for input
- ROTATING: User is rotating dial
- SELECTING: Navigating options
- SUBMENU: In sub-menu (Queen Cells count, Super fill, etc.)
- CLICKED: Single click detected
- DOUBLE_CLICKED: Double click detected
- LONG_PRESSING: Long press in progress
- SAVING: Data being saved
- TRANSITIONING: Moving to next/previous screen

State transitions handled in main app onRunning() loop
Each screen implements its own state handling
Global navigation (double-click, long-press) handled at app level
```

#### TR-009: Memory Requirements
- Flash: < 100KB for app code and assets
- RAM: < 50KB during runtime
- Stack: < 4KB per app
- No dynamic memory allocation in ISR handlers

### 5.3 Dependencies

#### TR-010: Required Libraries
```cpp
#include <M5Dial.h>              // M5Dial hardware abstraction
#include "utilities/gui_base/gui_base.h"  // GUI framework
#include "common_define.h"        // Common definitions
#include <Wire.h>                 // I2C communication
#include <SPI.h>                  // SPI for display
```

#### TR-011: Shared Data Structures
```cpp
// Defined in shared inspection_data.h
struct InspectionRecord {
    uint32_t hiveId;
    uint32_t timestamp;
    QueenRightData queenRight;
    // ... other inspection fields
};
```

---

## 6. User Interface Specifications

### 6.1 Visual Design - Screen Layouts

#### UI-001: Screen 1 - Queen Right Layout
**Circular Display (240x240px):**
```
┌─────────────────────────┐
│                         │
│     Queen Right         │  ← Header (0-60px)
│         [👑]            │  ← Icon (optional)
│                         │
│    ┌───────────┐        │
│    │   YES     │        │  ← Selected option
│    └───────────┘        │     (80-160px, highlighted)
│                         │
│        No              │  ← Unselected option
│                         │     (160-200px, dimmed)
│                         │
│   [Rotate to select]   │  ← Hint text (200-240px)
└─────────────────────────┘
```

#### UI-002: Screen 2 - Queen Cells Layout
**Two-column selection interface:**
```
┌─────────────────────────┐
│    Queen Cells          │  ← Header
│                         │
│ ▶ Supersedure     No    │  ← Selected, value shown
│                         │
│   Swarm Cells     1-5   │  ← Not selected, value shown
│                         │
│   [Next]               │  ← Navigation button
│                         │
│   Screen 2/7           │  ← Progress indicator
└─────────────────────────┘

// When cell type clicked:
┌─────────────────────────┐
│  Supersedure Cells      │  ← Sub-header
│                         │
│        No              │
│  ┌─────────────┐       │
│  │   Yes       │       │  ← Selected count
│  └─────────────┘       │
│       1-5              │
│       5-10             │
│       10+              │
└─────────────────────────┘
```

#### UI-003: Screen 3 - Supers Layout
**Number display with sub-menu:**
```
┌─────────────────────────┐
│       Supers            │  ← Header
│                         │
│         ┌───┐           │
│         │ 3 │           │  ← Large number
│         └───┘           │
│                         │
│   [Set Fill %]         │  ← Option
│   [Skip Fill]          │
│   [Next]               │
└─────────────────────────┘

// Fill percentage sub-menu:
┌─────────────────────────┐
│     Super Fill %        │
│                         │
│ ▶ Super 1      75%      │  ← Selected super
│   Super 2      50%      │
│   Super 3      --       │  ← Not set
│                         │
│   [Done]               │
└─────────────────────────┘

// Individual super fill:
┌─────────────────────────┐
│      Super 1            │
│                         │
│   0%    25%   50%       │
│  ┌────┐               │
│  │75% │  100%          │  ← Selected
│  └────┘               │
└─────────────────────────┘
```

#### UI-004: Screen 4 - Temperament Layout
**Circular selector or list:**
```
┌─────────────────────────┐
│    Temperament          │  ← Header
│                         │
│        Calm  😊         │
│   ┌─────────────┐       │
│   │  Angry 😠   │       │  ← Selected
│   └─────────────┘       │
│       Sticky            │
│     Festooning          │
│      Bearding           │
│   Washboarding          │
│                         │
│      [Next]            │
└─────────────────────────┘
```

#### UI-005: Screen 5 - Brood Size Layout
**Large number display:**
```
┌─────────────────────────┐
│    Brood Frames         │  ← Header
│                         │
│                         │
│       ┌─────┐           │
│       │ 18  │           │  ← Very large count
│       └─────┘           │
│       frames            │
│                         │
│    [- 5]  [- 1]        │  ← Optional fast buttons
│                         │
│      [Next]            │
└─────────────────────────┘
```

#### UI-006: Screen 6 - Treatment Layout
**Icon-based selection:**
```
┌─────────────────────────┐
│  Treatment Applied      │  ← Header
│                         │
│        None             │
│  ┌──────────────────┐   │
│  │ 🧪 Oxalic Acid  │   │  ← Selected
│  └──────────────────┘   │
│    💊 Hopguard          │
│    🧪 Formic Acid       │
│    🔲 Drone Frame In    │
│    🔲 Drone Frame Out   │
│                         │
│      [Next]            │
└─────────────────────────┘
```

#### UI-007: Screen 7 - Pests Layout
**Checklist format:**
```
┌─────────────────────────┐
│  Pests Identified       │  ← Header
│                         │
│  ☐ Wax Moth            │
│  ☑ Hive Beetle         │  ← Checked (Yes)
│  ☐ American Foulbrood  │
│  ☐ European Foulbrood  │
│                         │
│  ⚠️ 1 pest detected     │  ← Summary
│                         │
│     [Finish]           │  ← Completion
└─────────────────────────┘
```

### 6.2 Color Scheme

#### UI-008: Universal Color Palette
- **Background**: Dark blue/black (#1a1a2e)
- **Header text**: White (#ffffff)
- **Selected option background**: Orange (#ff6b35) - M5Stack brand color
- **Selected option text**: White (#ffffff)
- **Unselected option text**: Gray (#808080)
- **Accent color**: Yellow/gold (#ffd700)
- **Success**: Green (#00ff00)
- **Warning**: Yellow (#ffcc00)
- **Error/Critical**: Red (#ff0000)

#### UI-009: Screen-Specific Colors
- **Queen Right - Yes**: Green tint background
- **Queen Right - No**: Red tint background
- **Queen Cells**: Neutral with count highlights
- **Supers - Fill levels**:
  - 0-25%: Red
  - 26-50%: Orange
  - 51-75%: Yellow
  - 76-100%: Green
- **Temperament**:
  - Calm: Green (#00ff00)
  - Angry: Red (#ff0000)
  - Others: Yellow/Orange (#ffcc00)
- **Pests - Foul Brood**: Critical red with warning icon

### 6.3 Typography

#### UI-010: Font Sizes and Weights
- **Headers**: Bold, 18-20px
- **Large numbers** (counts): Bold, 40-48px
- **Options/selections**: Bold, 24-32px
- **Secondary text**: Regular, 16-18px
- **Hint text**: Regular, 14px
- **Font family**: System default, Roboto, or Arial

### 6.4 Animations and Transitions

#### UI-011: Rotation Feedback
- Duration: 150ms ease-out
- Effect: Selected item scales slightly (1.0 → 1.1 → 1.0)
- Concurrent: Background color fade transition
- Haptic: Light buzzer tick (optional)

#### UI-012: Click Feedback
- Duration: 100ms
- Effect: Brief white flash overlay or scale pulse
- Sound: Short beep from buzzer (50ms, medium volume)

#### UI-013: Long Press Indicator
- Duration: 800ms (full press time)
- Effect: Circular progress ring around edge of screen
- Color: Green (#00ff00)
- Update: Every 100ms (8 steps total)
- Text appears: "Saving..."

#### UI-014: Screen Transitions
- Forward navigation: Slide left (200ms, ease-in-out)
- Backward navigation: Slide right (200ms, ease-in-out)
- Fade overlay during transition for smoothness

#### UI-015: Value Change Animation
- Number increment/decrement: Smooth count-up animation
- Checkbox toggle: Fill/unfill with bounce effect
- Percentage slider: Smooth bar fill animation

### 6.5 Icons and Visual Elements

#### UI-016: Icon Set
- **Queen**: Crown emoji or stylized queen bee icon (👑 or 🐝)
- **Queen Cells**: Honeycomb with larvae
- **Supers**: Stacked boxes
- **Temperament**: Emoji faces (😊😠 etc.) or bee behavior icons
- **Brood**: Honeycomb pattern with larvae
- **Treatment**: Flask/beaker for chemicals, frame icon for drone frames
- **Pests**: Specific icons for moth, beetle, disease indicators

#### UI-017: Progress Indicator
- Position: Bottom edge or around perimeter
- Style: Dots (• • ● • • • •) or mini progress bar
- Updates: As user advances through screens
- Shows: Current screen / Total (e.g., "3/7")

### 6.6 Accessibility and Usability

#### UI-018: Glove-Friendly Design
- Touch target size: Minimum 80px diameter for clickable elements
- Spacing: Minimum 20px between interactive elements
- High contrast: WCAG AA compliant (4.5:1 minimum)
- Large text: 24px+ for primary content

#### UI-019: Outdoor Visibility
- Brightness: Maximum backlight setting (GPIO9 at 100%)
- Contrast: High contrast mode with black backgrounds
- Text: Bold weights preferred for readability
- Colors: Avoid low-contrast combinations (e.g., light gray on white)

#### UI-020: Visual Feedback Requirements
- Every rotation: Immediate visual change (< 50ms latency)
- Every click: Visual + audio feedback
- Every long press: Progress indicator + audio confirmation
- Errors: Red flash + error beep pattern

---

## 7. Error Handling and Edge Cases

### 7.1 Error Scenarios

#### EH-001: Hardware Initialization Failure
**Scenario:** Display, encoder, or I2C devices fail to initialize
**Handling:**
- Log error to serial console
- Display error message on screen (if display functional)
- Trigger error beep pattern (if buzzer functional)
- Attempt recovery: Retry initialization 3 times with 1s delay
- If all retries fail: Return to main menu with error flag

#### EH-002: RTC Read Failure
**Scenario:** Cannot read time from RTC8563
**Handling:**
- Use system uptime as fallback timestamp
- Set flag indicating timestamp is relative, not absolute
- Log warning to console
- Continue operation (non-critical failure)

#### EH-003: Save Operation Failure
**Scenario:** Data cannot be persisted (storage full, corruption, etc.)
**Handling:**
- Display error message: "Save Failed - Retry?"
- Options: "Retry" / "Cancel"
- User can retry save operation
- If cancelled, data remains in memory but marked as unsaved
- Log error details for debugging

#### EH-004: Button Debounce Glitches
**Scenario:** Mechanical bounce or EMI causes false triggers
**Handling:**
- Software debounce: 50ms minimum between valid clicks
- State machine prevents invalid transitions
- Ignore inputs during transition animations

### 7.2 Edge Cases

#### EC-001: Rapid Rotation
**Scenario:** User rotates dial very quickly
**Handling:**
- Limit state changes to once per 100ms
- Queue only the latest rotation direction
- Visual feedback kept smooth despite input flooding

#### EC-002: Ambiguous Multi-Press
**Scenario:** User presses between single/double click timing
**Handling:**
- Use timer-based detection: Wait 500ms after first click
- If second click within window: Treat as double-click
- If timeout: Treat as single click
- Provide immediate visual feedback on first click to indicate system registered input

#### EC-003: Long Press Abandonment
**Scenario:** User begins long press but releases before 800ms
**Handling:**
- Cancel save operation
- Progress ring fades out
- Return to normal state
- No action taken

#### EC-004: Screen Timeout
**Scenario:** No user input for extended period (configurable, default 5 minutes)
**Handling:**
- Dim backlight to 20%
- Touch or rotation wakes screen
- No data loss
- Resume exact previous state

#### EC-005: Low Battery During Save
**Scenario:** Battery voltage drops during save operation
**Handling:**
- Monitor battery voltage (GPIO for ADC reading)
- If voltage < 3.3V during save: Prioritize save completion
- Display low battery warning after save
- Disable long-running operations if voltage critical

---

## 8. Testing Requirements

### 8.1 Unit Tests

#### UT-001: Encoder Input Processing
**Test Cases:**
- Clockwise rotation changes selection correctly on all screens
- Counter-clockwise rotation changes selection correctly
- Rapid rotation does not cause glitches or missed states
- Encoder state is correctly read from GPIO40/41
- Wrap-around works correctly where applicable

#### UT-002: Click Detection Logic
**Test Cases:**
- Single click (< 300ms press) is correctly identified
- Double click (two clicks within 500ms) is correctly identified
- Long press (> 800ms) is correctly identified
- Ambiguous timing defaults to safe action
- Click detection works consistently across all screens

#### UT-003: Data Structure Validation
**Test Cases:**
- All data structures correctly initialize with default values
- isSet flags properly indicate selection status
- Timestamps are correctly captured from RTC
- Data serializes/deserializes without loss
- Enums stay within valid ranges
- Bit-packed format correctly encodes/decodes all fields

#### UT-004: State Machine Transitions
**Test Cases:**
- All valid screen transitions execute correctly
- Invalid transitions are blocked
- State recovery after error works as expected
- Sub-menu states (Queen Cells count, Super fill) work correctly
- Double-click back navigation works from all screens
- Long-press save works from all screens

#### UT-005: Screen-Specific Logic
**Test Cases for Each Screen:**
- **Queen Right**: Yes/No toggle works correctly
- **Queen Cells**: Two-level selection for both supersedure and swarm
- **Supers**: Count selection (0-5) and optional fill percentages
- **Temperament**: All 6 temperament options selectable
- **Brood Size**: Count from 0-40, no wrap-around at limits
- **Treatment**: All treatment types selectable, drone frame timer calculation
- **Pests**: All 4 pests independently toggle between No/Yes

#### UT-006: Data Persistence
**Test Cases:**
- Each screen saves data when advancing
- Data persists when navigating backward
- Long press saves all collected data
- Incomplete inspections can be resumed
- InspectionRecord completion percentage calculates correctly

### 8.2 Integration Tests

#### IT-001: Hardware Integration
**Test Cases:**
- Display renders correctly on GC9A01 for all screen layouts
- Encoder input is accurately captured
- Touch input is responsive
- RTC provides valid timestamps
- Buzzer produces audible feedback for all interaction types
- All GPIO pins correctly mapped and functioning

#### IT-002: MOONCAKE Framework Integration
**Test Cases:**
- App installs correctly in app manager
- onCreate/onResume/onPause/onDestroy lifecycle works
- App data persists across lifecycle events
- Navigation to/from main menu functions correctly
- Multiple inspection sessions can be started and completed

#### IT-003: Screen Flow Integration
**Test Cases:**
- Forward navigation (click) advances through all 7 screens in order
- Backward navigation (double-click) returns to previous screen
- Data from previous screens is accessible in later screens
- Screen transitions are smooth and error-free
- Progress indicator updates correctly

#### IT-004: Data Integration
**Test Cases:**
- Complete inspection creates valid InspectionRecord
- Partial inspection saves correctly with lastCompletedScreen marker
- RFID-compatible data format can be generated
- Database sync format is correctly generated
- Timestamps are consistent across all inspection data

### 8.3 System Tests

#### ST-001: End-to-End Complete Inspection
**Scenario:** User completes full inspection workflow
**Steps:**
1. Launch app from main menu
2. Screen 1: Select "Yes" for Queen Right
3. Screen 2: Select 1-5 supersedure cells, No swarm cells, click Next
4. Screen 3: Select 3 supers, set fills to 50%, 75%, 100%, click Next
5. Screen 4: Select "Calm" temperament, click Next
6. Screen 5: Set brood count to 18 frames, click Next
7. Screen 6: Select "Oxalic Acid" treatment, click Next
8. Screen 7: Toggle "Hive Beetle" to Yes, click Finish
9. Verify all data saved correctly
10. Verify return to main menu

**Expected Result:** Complete inspection in < 2 minutes, all data accurately captured

#### ST-002: Partial Inspection with Save
**Scenario:** User completes partial inspection and saves
**Steps:**
1. Complete screens 1-3
2. On screen 4, long-press to save
3. Verify data saved with lastCompletedScreen = 3
4. Verify return to main menu
5. Re-launch app
6. Verify can resume from screen 4

**Expected Result:** Partial data persists, can resume inspection

#### ST-003: Back Navigation Through All Screens
**Scenario:** User navigates backward through inspection
**Steps:**
1. Complete screens 1-5
2. Double-click on screen 6
3. Verify return to screen 5 with data intact
4. Double-click on screen 5
5. Verify return to screen 4 with data intact
6. Continue double-clicking back to screen 1
7. Verify all data preserved

**Expected Result:** Backward navigation works, no data loss

#### ST-004: Sub-Menu Navigation
**Scenario:** Test sub-menus (Queen Cells, Supers)
**Steps:**
1. On Queen Cells screen, select Supersedure Cells
2. Choose count "1-5"
3. Verify return to main menu with value displayed
4. Select Swarm Cells
5. Choose count "No"
6. Click Next
7. On Supers screen, set count to 3
8. Enter Set Fill %
9. Set Super 1 to 75%, Super 2 to 50%, skip Super 3
10. Verify values saved

**Expected Result:** Sub-menus work correctly, values persist

#### ST-005: Error Recovery
**Scenario:** Simulate hardware failures and verify recovery
**Steps:**
1. Simulate RTC read failure
2. Launch app and complete inspection
3. Verify fallback timestamp used
4. Simulate encoder glitch (noise)
5. Verify debouncing prevents false inputs
6. Simulate low memory condition
7. Verify graceful degradation

**Expected Result:** App handles errors gracefully, continues to function

#### ST-006: Drone Frame Reminder
**Scenario:** Test treatment reminder calculation
**Steps:**
1. Complete inspection to Treatment screen
2. Select "Empty Drone Frame In"
3. Note current date from RTC
4. Complete inspection
5. Verify droneFrameRemovalDate = current + 21 days
6. Verify date displayed to user

**Expected Result:** Reminder date correctly calculated and displayed

### 8.4 User Acceptance Tests

#### UAT-001: Field Usability
**Participants:** 5-8 beekeepers with varying technical experience
**Tasks:**
- Complete full inspection while wearing beekeeping gloves
- Complete inspection in bright outdoor sunlight
- Use device while holding hive frame in other hand
- Complete inspection within 2 minutes

**Success Criteria:**
- 100% task completion rate
- Average completion time < 2 minutes
- User satisfaction rating > 4/5
- < 10% error rate (wrong selections)

#### UAT-002: Interaction Intuitiveness
**Participants:** First-time users (beekeepers)
**Tasks:**
- Complete full inspection with minimal instruction
- Use double-click to go back and correct mistake
- Use long-press to save and exit mid-inspection
- Resume saved inspection

**Success Criteria:**
- > 80% can complete full inspection without help
- > 90% understand rotation and click patterns
- > 70% successfully use double-click back navigation
- User confidence rating > 4/5

#### UAT-003: Data Accuracy
**Participants:** Experienced beekeepers
**Tasks:**
- Complete 5 inspections with known hive states
- Compare recorded data to actual hive conditions
- Verify all options are accessible and understandable

**Success Criteria:**
- 100% data accuracy (recorded = actual)
- All terminology understood by beekeepers
- No missing data fields identified
- Suggested improvements < 3 per tester

#### UAT-004: Workflow Efficiency
**Participants:** Active beekeepers managing 10+ hives
**Tasks:**
- Complete 10 consecutive inspections
- Compare time to paper-based recording
- Rate ease of use vs. traditional methods

**Success Criteria:**
- M5Dial method ≤ paper method time
- Preference for digital: > 70%
- Perceived accuracy: Higher than paper
- Would recommend to others: > 80%

---

## 9. Performance Requirements

### 9.1 Response Times

#### PERF-001: Input Latency
- Encoder rotation to screen update: < 100ms
- Button press to visual feedback: < 50ms
- Touch input response: < 50ms
- Sub-menu transitions: < 150ms

#### PERF-002: Screen Rendering
- Frame rate: Minimum 30fps during animations
- Static screen: Update only on state change (power saving)
- Screen transition animations: 200ms maximum duration
- Sub-menu open/close: 150ms transition

#### PERF-003: Save Operations
- Screen-to-screen data save: < 20ms (in-memory)
- Long press full save to persistent storage: < 500ms
- RFID write (future): < 1000ms
- RTC timestamp capture: < 10ms

#### PERF-004: Complete Inspection Timing
- Minimum time to complete all 7 screens: 60 seconds (power user)
- Target time for average user: 90-120 seconds
- Maximum acceptable time: 180 seconds (3 minutes)

### 9.2 Resource Utilization

#### PERF-005: Memory Usage
- Heap usage: < 100KB total (all screens + data)
- Stack usage per screen: < 4KB
- InspectionRecord structure: < 1KB
- No memory leaks over 100 complete inspection cycles
- Texture/icon assets: < 50KB total

#### PERF-006: Power Consumption
- Active operation: < 150mA @ 6V
- Idle (screen dimmed): < 50mA @ 6V
- Sleep mode: < 2mA @ 4.2V (battery powered)
- Battery life target: > 8 hours continuous use (50+ inspections)
- Battery life with intermittent use: > 16 hours (field day)

#### PERF-007: CPU Usage
- Main loop: < 60% CPU utilization average
- Peak CPU (during transitions): < 90%
- ISR processing: < 1% total CPU time
- Display updates: < 25% CPU utilization
- Idle time available for background tasks: > 30%

### 9.3 Scalability

#### PERF-008: Data Volume
- Support for 1000+ inspections in memory/storage
- RFID tag capacity: 10-20 inspection records
- Database sync: Batch upload 100+ records without performance degradation

#### PERF-009: Screen Rendering Complexity
- Maximum icons per screen: 10
- Maximum text elements per screen: 15
- Maximum interactive elements per screen: 7
- Smooth animation even with full screen layout

---

## 10. Security and Data Privacy

### 10.1 Data Security

#### SEC-001: Data Integrity
- CRC/checksum validation for saved data
- Atomic write operations (all-or-nothing)
- Version stamping of data structures

#### SEC-002: RFID Security (Future)
- Read protection of sensitive RFID blocks
- Authentication before write operations
- Encryption of data written to RFID tags

### 10.2 Privacy Considerations

#### PRIV-001: Data Minimization
- Only essential inspection data collected
- No personal identifiable information stored without consent
- Data retention: User configurable

---

## 11. Documentation Requirements

### 11.1 Code Documentation

#### DOC-001: Code Comments
- Header comments for all functions
- Inline comments for complex logic
- State machine documentation
- Hardware pin definitions clearly commented

#### DOC-002: API Documentation
- Public methods documented with Doxygen format
- Parameter descriptions
- Return value explanations
- Usage examples

### 11.2 User Documentation

#### DOC-003: In-App Help
- Brief tooltip on first launch explaining controls
- Visual indicators (arrows) showing rotation direction
- Icon legend if multiple icons used

#### DOC-004: User Manual Section
- Description of queen right selection process
- Explanation of Yes/No significance
- Troubleshooting common issues
- Screenshots of expected screens

---

## 12. Deployment and Versioning

### 12.1 Version Management

#### VER-001: Version Numbering
- Format: MAJOR.MINOR.PATCH (e.g., 1.0.0)
- Increment rules:
  - MAJOR: Breaking changes to data structure or API
  - MINOR: New features, backward compatible
  - PATCH: Bug fixes, no feature changes

#### VER-002: Compatibility
- Data structure versioning for forward/backward compatibility
- Migration scripts for data structure changes
- Clear changelog in code repository

### 12.2 Build Configuration

#### BUILD-001: Compilation Flags
```cpp
#define APP_VERSION "1.0.0"
#define DEBUG_MODE 0              // 0 = production, 1 = debug
#define ENABLE_SERIAL_LOG 1       // Console logging
#define BUZZER_ENABLED 1          // Haptic feedback
```

#### BUILD-002: Platform Target
- Hardware: M5Stack Dial (K130)
- MCU: ESP32-S3
- Framework: Arduino or ESP-IDF
- IDE: Arduino IDE or PlatformIO

---

## 13. Dependencies and Prerequisites

### 13.1 Hardware Dependencies

#### DEP-001: Required Hardware
- M5Stack Dial device (SKU: K130)
- RFID cards (standard credit card size, ISO/IEC 14443 Type A/B)
- Power supply: USB-C or 6-36V DC or LiPo battery (1.25mm-2P connector)

### 13.2 Software Dependencies

#### DEP-002: Required Libraries
- M5Dial library (official M5Stack)
- LovyanGFX (display driver)
- FastLED (optional, for LED effects if added)
- ArduinoJson (for data serialization)
- Wire library (I2C)
- SPI library

#### DEP-003: Development Environment
- Arduino IDE 2.x or PlatformIO
- ESP32 board support package v2.0.x or later
- USB-C cable for programming
- Serial monitor for debugging

### 13.3 Prior System Requirements

#### DEP-004: Existing System Components
- Main menu app (must exist to return to)
- Inspection data management module
- RFID handler module (for future RFID integration)
- Database/storage module

---

## 14. Future Enhancements (Out of Scope for v1.0)

### 14.1 Planned Features for v2.0

#### FE-001: Image Capture Integration
- Add camera module support for documenting hive conditions
- Photo attachments for pests, diseases, queen sightings
- Image storage and sync with database

#### FE-002: Yard Management Integration
- Select yard before inspection
- Multi-hive batch inspections
- Yard-level statistics and summaries

#### FE-003: Historical Data Display
- Show previous inspection data on relevant screens
- Trend graphs for brood size, temperament, etc.
- Warning alerts for significant changes

#### FE-004: Weather Integration
- Auto-record weather conditions during inspection
- Correlate bee behavior with weather
- Optimal inspection time suggestions

#### FE-005: GPS Location Tagging
- Auto-record inspection location
- Map view of all hive locations
- Distance calculations for travel planning

#### FE-006: Quick Inspection Mode
- Abbreviated inspection for experienced users
- "Everything OK" quick-check option
- Flag-only-issues workflow

#### FE-007: Custom Fields
- User-definable inspection fields
- Configurable option lists
- Field templates for different bee types (European, Africanized, etc.)

#### FE-008: Voice Notes
- Audio recording for detailed observations
- Speech-to-text for notes field
- Playback of previous inspection notes

#### FE-009: Uncertain/Unknown Options
- Add "?" option to all screens for ambiguous situations
- Track confidence level in observations
- Suggestions based on uncertain data

#### FE-010: Multi-Language Support
- Internationalization framework
- Initial languages: English, Spanish, German, French, Italian
- Language selection in settings

#### FE-011: Cloud Sync and Backup
- Automatic cloud backup of all inspections
- Multi-device synchronization
- Web dashboard for data analysis

#### FE-012: AI-Assisted Diagnosis
- Pattern recognition for disease/pest identification
- Predictive analytics for swarming
- Treatment recommendations based on inspection data

#### FE-013: Beekeeping Calendar
- Scheduled inspection reminders
- Treatment schedule tracking
- Seasonal task lists

#### FE-014: Export and Reporting
- PDF inspection reports
- CSV export for analysis
- Graphs and visualizations
- Email/share inspection summaries

#### FE-015: Collaborative Features
- Share inspections with mentors/partners
- Team inspection workflows
- Comments and discussion threads

---

## 15. Success Criteria and Acceptance

### 15.1 Definition of Done

The Beehive Inspection feature is considered complete when:

1. ✅ All MUST HAVE functional requirements implemented for all 7 screens
2. ✅ All unit tests pass with > 90% code coverage
3. ✅ All integration tests pass
4. ✅ All system tests pass
5. ✅ At least 5 UAT participants successfully complete full inspections
6. ✅ Performance requirements met in real-world field testing
7. ✅ Average inspection completion time < 2 minutes
8. ✅ Code reviewed and approved by lead developer
9. ✅ Documentation complete (code comments + user guide)
10. ✅ No critical or high-priority bugs remain
11. ✅ Successfully integrates with main menu and RFID system (if applicable)
12. ✅ Power consumption within specified limits (8+ hours battery life)
13. ✅ All screen transitions smooth and error-free
14. ✅ Data persistence works across all lifecycle events
15. ✅ Buzzer feedback appropriate and consistent

### 15.2 Acceptance Checklist

**Global Navigation:**
- [ ] Rotary encoder accurately controls all screen types
- [ ] Single click advances through all 7 screens
- [ ] Double click returns to previous screen from any screen
- [ ] Long press saves all data and returns to main menu from any screen
- [ ] Visual feedback is clear and responsive on all screens
- [ ] Buzzer provides appropriate audio feedback for all interaction types

**Screen 1: Queen Right**
- [ ] Yes/No selection works correctly
- [ ] Data saves when advancing to next screen

**Screen 2: Queen Cells**
- [ ] Two-level menu (cell type → count) works correctly
- [ ] Supersedure and swarm cells can be set independently
- [ ] All count options accessible (No, Yes, 1-5, 5-10, 10+)
- [ ] Values persist and display correctly
- [ ] Next button advances to Supers screen

**Screen 3: Supers**
- [ ] Super count selection (0-5) works correctly
- [ ] Fill percentage sub-menu accessible
- [ ] All supers can have fill % set independently
- [ ] Skip Fill option works
- [ ] Next button advances to Temperament screen

**Screen 4: Temperament**
- [ ] All 6 temperament options selectable
- [ ] Visual indicators (colors/emojis) display correctly
- [ ] Next button advances to Brood Size screen

**Screen 5: Brood Size**
- [ ] Count adjusts from 0-40 with rotation
- [ ] No wrap-around at 0 and 40
- [ ] Large number display clearly visible
- [ ] Fast scroll (optional) works if implemented
- [ ] Next button advances to Treatment screen

**Screen 6: Treatment**
- [ ] All treatment options selectable
- [ ] Drone frame reminder calculates correctly (21 days)
- [ ] Next button advances to Pests screen

**Screen 7: Pests**
- [ ] All 4 pests independently toggleable
- [ ] Default state is "No" for all pests
- [ ] Critical pest warning displays for foul brood
- [ ] Finish button saves and returns to main menu

**Data Management:**
- [ ] InspectionRecord structure correctly populated
- [ ] Timestamps captured from RTC for all screens
- [ ] Data persists when navigating backward
- [ ] Long press saves incomplete inspections
- [ ] Completion percentage calculates correctly
- [ ] No data loss during app lifecycle transitions
- [ ] RFID-compatible format can be generated

**Performance:**
- [ ] No memory leaks detected in 24-hour soak test
- [ ] Response time < 100ms for all inputs
- [ ] Screen transitions < 200ms
- [ ] Battery lasts minimum 8 hours continuous use
- [ ] No lag or stuttering during normal operation

**Usability:**
- [ ] Functions correctly with gloves in outdoor conditions
- [ ] Screen remains visible in direct sunlight
- [ ] Average completion time < 2 minutes
- [ ] User satisfaction rating > 4.5/5
- [ ] < 10% user error rate

**Code Quality:**
- [ ] All error conditions handled gracefully
- [ ] Code follows project style guidelines
- [ ] All functions documented with comments
- [ ] No compiler warnings
- [ ] No TODO or FIXME comments in production code

**Documentation:**
- [ ] User guide section complete
- [ ] All data structures documented
- [ ] API documentation complete
- [ ] Troubleshooting guide included

---

## 16. Risk Assessment

| Risk | Probability | Impact | Mitigation |
|------|-------------|--------|------------|
| Encoder noise causing false inputs | Medium | High | Implement robust debouncing and state filtering; extensive field testing |
| Display not visible in bright sun | Medium | High | Use maximum brightness, high contrast colors; test in various outdoor conditions |
| Complex navigation confusing users | Medium | High | Consistent interaction patterns across all screens; progress indicators; UAT with multiple users |
| Memory fragmentation across 7 screens | Medium | Medium | Avoid dynamic allocation, use static buffers; stress test with multiple inspection cycles |
| Long inspection time causing user fatigue | Low | Medium | Optimize screen flow; allow skipping optional fields; provide shortcuts |
| Data loss during screen transitions | Low | High | Implement incremental saving; atomic operations; comprehensive state testing |
| Sub-menu navigation too complex | Medium | Medium | Clear visual hierarchy; breadcrumb indicators; consistent back navigation |
| Battery drain during long inspections | Medium | Medium | Optimize display updates; implement screen dimming; test in field conditions |
| RTC battery drain over time | Low | Medium | Implement sleep mode; monitor power in idle state |
| MOONCAKE lifecycle conflicts | Medium | Medium | Thorough testing of all lifecycle events; proper state preservation |
| User confusion about screen purpose | Medium | High | Clear headers and icons; contextual help on first use; UAT feedback |
| Save operation failure mid-inspection | Low | High | Implement retry logic, error messages; auto-save progress periodically |
| Incorrect data due to rushed input | Medium | Medium | Require confirmation for critical fields (foul brood); allow review before final save |
| Device drop/damage in field | High | Medium | Recommend protective case; design for robustness; data sync to prevent loss |

---

## 17. Appendices

### Appendix A: Hardware Pin Assignments

| Function | GPIO Pin | Notes |
|----------|----------|-------|
| Encoder B | 40 | Rotary encoder quadrature signal B |
| Encoder A | 41 | Rotary encoder quadrature signal A |
| LCD RS | 4 | Display register select |
| LCD MOSI | 5 | Display data |
| LCD SCK | 6 | Display clock |
| LCD CS | 7 | Display chip select |
| LCD RESET | 8 | Display reset |
| LCD BL | 9 | Display backlight |
| I2C SDA | 11 | Touch and RTC data |
| I2C SCL | 12 | Touch and RTC clock |
| Buzzer | 3 | Audio feedback |
| HOLD | 46 | Power hold pin |

### Appendix B: Data Structure Definitions

```cpp
// Defined in inspection_data.h

// ===== SCREEN 1: Queen Right =====
enum QueenRightStatus {
    QUEEN_RIGHT_UNKNOWN = 0,
    QUEEN_RIGHT_YES = 1,
    QUEEN_RIGHT_NO = 2
};

struct QueenRightData {
    QueenRightStatus status;
    uint32_t timestamp;
    bool isSet;
    
    QueenRightData() : status(QUEEN_RIGHT_UNKNOWN), 
                       timestamp(0), 
                       isSet(false) {}
};

// ===== SCREEN 2: Queen Cells =====
enum CellCount {
    CELL_COUNT_NO = 0,      // No cells observed
    CELL_COUNT_YES = 1,     // Present but not counted
    CELL_COUNT_1_5 = 2,     // 1-5 cells
    CELL_COUNT_5_10 = 3,    // 5-10 cells
    CELL_COUNT_10_PLUS = 4  // 10+ cells
};

struct QueenCellData {
    CellCount supersedureCells;
    CellCount swarmCells;
    uint32_t timestamp;
    bool isSet;
    
    QueenCellData() : supersedureCells(CELL_COUNT_NO),
                      swarmCells(CELL_COUNT_NO),
                      timestamp(0),
                      isSet(false) {}
};

// ===== SCREEN 3: Supers =====
enum FillPercentage {
    FILL_UNSET = 255,   // Not recorded
    FILL_0 = 0,
    FILL_25 = 25,
    FILL_50 = 50,
    FILL_75 = 75,
    FILL_100 = 100
};

struct SupersData {
    uint8_t superCount;              // 0-5
    FillPercentage fillLevels[5];    // Fill % for each super
    uint32_t timestamp;
    bool isSet;
    
    SupersData() : superCount(0),
                   timestamp(0),
                   isSet(false) {
        for (int i = 0; i < 5; i++) {
            fillLevels[i] = FILL_UNSET;
        }
    }
};

// ===== SCREEN 4: Temperament =====
enum BeeTemperament {
    TEMPERAMENT_UNKNOWN = 0,
    TEMPERAMENT_CALM = 1,
    TEMPERAMENT_ANGRY = 2,
    TEMPERAMENT_STICKY = 3,
    TEMPERAMENT_FESTOONING = 4,
    TEMPERAMENT_BEARDING = 5,
    TEMPERAMENT_WASHBOARDING = 6
};

struct TemperamentData {
    BeeTemperament temperament;
    uint32_t timestamp;
    bool isSet;
    
    TemperamentData() : temperament(TEMPERAMENT_UNKNOWN),
                        timestamp(0),
                        isSet(false) {}
};

// ===== SCREEN 5: Brood Size =====
struct BroodSizeData {
    uint8_t frameCount;     // 0-40 frames
    uint32_t timestamp;
    bool isSet;
    
    BroodSizeData() : frameCount(0),
                      timestamp(0),
                      isSet(false) {}
};

// ===== SCREEN 6: Treatment =====
enum TreatmentType {
    TREATMENT_NONE = 0,
    TREATMENT_OXALIC_ACID = 1,
    TREATMENT_HOPGUARD = 2,
    TREATMENT_FORMIC_ACID = 3,
    TREATMENT_DRONE_FRAME_IN = 4,
    TREATMENT_DRONE_FRAME_OUT = 5
};

struct TreatmentData {
    TreatmentType treatment;
    uint32_t timestamp;
    uint32_t droneFrameRemovalDate;  // If DRONE_FRAME_IN, date to remove (timestamp + 21 days)
    bool isSet;
    
    TreatmentData() : treatment(TREATMENT_NONE),
                      timestamp(0),
                      droneFrameRemovalDate(0),
                      isSet(false) {}
};

// ===== SCREEN 7: Pests =====
struct PestsData {
    bool waxMoth;
    bool hiveBeetle;
    bool americanFoulBrood;
    bool europeanFoulBrood;
    uint32_t timestamp;
    bool isSet;
    
    PestsData() : waxMoth(false),
                  hiveBeetle(false),
                  americanFoulBrood(false),
                  europeanFoulBrood(false),
                  timestamp(0),
                  isSet(false) {}
    
    // Helper method to check if any pest is present
    bool anyPestPresent() const {
        return waxMoth || hiveBeetle || americanFoulBrood || europeanFoulBrood;
    }
    
    // Helper method to check if critical pest is present
    bool criticalPestPresent() const {
        return americanFoulBrood || europeanFoulBrood;
    }
};

// ===== COMPLETE INSPECTION RECORD =====
struct InspectionRecord {
    uint32_t recordId;           // Unique ID for this inspection
    uint32_t hiveId;             // Associated hive ID
    uint32_t timestampStart;     // When inspection began
    uint32_t timestampEnd;       // When inspection completed
    
    // All inspection data
    QueenRightData queenRight;
    QueenCellData queenCells;
    SupersData supers;
    TemperamentData temperament;
    BroodSizeData broodSize;
    TreatmentData treatment;
    PestsData pests;
    
    bool isComplete;             // True if all screens completed
    uint8_t lastCompletedScreen; // Last screen user completed (1-7)
    
    InspectionRecord() : recordId(0),
                         hiveId(0),
                         timestampStart(0),
                         timestampEnd(0),
                         isComplete(false),
                         lastCompletedScreen(0) {}
    
    // Calculate completion percentage
    float getCompletionPercentage() const {
        int completed = 0;
        if (queenRight.isSet) completed++;
        if (queenCells.isSet) completed++;
        if (supers.isSet) completed++;
        if (temperament.isSet) completed++;
        if (broodSize.isSet) completed++;
        if (treatment.isSet) completed++;
        if (pests.isSet) completed++;
        return (completed / 7.0f) * 100.0f;
    }
};

// ===== SERIALIZATION HELPERS =====
// For RFID/Database storage
struct InspectionRecordCompact {
    // Bit-packed version for efficient storage
    uint32_t recordId;
    uint32_t hiveId;
    uint32_t timestamp;
    
    uint8_t queenRight : 2;           // 2 bits (0=unknown, 1=yes, 2=no)
    uint8_t supersedureCells : 3;     // 3 bits (5 options)
    uint8_t swarmCells : 3;           // 3 bits (5 options)
    uint8_t superCount : 3;           // 3 bits (0-5)
    uint8_t temperament : 3;          // 3 bits (7 options)
    uint8_t broodFrames : 6;          // 6 bits (0-40)
    uint8_t treatment : 3;            // 3 bits (6 options)
    uint8_t pests : 4;                // 4 bits (4 pest flags)
    // Super fills stored separately if needed
    
    // Total: ~10 bytes + super fills array
};
```

### Appendix C: Color Reference

```cpp
// Color definitions (RGB565 format)
#define COLOR_BACKGROUND     0x1082  // Dark blue #1a1a2e
#define COLOR_TEXT_PRIMARY   0xFFFF  // White
#define COLOR_TEXT_SECONDARY 0x8410  // Gray
#define COLOR_ACCENT_ORANGE  0xFB60  // M5Stack orange #ff6b35
#define COLOR_ICON_GOLD      0xFFE0  // Gold #ffd700
#define COLOR_SUCCESS        0x07E0  // Green
#define COLOR_ERROR          0xF800  // Red
```

### Appendix D: Interaction Timing Constants

```cpp
#define DEBOUNCE_TIME_MS        50    // Button debounce
#define CLICK_MAX_MS            300   // Max duration for single click
#define DOUBLE_CLICK_WINDOW_MS  500   // Window for detecting double click
#define LONG_PRESS_MS           800   // Duration for long press
#define ROTATION_COOLDOWN_MS    100   // Min time between rotations
#define TRANSITION_ANIM_MS      200   // Screen transition duration
#define FEEDBACK_FLASH_MS       100   // Visual feedback duration
```

### Appendix E: References

1. M5Stack Dial Product Page: https://docs.m5stack.com/en/core/Dial
2. MOONCAKE Framework Documentation: (internal project docs)
3. ESP32-S3 Technical Reference: https://www.espressif.com/en/products/socs/esp32-s3
4. Beekeeping Inspection Best Practices: (user research documentation)
5. GC9A01 Display Datasheet: (hardware specs)
6. BM8563 RTC Datasheet: (hardware specs)

---

## Document Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 0.1 | 2026-02-08 | Claude | Initial draft |
| 1.0 | 2026-02-08 | Claude | Complete PRD for review |

---

**End of Document**
