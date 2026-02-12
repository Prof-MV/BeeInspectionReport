# Product Requirements Document
## Yard Management App - Complete Yard & Hive Management Flow

---

**Project:** M5Dial Beehive Management System  
**Feature:** Yard and Hive Management (App Module)  
**Version:** 1.1  
**Date:** February 12, 2026  
**Status:** Draft  

---

## 1. Executive Summary

This document defines the requirements for the Yard Management app on the M5Stack Dial device. The Yard Management app enables beekeepers to organize and track hives within yards using a unique 6-digit numbering system. Yards are identified by user-assigned 6-digit numbers with nicknames, while hives are tracked with system-wide unique 6-digit numbers. The app supports creating and closing yards, adding hives through splits or swarms, and monitoring overall yard health statistics derived from inspection data. Status tracking (Active/Closed for yards, Active/Sold/Dead/Obsconded for hives) is maintained separately from the identification numbers. The app integrates RFID technology for both yard identification (gate tags) and individual hive tracking, ensuring accurate location-based hive management.

---

## 2. Objectives

### 2.1 Primary Goals
- Enable beekeepers to create, manage, and close yards using the M5Dial's rotary interface
- Provide yard-level summary statistics derived from inspection data
- Support hive addition through swarms or splits from existing hives
- Track hive lifecycle events including creation, origin, and closure (sold/dead/obsconded)
- Integrate RFID technology for both yard gates and individual hives
- Ensure single-handed operation suitable for field use with gloves

### 2.2 Success Metrics
- Users can create a new yard in < 30 seconds
- Users can add a new hive in < 45 seconds
- Zero failed saves due to interaction errors
- 100% consistency with M5Dial interaction patterns
- Successful RFID integration with 95%+ scan success rate
- User satisfaction rating > 4.5/5 for workflow efficiency

### 2.3 Screen Flow Overview
1. **Yard List** - Select yard or access Management
2. **Management Menu** - Create or Close Yard options
3. **Create Yard** - Enter name and scan gate RFID tag
4. **Close Yard** - Select yard to close
5. **Yard Summary** - View statistics for selected yard
6. **Hive Management Input** - Add or Close Hive options
7. **Add Hive** - Enter number, select origin, scan RFID tag
8. **Close Hive** - Select hive, select reason, scan RFID tag

---

## 3. User Stories

### US-001: View Yard List
**As a** beekeeper with multiple yards  
**I want to** see a list of all my active yards  
**So that** I can select which yard to work with

**Acceptance Criteria:**
- Screen displays list of all active (not closed) yards
- Each yard shows its name
- "Management" option appears at bottom of list
- User can rotate to highlight different yards
- Single click selects a yard and navigates to Yard Summary
- Single click on "Management" navigates to Management Menu

### US-002: Create New Yard
**As a** beekeeper establishing a new apiary location  
**I want to** create a new yard with a 6-digit number, nickname, and RFID tag  
**So that** I can organize hives by location

**Acceptance Criteria:**
- User can enter a unique 6-digit number for the yard
- User can enter a nickname/name for the yard
- User can scan an RFID tag for the yard gate
- RFID tag ID is stored with the yard record
- Yard status is set to "A" (Active)
- Timestamp of yard creation is recorded
- New yard appears in the Yard List
- User returns to Yard List after creation

### US-003: Close Existing Yard
**As a** beekeeper who no longer uses a location  
**I want to** close a yard  
**So that** it no longer appears in my active yard list

**Acceptance Criteria:**
- User can select from list of active yards (status "A")
- Yard status is changed to "C" (Closed)
- Timestamp of closure is recorded
- Yard is removed from Yard List (filtered out)
- Yard data is retained in storage for historical records
- User returns to Yard List after closure

### US-004: View Yard Summary Statistics
**As a** beekeeper monitoring yard health  
**I want to** see overall statistics for a yard  
**So that** I can quickly assess the yard's status

**Acceptance Criteria:**
- Display total number of hives in yard (excluding closed hives)
- Display count of queenright hives (from inspection data)
- Display total super count across all hives
- Display average percentage fill of supers
- Statistics are derived from latest inspection records
- "Next" button advances to Hive Management Input
- "Back" button returns to Yard List

### US-005: Add New Hive to Yard
**As a** beekeeper adding hives through swarms or splits  
**I want to** record a new hive with its 6-digit number, origin, and RFID tag  
**So that** I can track hive lineage and location

**Acceptance Criteria:**
- User can enter a unique 6-digit number for the hive
- User can select origin: "Swarm" or existing hive number from yard
- User can scan RFID tag for the hive
- RFID tag ID must be unique across all hives
- 6-digit hive number must be unique system-wide
- Hive status is set to "A" (Active)
- Origin hive remains in the yard list (for splits)
- Timestamp of hive creation is recorded
- User returns to Yard Summary after creation

### US-006: Close Hive in Yard
**As a** beekeeper recording hive losses or sales  
**I want to** close a hive with a reason and scan its RFID tag  
**So that** I can track hive lifecycle and remove it from active management

**Acceptance Criteria:**
- User selects from list of active hives in yard (status "A")
- User selects closure reason: "Sold" (S), "Dead" (D), or "Obsconded" (O)
- User scans the hive's RFID tag for confirmation
- RFID tag ID is deleted from hive record
- Hive status is changed to "S", "D", or "O" (not prefixed to number)
- Hive 6-digit number remains unchanged
- Timestamp of closure is recorded
- Closed hives are filtered out of active hive lists
- Closed hives don't appear in yard statistics or origin lists
- Hive data is retained in storage for historical records
- User returns to Yard Summary after closure

### US-007: Navigate Application Flow
**As a** beekeeper using the Yard Management app  
**I want to** move through screens using consistent controls  
**So that** I can efficiently manage my yards and hives

**Acceptance Criteria:**
- Single click advances to next logical screen
- Double-click returns to previous screen where applicable
- "Back" buttons explicitly return to previous screen
- Long press (>800ms) saves data and returns to main menu
- Visual feedback indicates current position in flow
- All navigation is consistent with M5Dial interaction patterns

---

## 4. Functional Requirements

### 4.0 Global Interaction Patterns

#### FR-000: Consistent Navigation
**Priority:** MUST HAVE  
**Description:** All screens follow consistent interaction patterns
- **Rotate dial**: Navigate lists or adjust values
- **Single click**: Confirm selection / Advance to next screen
- **Double click**: Return to previous screen (where applicable)
- **Long press (>800ms)**: Save all data and return to main menu
- **Back button**: Explicit button to return to previous screen (selected screens)
- Visual feedback for all interactions
- Buzzer feedback for confirmations

---

### 4.1 Screen 1: Yard List

#### FR-101: Yard List Display
**Priority:** MUST HAVE  
**Description:** Display all active yards in a scrollable list
- Header: "Yard Management" or "Select Yard"
- List format: Vertical scrollable list
- Each yard displays: 6-digit number and nickname (e.g., "100001 - West Apiary")
- Last item in list: "Management" (always visible at bottom)
- Active yards only (status "A" only, status "C" excluded)
- Sorted by yard number (ascending)

#### FR-102: Yard Selection
**Priority:** MUST HAVE  
**Description:** User can select a yard from the list
- Rotate dial: Navigate up/down through list
- Visual indicator: Highlighted item with distinct background
- Single click on yard: Navigate to Yard Summary (Screen 5) for selected yard
- Single click on "Management": Navigate to Management Menu (Screen 2)

#### FR-103: Empty State Handling
**Priority:** MUST HAVE  
**Description:** Handle case when no yards exist
- Display message: "No Yards Yet"
- Display "Management" option
- User can only access Management to create first yard

---

### 4.2 Screen 2: Management Menu

#### FR-201: Management Options Display
**Priority:** MUST HAVE  
**Description:** Display yard management options
- Header: "Yard Management"
- Two options displayed:
  - "Create Yard"
  - "Close Yard"
- Layout: Vertical list format

#### FR-202: Management Option Selection
**Priority:** MUST HAVE  
**Description:** User selects management action
- Rotate dial: Highlight "Create Yard" or "Close Yard"
- Visual indicator: Arrow or highlight box
- Single click on "Create Yard": Navigate to Create Yard screen (Screen 3)
- Single click on "Close Yard": Navigate to Close Yard screen (Screen 4)
- Double click: Return to Yard List (Screen 1)

---

### 4.3 Screen 3: Create Yard

#### FR-301: Yard Number Input
**Priority:** MUST HAVE  
**Description:** User enters 6-digit number for new yard
- Header: "Create Yard - Number"
- Display: Current yard number being entered
- Input method: Rotate to increment/decrement each digit, click to advance to next digit
- Format: 6 digits (100000-999999)
- Visual: Large display of current number with highlighted digit position
- "Next" button advances to nickname input when complete
- Double click: Return to Management Menu (Screen 2)

#### FR-302: Yard Number Validation
**Priority:** MUST HAVE  
**Description:** Ensure yard number is unique system-wide
- Check if 6-digit number already exists (active or closed yards)
- If duplicate:
  - Display error: "Yard [NNNNNN] exists"
  - Allow user to enter different number
  - Cannot proceed until unique number provided
- Validate number is within range (100000-999999)

#### FR-303: Yard Nickname Input
**Priority:** MUST HAVE  
**Description:** User enters nickname for new yard
- Header: "Create Yard - Nickname"
- Input field for yard nickname
- Character set: Alphanumeric + space, dash, underscore
- Max length: 20 characters
- Input method: Rotate to select character, click to confirm, advance to next position
- Visual: Current character position highlighted
- "Next" button advances to RFID scan when nickname is complete
- "Back" button: Return to yard number input
- Double click: Return to yard number input

#### FR-304: Yard Gate RFID Scan
**Priority:** MUST HAVE  
**Description:** User scans RFID tag for yard gate
- Header: "Create Yard - Scan Tag"
- Display: "Yard [NNNNNN] - [Nickname]"
- Display: "Scan Gate RFID Tag"
- Visual indicator: RFID icon or animation
- Prompt: "Present tag to reader"
- On successful scan:
  - Display tag ID briefly (e.g., "Tag: 9A3F2B10")
  - Visual/haptic confirmation
  - Create yard record with number, nickname, RFID tag, status "A", and timestamp
  - Return to Yard List (Screen 1)
- On scan failure:
  - Display error message
  - Allow retry
  - Option to cancel and return to nickname input
- "Back" button: Return to nickname input (before scan)
- Double click: Return to nickname input (before scan)

#### FR-305: Duplicate RFID Validation
**Priority:** MUST HAVE  
**Description:** Prevent duplicate RFID tags
- Validate RFID tag is unique across all yard gates
- If duplicate detected:
  - Display error message: "Tag already in use"
  - Allow user to rescan different tag
  - Cannot proceed until unique tag provided

---

### 4.4 Screen 4: Close Yard

#### FR-401: Yard Selection for Closure
**Priority:** MUST HAVE  
**Description:** User selects yard to close
- Header: "Close Yard"
- Display list of all active yards (status "A" only)
- Display format: "NNNNNN - Nickname" (e.g., "100001 - West Apiary")
- Rotate dial: Navigate through yard list
- Visual indicator: Highlighted yard
- Single click: Confirm yard selection and show confirmation prompt
- Double click: Return to Management Menu (Screen 2)

#### FR-402: Closure Confirmation
**Priority:** MUST HAVE  
**Description:** Confirm yard closure action
- Display: "Close Yard [NNNNNN]?"
- Display: "[Nickname]"
- Options: "Yes" / "Cancel"
- Rotate dial: Toggle between options
- Single click on "Yes":
  - Change yard status to "C" (Closed)
  - Record closure timestamp
  - Remove yard from active list
  - Return to Yard List (Screen 1)
- Single click on "Cancel": Return to yard selection list
- Double click: Return to yard selection list

#### FR-403: Closure Record Keeping
**Priority:** MUST HAVE  
**Description:** Maintain closed yard data for records
- Yard record is not deleted
- Yard status changed from "A" to "C"
- Closure timestamp is recorded
- Closed yards (status "C") do not appear in Yard List
- Closed yards retained for historical data access
- Associated hives remain in storage with their yard reference

---

### 4.5 Screen 5: Yard Summary

#### FR-501: Summary Statistics Display
**Priority:** MUST HAVE  
**Description:** Display key statistics for selected yard
- Header: "Yard [NNNNNN]" or "[Nickname]"
- Subheader: "Yard [NNNNNN] - [Nickname]" (if both needed)
- Statistics displayed (large, readable font):
  1. **Total Hives**: Count of active (status "A") hives in yard
  2. **Queenright Hives**: Count of hives marked queenright in latest inspection
  3. **Total Supers**: Sum of all supers across all active hives
  4. **Average Fill %**: Average fill percentage across all supers
- Statistics derived from latest inspection records for each active hive
- Format: "Total Hives: 12", "Queenright: 10", etc.
- Layout: Stacked vertically with clear labels

#### FR-502: Data Source Integration
**Priority:** MUST HAVE  
**Description:** Pull statistics from inspection data
- Query latest inspection record for each active hive (status "A") in yard
- For queenright count: Count hives where queenRight.status == YES
- For supers: Sum superCount across all hives
- For fill percentage:
  - Calculate weighted average based on super count
  - Include only supers with fill level set
  - Display as percentage (e.g., "Avg Fill: 65%")
- If no inspection data exists: Display "No Data" for that statistic
- Cache statistics to avoid repeated queries
- Exclude hives with status S, D, or O from all calculations

#### FR-503: Navigation from Summary
**Priority:** MUST HAVE  
**Description:** Navigate to hive management or back
- "Next" button at bottom: Advances to Hive Management Input (Screen 6)
- "Back" button at top: Returns to Yard List (Screen 1)
- Single click on "Next": Advance to Screen 6
- Single click on "Back": Return to Screen 1
- Double click: Return to Yard List (Screen 1)
- Long press: Save and return to main menu

---

### 4.6 Screen 6: Hive Management Input

#### FR-601: Hive Management Options Display
**Priority:** MUST HAVE  
**Description:** Display hive management actions
- Header: "Yard [NNNNNN] - Hive Mgmt"
- Two options displayed:
  - "Add Hive"
  - "Close Hive"
- Layout: Vertical list format

#### FR-602: Hive Management Selection
**Priority:** MUST HAVE  
**Description:** User selects hive management action
- Rotate dial: Highlight "Add Hive" or "Close Hive"
- Visual indicator: Arrow or highlight box
- Single click on "Add Hive": Navigate to Add Hive screen (Screen 7)
- Single click on "Close Hive": Navigate to Close Hive screen (Screen 8)
- "Back" button: Returns to Yard Summary (Screen 5)
- Double click: Return to Yard Summary (Screen 5)

---

### 4.7 Screen 7: Add Hive

#### FR-701: Hive Number Input
**Priority:** MUST HAVE  
**Description:** User enters 6-digit number for new hive
- Header: "Add Hive - Number"
- Display: Current hive number being entered
- Input method: Rotate to increment/decrement each digit, click to advance to next digit
- Format: 6 digits (100000-999999)
- Visual: Large display of current number with highlighted digit position
- "Next" button advances to origin selection when complete
- "Back" button: Return to Hive Management Input (Screen 6)
- Double click: Return to Hive Management Input (Screen 6)

#### FR-702: Hive Number Validation
**Priority:** MUST HAVE  
**Description:** Ensure hive number is unique system-wide
- Check if 6-digit number already exists in ANY yard (active or closed hives)
- If duplicate:
  - Display error: "Hive [NNNNNN] exists"
  - Allow user to enter different number
  - Cannot proceed until unique number provided
- Validate number is within range (100000-999999)
- Uniqueness is system-wide, not per-yard

#### FR-703: Origin Selection
**Priority:** MUST HAVE  
**Description:** User selects origin of new hive
- Header: "Add Hive - Origin"
- Display: "Hive [NNNNNN] - Select Origin"
- Options listed:
  - "Swarm"
  - List of active hive 6-digit numbers in yard (e.g., "100123", "100456", etc.)
- Layout: Scrollable vertical list
- Rotate dial: Navigate through origin options
- Visual indicator: Highlighted option
- Single click: Confirm origin and advance to RFID scan
- "Back" button: Return to hive number input
- Double click: Return to hive number input

#### FR-704: Hive RFID Scan
**Priority:** MUST HAVE  
**Description:** User scans RFID tag for new hive
- Header: "Add Hive - Scan Tag"
- Display: "Hive [NNNNNN] from [Origin]"
- Display: "Scan Hive RFID Tag"
- Visual indicator: RFID icon or animation
- Prompt: "Present tag to reader"
- On successful scan:
  - Validate tag is unique across ALL hives in system
  - Display tag ID briefly (e.g., "Tag: 7B2E4C91")
  - Visual/haptic confirmation
  - Create hive record with 6-digit number, origin, RFID tag, yard ID, status "A", and timestamp
  - Return to Yard Summary (Screen 5)
- On scan failure or duplicate tag:
  - Display error message
  - Allow retry
  - Option to cancel and return to origin selection
- "Back" button: Return to origin selection (before scan)
- Double click: Return to origin selection (before scan)

#### FR-705: Hive Record Creation
**Priority:** MUST HAVE  
**Description:** Store new hive data in NVS
- Record structure includes:
  - Hive 6-digit number (100000-999999)
  - Yard ID (reference to parent yard's 6-digit number)
  - Origin type: SWARM or SPLIT
  - Origin hive 6-digit number (if split, null if swarm)
  - RFID tag ID (unique identifier)
  - Creation timestamp
  - Status: "A" (Active)
- Store in NVS with unique key
- Update yard's hive count
- Generate unique hive ID for system-wide reference

---

### 4.8 Screen 8: Close Hive

#### FR-801: Hive Selection for Closure
**Priority:** MUST HAVE  
**Description:** User selects hive to close
- Header: "Close Hive"
- Display list of all active hives (status "A") in current yard
- Format: "Hive [NNNNNN]" for each entry (6-digit number)
- Sorted numerically by hive number
- Rotate dial: Navigate through hive list
- Visual indicator: Highlighted hive
- Single click: Confirm hive selection and advance to reason selection
- "Back" button: Return to Hive Management Input (Screen 6)
- Double click: Return to Hive Management Input (Screen 6)

#### FR-802: Closure Reason Selection
**Priority:** MUST HAVE  
**Description:** User selects reason for closing hive
- Header: "Close Hive [NNNNNN]"
- Display: "Select Reason"
- Three options:
  - "Sold" (will set status to 'S')
  - "Dead" (will set status to 'D')
  - "Obsconded" (will set status to 'O')
- Layout: Vertical list
- Rotate dial: Navigate through reason options
- Visual indicator: Highlighted reason
- Single click: Confirm reason and advance to RFID scan
- "Back" button: Return to hive selection
- Double click: Return to hive selection

#### FR-803: Closure RFID Scan
**Priority:** MUST HAVE  
**Description:** User scans hive RFID tag for confirmation
- Header: "Close Hive [NNNNNN]"
- Display: "Reason: [Selected Reason]"
- Display: "Scan Hive Tag to Confirm"
- Visual indicator: RFID icon
- Prompt: "Present tag to reader"
- On successful scan:
  - Validate tag matches hive's stored RFID tag ID
  - If match:
    - Visual/haptic confirmation
    - Update hive record (see FR-804)
    - Return to Yard Summary (Screen 5)
  - If no match:
    - Display error: "Wrong Tag"
    - Allow retry or cancel
- On scan failure:
  - Display error message
  - Allow retry
  - Option to cancel and return to reason selection
- "Back" button: Return to reason selection
- Double click: Return to reason selection

#### FR-804: Hive Closure Record Update
**Priority:** MUST HAVE  
**Description:** Update hive record upon closure
- Update hive record:
  - Delete RFID tag ID field (set to null/empty)
  - Change status to selected reason:
    - Sold: status = "S"
    - Dead: status = "D"
    - Obsconded: status = "O"
  - Hive 6-digit number remains unchanged
  - Record closure timestamp
  - Record closure reason
- Hive record is not deleted (retained for historical data)
- Closed hive does not appear in:
  - Active hive lists (filtered by status != "A")
  - Yard summary statistics
  - Origin selection lists for new hives
- Update yard's active hive count

---

## 5. Data Models

### 5.1 Yard Record Structure

```cpp
enum YardStatus {
    YARD_STATUS_ACTIVE = 'A',
    YARD_STATUS_CLOSED = 'C'
};

struct YardRecord {
    uint32_t yardNumber;          // Unique 6-digit number (100000-999999)
    char nickname[21];            // Yard nickname (max 20 chars + null terminator)
    char rfidTagId[17];           // RFID tag ID for gate (16 hex chars + null)
    uint32_t creationTimestamp;   // Unix timestamp when yard was created
    uint32_t closureTimestamp;    // Unix timestamp when closed (0 if active)
    char status;                  // 'A' = Active, 'C' = Closed
    uint16_t activeHiveCount;     // Count of active hives (for quick reference)
};

// NVS storage format (54 bytes)
struct YardRecordNVS {
    uint32_t yardNumber;          // 4 bytes (100000-999999)
    char nickname[21];            // 21 bytes
    char rfidTagId[17];           // 17 bytes
    uint32_t creationTimestamp;   // 4 bytes
    uint32_t closureTimestamp;    // 4 bytes (0 if active)
    uint8_t status;               // 1 byte ('A' or 'C')
    uint16_t crc16;               // 2 bytes checksum
};
// Total: 53 bytes per yard record
```

### 5.2 Hive Record Structure

```cpp
enum HiveOriginType {
    ORIGIN_SWARM = 0,
    ORIGIN_SPLIT = 1
};

enum HiveStatus {
    HIVE_STATUS_ACTIVE = 'A',
    HIVE_STATUS_SOLD = 'S',
    HIVE_STATUS_DEAD = 'D',
    HIVE_STATUS_OBSCONDED = 'O'
};

struct HiveRecord {
    uint32_t hiveNumber;          // Unique 6-digit number (100000-999999)
    uint32_t yardNumber;          // Reference to parent yard's 6-digit number
    HiveOriginType originType;    // SWARM or SPLIT
    uint32_t originHiveNumber;    // If split: origin hive 6-digit number, else 0
    char rfidTagId[17];           // RFID tag ID (empty if closed)
    uint32_t creationTimestamp;   // Unix timestamp when hive was created
    uint32_t closureTimestamp;    // Unix timestamp when closed (0 if active)
    char status;                  // 'A' = Active, 'S' = Sold, 'D' = Dead, 'O' = Obsconded
};

// NVS storage format (51 bytes)
struct HiveRecordNVS {
    uint32_t hiveNumber;          // 4 bytes (100000-999999)
    uint32_t yardNumber;          // 4 bytes (100000-999999)
    uint8_t originType;           // 1 byte (0=swarm, 1=split)
    uint32_t originHiveNumber;    // 4 bytes (0 if swarm)
    char rfidTagId[17];           // 17 bytes (empty if closed)
    uint32_t creationTimestamp;   // 4 bytes
    uint32_t closureTimestamp;    // 4 bytes (0 if active)
    uint8_t status;               // 1 byte ('A', 'S', 'D', or 'O')
    uint16_t crc16;               // 2 bytes checksum
};
// Total: 49 bytes per hive record
```

### 5.3 Yard Summary Statistics Structure

```cpp
struct YardSummaryStats {
    uint32_t yardNumber;          // Yard being summarized
    uint16_t totalHives;          // Count of active hives (status 'A')
    uint16_t queenrightHives;     // Count of queenright hives (from inspections)
    uint16_t totalSupers;         // Total supers across all hives
    float averageFillPercent;     // Average fill % (0-100)
    uint32_t lastCalculated;      // Timestamp of last calculation
    bool hasInspectionData;       // true if any inspection data exists
};

// This structure is computed on-demand from:
// 1. HiveRecord data (for totalHives - status 'A' only)
// 2. InspectionRecord data from Beehive Inspection app (for other stats)
```

---

## 6. Integration with Beehive Inspection App

### 6.1 Data Dependencies

#### Shared Data Access
**Priority:** MUST HAVE  
**Description:** Yard Management app reads inspection data
- Yard Management reads InspectionRecord data from NVS
- Query pattern: Get latest inspection for each active hive in yard
- Statistics calculated on-demand when Yard Summary screen loads
- No writes to InspectionRecord from Yard Management app

#### Inspection Data Query
```cpp
// Query latest inspection for a hive (using 6-digit hive number)
InspectionRecord getLatestInspection(uint32_t hiveNumber) {
    // Search NVS for most recent inspection record for hiveNumber
    // Return empty record if none found
}

// Calculate yard statistics
YardSummaryStats calculateYardStats(uint32_t yardNumber) {
    YardSummaryStats stats = {0};
    stats.yardNumber = yardNumber;
    
    // Get all active hives in yard (status 'A' only)
    HiveRecord* hives = getActiveHivesInYard(yardNumber);
    stats.totalHives = count(hives);
    
    // For each hive, get latest inspection
    for (each hive in hives) {
        // Only process hives with status 'A'
        if (hive.status != 'A') continue;
        
        InspectionRecord inspection = getLatestInspection(hive.hiveNumber);
        
        if (inspection.isComplete) {
            stats.hasInspectionData = true;
            
            // Count queenright
            if (inspection.queenRight.status == QUEEN_RIGHT_YES) {
                stats.queenrightHives++;
            }
            
            // Sum supers
            stats.totalSupers += inspection.supers.superCount;
            
            // Calculate average fill (weighted by super count)
            for (int i = 0; i < inspection.supers.superCount; i++) {
                if (inspection.supers.fillLevels[i] != FILL_UNKNOWN) {
                    // Convert FillPercentage enum to actual percentage
                    // Accumulate for weighted average calculation
                }
            }
        }
    }
    
    // Calculate weighted average fill percentage
    stats.averageFillPercent = calculateWeightedAverage();
    stats.lastCalculated = getCurrentTimestamp();
    
    return stats;
}

// Helper function to get active hives in a yard
HiveRecord* getActiveHivesInYard(uint32_t yardNumber) {
    // Query all hive records where yardNumber matches AND status == 'A'
    // Return array of matching HiveRecord structures
}
```

### 6.2 RFID System Integration

#### Shared RFID Handler
**Priority:** MUST HAVE  
**Description:** Both apps use same RFID scanning functionality
- RFID module: PN532 or compatible
- Communication: I2C or SPI
- Tag format: ISO14443A (NTAG, MIFARE, etc.)
- Tag ID format: 16 hex characters (8 bytes)
- Scan timeout: 10 seconds
- Retry capability: 3 attempts

#### RFID Uniqueness Validation
```cpp
// Check if RFID tag is already in use
bool isRFIDTagUnique(const char* tagId, RFIDTagType type) {
    // Type: YARD_GATE or HIVE
    // Search all yard records if type == YARD_GATE
    // Search all hive records if type == HIVE
    // Return false if tag ID found in active (non-closed) records
    // Return true if tag is unique
}
```

---

## 7. User Interface Specifications

### 7.1 Screen Layouts

#### Layout Pattern: List Screens
Used for: Yard List, Hive List, Origin Selection
```
┌─────────────────────────┐
│      [Header Text]      │  ← Title (30px from top)
├─────────────────────────┤
│                         │
│   > 100001 - West       │  ← Highlighted item
│     100002 - East       │
│     100015 - North      │  ← Scrollable list
│     100022 - South      │
│     ...                 │
│                         │
│   [Back]                │  ← Back button (bottom)
└─────────────────────────┘
```

#### Layout Pattern: Input Screens
Used for: Hive Number, Yard Number
```
┌─────────────────────────┐
│      [Header Text]      │
├─────────────────────────┤
│                         │
│                         │
│       ┌────────┐        │
│       │ 123456 │        │  ← Large 6-digit number
│       └────────┘        │
│          ^              │  ← Digit position indicator
│                         │
│   [Back]    [Next]      │  ← Navigation buttons
└─────────────────────────┘
```

#### Layout Pattern: RFID Scan Screens
Used for: Gate RFID, Hive RFID
```
┌─────────────────────────┐
│      Scan RFID Tag      │
├─────────────────────────┤
│                         │
│         ╭───╮           │
│        │ NFC │          │  ← RFID icon/animation
│         ╰───╯           │
│                         │
│   Present tag to        │
│   reader...             │  ← Instructions
│                         │
│   [Back]                │
└─────────────────────────┘
```

#### Layout Pattern: Summary Statistics
Used for: Yard Summary
```
┌─────────────────────────┐
│  Yard 100001 Summary    │
│  (West Apiary)          │
├─────────────────────────┤
│                         │
│  Total Hives:     12    │
│  Queenright:      10    │  ← Statistics (large font)
│  Total Supers:    28    │
│  Avg Fill:        67%   │
│                         │
│                         │
│   [Back]    [Next]      │
└─────────────────────────┘
```

### 7.2 Visual Design Elements

#### Color Scheme
```cpp
// Primary colors (consistent with Beehive Inspection app)
#define COLOR_BACKGROUND     0x1082  // Dark blue #1a1a2e
#define COLOR_TEXT_PRIMARY   0xFFFF  // White
#define COLOR_TEXT_SECONDARY 0x8410  // Gray
#define COLOR_ACCENT_ORANGE  0xFB60  // M5Stack orange #ff6b35
#define COLOR_ICON_GOLD      0xFFE0  // Gold #ffd700
#define COLOR_SUCCESS        0x07E0  // Green
#define COLOR_ERROR          0xF800  // Red
#define COLOR_HIGHLIGHT_BG   0x4208  // Highlighted item background
#define COLOR_BORDER         0x632C  // Border/separator color
```

#### Typography
- **Headers**: Bold, 20pt equivalent, COLOR_TEXT_PRIMARY
- **Body text**: Regular, 16pt equivalent, COLOR_TEXT_PRIMARY
- **Large numbers**: Bold, 32pt equivalent, COLOR_ACCENT_ORANGE
- **Secondary text**: Regular, 14pt, COLOR_TEXT_SECONDARY
- **Buttons**: Bold, 14pt, COLOR_TEXT_PRIMARY

#### Icon Set
- **RFID Icon**: 60×60px, COLOR_ICON_GOLD
- **Back Arrow**: 40×40px, COLOR_TEXT_PRIMARY
- **Next Arrow**: 40×40px, COLOR_ACCENT_ORANGE
- **Checkmark**: 40×40px, COLOR_SUCCESS
- **X/Cancel**: 40×40px, COLOR_ERROR
- **Yard/Apiary Icon**: 60×60px, COLOR_ICON_GOLD
- **Hive Icon**: 40×40px, COLOR_ACCENT_ORANGE

### 7.3 Interaction Feedback

#### Visual Feedback
- **Selection**: Highlighted background color changes
- **Confirmation**: Green checkmark flash (200ms)
- **Error**: Red X flash (200ms) + shake animation
- **Scan success**: RFID icon pulses green
- **Scan failure**: RFID icon flashes red
- **Loading**: Rotating circle animation

#### Haptic Feedback
- **Single click**: Short buzz (50ms)
- **Confirmation**: Double buzz (50ms + 50ms)
- **Error**: Long buzz (200ms)
- **Scan success**: Triple buzz (50ms + 50ms + 50ms)
- **Dial rotation**: Micro buzz per detent (10ms)

#### Audio Feedback
- **Confirmation**: High beep (1kHz, 100ms)
- **Error**: Low beep (500Hz, 200ms)
- **Scan success**: Ascending beep (1kHz → 2kHz, 150ms)
- **Scan failure**: Descending beep (1kHz → 500Hz, 150ms)

---

## 8. Non-Functional Requirements

### 8.1 Performance

#### NFR-001: Response Time
**Priority:** MUST HAVE
- Screen transitions: < 200ms
- Dial rotation response: < 50ms
- Button press acknowledgment: < 100ms
- RFID scan detection: < 3 seconds
- Statistics calculation: < 500ms
- NVS read/write: < 100ms per operation

#### NFR-002: Resource Usage
**Priority:** MUST HAVE
- RAM usage: < 30KB for app state
- NVS storage per yard: 53 bytes
- NVS storage per hive: 53 bytes
- Screen buffer: 240×240×2 bytes = 115KB
- RFID buffer: < 1KB

### 8.2 Reliability

#### NFR-003: Data Integrity
**Priority:** MUST HAVE
- CRC16 checksum for all NVS records
- Atomic NVS write operations
- Data validation on read
- Corruption detection and handling
- Backup/rollback capability for critical operations

#### NFR-004: Error Handling
**Priority:** MUST HAVE
- Graceful handling of RFID scan failures
- Recovery from NVS read/write errors
- User-friendly error messages
- Retry mechanisms for transient failures
- Logging of critical errors

### 8.3 Usability

#### NFR-005: Field Usability
**Priority:** MUST HAVE
- Operable with gloves (thick beekeeping gloves)
- Readable in bright sunlight (max screen brightness)
- Single-handed operation throughout entire flow
- Clear visual hierarchy for quick scanning
- Minimal input steps for common operations

#### NFR-006: Learnability
**Priority:** SHOULD HAVE
- Consistent interaction patterns across all screens
- Clear labeling of all options
- Intuitive navigation flow
- Minimal training required
- Visual cues for available actions

### 8.4 Compatibility

#### NFR-007: Hardware Compatibility
**Priority:** MUST HAVE
- M5Stack Dial ESP32-S3 compatibility
- GC9A01 240×240 round display support
- RFID module integration (PN532 or compatible)
- RTC support for timestamps
- Battery operation (min 2 hours continuous use)

#### NFR-008: Software Compatibility
**Priority:** MUST HAVE
- Integration with MOONCAKE app framework
- Compatible with Beehive Inspection app data structures
- NVS partition sharing with other apps
- ESP-IDF framework compatibility
- No conflicts with other M5Dial apps

---

## 9. Technical Architecture

### 9.1 App Structure

```
YardManagementApp/
├── YardManagementApp.h         // Main app class
├── YardManagementApp.cpp       // App implementation
├── screens/
│   ├── ScreenYardList.h/cpp                // Screen 1
│   ├── ScreenManagementMenu.h/cpp          // Screen 2
│   ├── ScreenCreateYardNumber.h/cpp        // Screen 3a (6-digit number)
│   ├── ScreenCreateYardNickname.h/cpp      // Screen 3b (nickname)
│   ├── ScreenCreateYardRFID.h/cpp          // Screen 3c (RFID scan)
│   ├── ScreenCloseYard.h/cpp               // Screen 4
│   ├── ScreenYardSummary.h/cpp             // Screen 5
│   ├── ScreenHiveMgmtInput.h/cpp           // Screen 6
│   ├── ScreenAddHiveNumber.h/cpp           // Screen 7a (6-digit number)
│   ├── ScreenAddHiveOrigin.h/cpp           // Screen 7b (origin selection)
│   ├── ScreenAddHiveRFID.h/cpp             // Screen 7c (RFID scan)
│   ├── ScreenCloseHiveSelect.h/cpp         // Screen 8a (select hive)
│   ├── ScreenCloseHiveReason.h/cpp         // Screen 8b (reason)
│   └── ScreenCloseHiveRFID.h/cpp           // Screen 8c (RFID scan)
├── data/
│   ├── YardDataManager.h/cpp   // Yard CRUD operations
│   ├── HiveDataManager.h/cpp   // Hive CRUD operations
│   └── StatsCalculator.h/cpp   // Yard statistics calculation
├── rfid/
│   ├── RFIDScanner.h/cpp       // RFID scanning functionality
│   └── RFIDTagValidator.h/cpp  // Tag validation and uniqueness
└── ui/
    ├── ListRenderer.h/cpp      // Reusable list UI component
    ├── SixDigitInput.h/cpp     // 6-digit number input component
    ├── TextInputRenderer.h/cpp // Text input (nickname) rendering
    └── StatDisplay.h/cpp       // Statistics display component
```

### 9.2 State Management

```cpp
enum AppState {
    STATE_YARD_LIST,
    STATE_MANAGEMENT_MENU,
    STATE_CREATE_YARD_NUMBER,
    STATE_CREATE_YARD_NICKNAME,
    STATE_CREATE_YARD_RFID,
    STATE_CLOSE_YARD_SELECT,
    STATE_CLOSE_YARD_CONFIRM,
    STATE_YARD_SUMMARY,
    STATE_HIVE_MGMT_INPUT,
    STATE_ADD_HIVE_NUMBER,
    STATE_ADD_HIVE_ORIGIN,
    STATE_ADD_HIVE_RFID,
    STATE_CLOSE_HIVE_SELECT,
    STATE_CLOSE_HIVE_REASON,
    STATE_CLOSE_HIVE_RFID
};

class YardManagementApp {
private:
    AppState currentState;
    AppState previousState;  // For back navigation
    
    // Context data
    uint32_t selectedYardNumber;       // 6-digit yard number (100000-999999)
    uint32_t newYardNumber;            // 6-digit number being created
    char yardNicknameBuffer[21];       // Yard nickname being created
    uint32_t newHiveNumber;            // 6-digit hive number (100000-999999)
    uint32_t selectedOriginHiveNumber; // 6-digit origin hive number
    HiveOriginType originType;
    uint32_t selectedHiveNumberForClosure; // 6-digit hive number to close
    char selectedClosureReason;        // 'S', 'D', or 'O'
    
    // Managers
    YardDataManager* yardData;
    HiveDataManager* hiveData;
    StatsCalculator* statsCalc;
    RFIDScanner* rfidScanner;
    
public:
    void handleRotation(int8_t direction);
    void handleClick();
    void handleDoubleClick();
    void handleLongPress();
    void render();
    void transitionToState(AppState newState);
};
```

### 9.3 NVS Key Schema

```cpp
// NVS Key Naming Convention
// Format: "app:entity:number"

// Yard records
// Key: "ym:yard:100000" through "ym:yard:999999"
// Value: YardRecordNVS struct (53 bytes)
// Uses the 6-digit yard number as the key suffix

// Hive records
// Key: "ym:hive:100000" through "ym:hive:999999"
// Value: HiveRecordNVS struct (49 bytes)
// Uses the 6-digit hive number as the key suffix

// App metadata
// Key: "ym:meta:yard_count"      (uint16_t - total yards including closed)
// Key: "ym:meta:hive_count"      (uint16_t - total hives including closed)

// Index structures for quick lookups
// Key: "ym:idx:yard_list"        (array of active yard 6-digit numbers, status 'A')
// Key: "ym:idx:yard:[yardNumber]:hives"  (array of active hive 6-digit numbers in yard, status 'A')

// Note: Yard and hive numbers are unique system-wide identifiers
// Range: 100000-999999 (900,000 possible values)
```

### 9.4 Memory Budget

```
Heap Memory Allocation:
- App state object:           2 KB
- Screen buffers:            10 KB
- RFID scanner:               1 KB
- Data managers:              3 KB
- UI components:              4 KB
- Temporary string buffers:   2 KB
- Lists/arrays (dynamic):     5 KB
Total estimated:            ~27 KB

Flash Memory (Program):
- App code:                 ~40 KB
- Screen implementations:   ~30 KB (increased for 6-digit input handling)
- Data managers:            ~10 KB
- RFID scanner:             ~8 KB
- UI components:            ~12 KB
Total estimated:           ~100 KB

NVS Storage (Data):
- Per yard record:           53 bytes
- Per hive record:           49 bytes
- Metadata:                  ~100 bytes
- Indices:                   ~500 bytes (est)
Example: 20 yards × 53 bytes = 1,060 bytes
Example: 200 hives × 49 bytes = 9,800 bytes
Estimated total:            ~11.5 KB for realistic dataset

Note: 6-digit numbering system allows for 900,000 unique 
identifiers (100000-999999) which far exceeds practical needs
```

---

## 10. Testing Requirements

### 10.1 Unit Tests

#### UT-001: Data Model Tests
- YardRecord serialization/deserialization
- HiveRecord serialization/deserialization
- CRC checksum calculation and validation
- Hive number string formatting (with prefixes)

#### UT-002: Validation Tests
- Yard 6-digit number uniqueness validation (system-wide)
- Yard nickname format validation
- Hive 6-digit number uniqueness validation (system-wide, not per-yard)
- RFID tag uniqueness (yard gates and hives)
- Input bounds checking (yard numbers 100000-999999, hive numbers 100000-999999)
- Status validation ('A', 'C' for yards; 'A', 'S', 'D', 'O' for hives)

#### UT-003: Statistics Calculation Tests
- Queenright count calculation
- Super count aggregation
- Average fill percentage calculation (weighted)
- Handling missing inspection data
- Handling yards with no hives

### 10.2 Integration Tests

#### IT-001: NVS Storage Tests
- Create, read, update, delete yard records
- Create, read, update, delete hive records
- Data persistence across power cycles
- Handling NVS full condition
- Concurrent access with Beehive Inspection app

#### IT-002: RFID Integration Tests
- Successful tag scan and read
- Tag scan timeout handling
- Duplicate tag detection
- Tag validation against stored records
- RFID hardware communication errors

#### IT-003: Cross-App Integration Tests
- Reading inspection data from Beehive Inspection app
- Statistics calculation from inspection records
- Handling missing or incomplete inspection data
- No data corruption when both apps access NVS

### 10.3 User Acceptance Tests

#### UAT-001: Create Yard Workflow
- User can create yard with 6-digit number
- User can enter yard nickname
- User can scan gate RFID tag
- Yard appears in Yard List with format "NNNNNN - Nickname"
- Yard status is set to 'A' (Active)
- Duplicate 6-digit number prevention works
- Duplicate RFID prevention works
- Number range validation works (100000-999999)

#### UAT-002: Close Yard Workflow
- User can select yard to close
- Confirmation dialog prevents accidents
- Yard status changed to 'C' (Closed)
- Closed yard removed from Yard List (filtered out)
- Yard data retained in storage

#### UAT-003: View Yard Summary
- Summary displays yard number and nickname
- Summary displays correct statistics
- Statistics include only active hives (status 'A')
- Statistics match inspection data
- Handles yards with no hives
- Handles hives with no inspection data

#### UAT-004: Add Hive Workflow
- User can enter 6-digit hive number
- User can select swarm or split origin
- User can scan hive RFID tag
- Hive appears in yard statistics
- Hive status is set to 'A' (Active)
- Duplicate 6-digit hive number prevention works (system-wide)
- Duplicate RFID prevention works
- Number range validation works (100000-999999)

#### UAT-005: Close Hive Workflow
- User can select hive to close
- User can select closure reason
- User can scan RFID tag for confirmation
- Wrong RFID tag rejected
- Hive status updated to 'S', 'D', or 'O' (not prefixed to number)
- Hive 6-digit number remains unchanged
- RFID tag removed from hive record
- Closed hive removed from active lists (filtered by status)
- Yard statistics updated correctly (excludes non-'A' hives)

#### UAT-006: Navigation and Usability
- All screens accessible via defined navigation
- Back buttons work correctly
- Double-click back navigation works
- Long-press save and exit works
- Interaction patterns consistent throughout

### 10.4 Field Tests

#### FT-001: Glove Operation
- All interactions work with thick beekeeping gloves
- Dial rotation detectable with gloves
- Buttons pressable with gloves
- Screen readable at arm's length

#### FT-002: Environmental Conditions
- Screen readable in direct sunlight
- Device operable in temperature range (5°C - 40°C)
- RFID scanning works in field conditions
- Battery life meets minimum 2-hour requirement

#### FT-003: Real-World Scenarios
- Create yard at new apiary location
- Add multiple hives in quick succession
- Close hive and verify removal from statistics
- View summary statistics match actual hive state
- Recover from interrupted workflows (long press save)

---

## 11. Acceptance Criteria

### Must Have (Release Blockers)

1. **Yard Management**
   - ✓ Create new yard with 6-digit number, nickname, and RFID gate tag
   - ✓ Close yard with confirmation
   - ✓ Yard status changes from 'A' to 'C' on closure
   - ✓ Yard list displays all active yards (status 'A' only)
   - ✓ Closed yards (status 'C') excluded from Yard List

2. **Hive Management**
   - ✓ Add hive with 6-digit number, origin (swarm/split), and RFID tag
   - ✓ Close hive with reason (Sold/Dead/Obsconded) and RFID confirmation
   - ✓ Hive status changes to 'S', 'D', or 'O' on closure (not prefixed to number)
   - ✓ Hive 6-digit number remains unchanged on closure
   - ✓ RFID tag removed from closed hive

3. **Statistics Display**
   - ✓ Yard summary shows yard number, nickname, and statistics
   - ✓ Statistics: total hives (status 'A'), queenright count, total supers, average fill
   - ✓ Statistics derived from latest inspection records
   - ✓ Only active hives (status 'A') included in calculations
   - ✓ Handles missing or incomplete inspection data

4. **Data Integrity**
   - ✓ Yard 6-digit numbers unique system-wide (100000-999999)
   - ✓ Hive 6-digit numbers unique system-wide (100000-999999)
   - ✓ RFID tags unique across system (yards and hives)
   - ✓ Data persisted in NVS with CRC checksums
   - ✓ Timestamps recorded for all create/close operations
   - ✓ Status values validated ('A'/'C' for yards, 'A'/'S'/'D'/'O' for hives)

5. **User Interface**
   - ✓ Consistent interaction patterns (rotate, click, double-click, long-press)
   - ✓ All screens follow defined layout patterns
   - ✓ 6-digit number input works correctly with digit-by-digit entry
   - ✓ Back buttons work correctly
   - ✓ Visual and haptic feedback for all interactions

6. **Integration**
   - ✓ Reads inspection data from Beehive Inspection app
   - ✓ No data corruption between apps
   - ✓ RFID scanning integration works reliably

### Should Have (Nice to Have)

1. Batch hive addition (add multiple hives in one session)
2. Yard statistics caching to improve performance
3. Export yard/hive data to external file
4. Hive search/filter functionality by 6-digit number
5. Quick-entry mode for common 6-digit number patterns

### Won't Have (Future Consideration)

1. View/list all hives in a yard
2. Edit yard number or nickname after creation
3. Edit hive number after creation
4. Move hive between yards
5. Reactivate closed yards (change status from 'C' back to 'A')
6. Reactivate closed hives (change status from S/D/O back to 'A')
7. Direct integration with Beehive Inspection app for starting inspections
8. GPS coordinates for yard locations
9. Photo attachment for yards or hives
10. Sequential auto-numbering for yards/hives (all numbers are user-assigned)

---

## 12. Risks and Mitigations

### Risk 1: RFID Tag Conflicts
**Impact:** High  
**Likelihood:** Medium  
**Mitigation:**
- Implement comprehensive uniqueness validation
- Check both yard gates and hives when validating tags
- Provide clear error messages when duplicates detected
- Allow user to retry with different tag
- Log all RFID assignments for debugging

### Risk 2: NVS Storage Exhaustion
**Impact:** High  
**Likelihood:** Medium  
**Mitigation:**
- Monitor NVS usage and warn user when approaching capacity
- Implement storage cleanup for very old closed records
- Limit total yards and hives to reasonable numbers (100 yards, 1000 hives)
- Provide export/backup functionality to free up space

### Risk 3: Inspection Data Integration Issues
**Impact:** High  
**Likelihood:** Low  
**Mitigation:**
- Clearly define data contract between apps
- Handle missing or corrupted inspection data gracefully
- Display "No Data" when inspection data unavailable
- Version inspection data structure for future compatibility
- Test thoroughly with various inspection data states

### Risk 4: User Input Errors
**Impact:** Medium  
**Likelihood:** High  
**Mitigation:**
- Implement validation at every input step
- Provide clear error messages
- Allow easy correction (back navigation)
- Confirmation dialogs for destructive actions (close yard, close hive)
- Cannot proceed with invalid data

### Risk 5: Field Usability Issues
**Impact:** Medium  
**Likelihood:** Medium  
**Mitigation:**
- Test with actual beekeeping gloves
- Ensure screen brightness sufficient for sunlight
- Keep interaction patterns simple and consistent
- Minimize input requirements
- Conduct field testing with beekeepers

### Risk 6: Battery Life Insufficient
**Impact:** Medium  
**Likelihood:** Low  
**Mitigation:**
- Implement screen timeout/sleep mode
- Optimize display power usage
- Test battery life under realistic usage
- Provide low battery warnings
- Document expected battery life and charging requirements

---

## 13. Implementation Plan

### Phase 1: Core Data Layer (Week 1)
- Implement YardRecord and HiveRecord data structures
- Implement NVS serialization/deserialization
- Implement YardDataManager and HiveDataManager
- Unit tests for data layer
- NVS integration tests

### Phase 2: Basic Screens (Week 2)
- Screen 1: Yard List
- Screen 2: Management Menu
- Screen 3: Create Yard (number input, nickname input, no RFID yet)
- Screen 4: Close Yard
- 6-digit number input component
- Basic navigation between screens
- UI component development

### Phase 3: RFID Integration (Week 3)
- RFID scanner implementation
- RFID validation and uniqueness checking
- Add RFID scanning to Create Yard flow
- Test RFID hardware integration

### Phase 4: Hive Management (Week 4)
- Screen 5: Yard Summary (without statistics yet)
- Screen 6: Hive Management Input
- Screen 7: Add Hive (all steps including RFID)
- Screen 8: Close Hive (all steps including RFID)
- Hive data management implementation

### Phase 5: Statistics Integration (Week 5)
- Implement StatsCalculator
- Integration with Beehive Inspection app data
- Complete Yard Summary screen with real statistics
- Testing with various data scenarios

### Phase 6: Polish and Testing (Week 6)
- Visual design refinement
- Haptic and audio feedback tuning
- Comprehensive testing (unit, integration, UAT)
- Bug fixes and performance optimization
- Field testing with beekeepers

### Phase 7: Documentation and Release (Week 7)
- User documentation
- Developer documentation
- Code comments and cleanup
- Release preparation
- Deployment to devices

---

## 14. Dependencies

### Hardware Dependencies
- M5Stack Dial ESP32-S3 device
- GC9A01 240×240 round display
- RFID reader module (PN532 or compatible)
- RTC module (BM8563 or compatible)
- Battery and power management

### Software Dependencies
- ESP-IDF framework (v4.4+)
- MOONCAKE app framework
- NVS library
- RFID library (PN532 driver)
- Beehive Inspection app (for inspection data)

### External Dependencies
- RFID tags (ISO14443A compatible, NTAG or MIFARE)
- Sufficient NVS partition size (32KB minimum)

---

## Appendices

### Appendix A: Screen Navigation Map

```
                    ┌─────────────────┐
                    │   Main Menu     │
                    │   (MOONCAKE)    │
                    └────────┬────────┘
                             │
                             ↓
                    ┌─────────────────┐
                    │  1. Yard List   │◄────────┐
                    └────────┬────────┘         │
                             │                  │
                ┌────────────┼────────────┐     │
                │                         │     │
                ↓                         ↓     │
       ┌────────────────┐       ┌────────────────┐
       │  Select Yard   │       │  Management    │
       └────────┬───────┘       └────────┬───────┘
                │                        │
                │              ┌─────────┴────────┐
                │              │                  │
                │              ↓                  ↓
                │     ┌────────────────┐ ┌────────────────┐
                │     │ 3. Create Yard │ │ 4. Close Yard  │
                │     │   - Name       │ │   - Select     │
                │     │   - RFID Scan  │ │   - Confirm    │
                │     └────────┬───────┘ └────────┬───────┘
                │              │                  │
                │              └──────────┬───────┘
                │                         │
                │                         │ (both return to Yard List)
                │                         │
                ↓                         ↓
       ┌────────────────┐                │
       │ 5. Yard        │◄───────────────┘
       │    Summary     │
       │  - Statistics  │
       └────────┬───────┘
                │
                ↓
       ┌────────────────┐
       │ 6. Hive Mgmt   │
       │    Input       │
       └────────┬───────┘
                │
      ┌─────────┴─────────┐
      │                   │
      ↓                   ↓
┌────────────────┐  ┌────────────────┐
│ 7. Add Hive    │  │ 8. Close Hive  │
│   - Number     │  │   - Select     │
│   - Origin     │  │   - Reason     │
│   - RFID Scan  │  │   - RFID Scan  │
└────────┬───────┘  └────────┬───────┘
         │                   │
         └─────────┬─────────┘
                   │
                   ↓
         (both return to Yard Summary)
```

### Appendix B: Example NVS Data Layout

```cpp
// Example yard record
Yard Number: 100001
Nickname: "West Apiary"
RFID Gate Tag: "9A3F2B1087654321"
Created: 1707696000 (Unix timestamp)
Closed: 0 (not closed)
Status: 'A' (Active)

NVS Key: "ym:yard:100001"
NVS Value (hex):
00 01 86 A1                          // yardNumber = 100001
57 65 73 74 20 41 70 69 61 72 79 00  // nickname = "West Apiary\0"
00 00 00 00 00 00 00 00 00           // nickname padding
39 41 33 46 32 42 31 30 38 37 36 35  // rfidTagId = "9A3F2B1087654321"
34 33 32 31 00                       // rfidTagId continued
65 C3 32 00                          // creationTimestamp
00 00 00 00                          // closureTimestamp = 0
41                                   // status = 'A' (Active)
A7 2E                                // CRC16 checksum
// Total: 53 bytes

// Example hive record (active)
Hive Number: 123456
Yard Number: 100001
Origin: Split from Hive 100123
RFID Tag: "7B2E4C91AA55BB66"
Created: 1707782400
Status: 'A' (Active)

NVS Key: "ym:hive:123456"
NVS Value (hex):
00 01 E2 40                          // hiveNumber = 123456
00 01 86 A1                          // yardNumber = 100001
01                                   // originType = SPLIT
00 01 87 0B                          // originHiveNumber = 100123
37 42 32 45 34 43 39 31 41 41 35 35  // rfidTagId
42 42 36 36 00                       // rfidTagId continued
65 C4 A0 00                          // creationTimestamp
00 00 00 00                          // closureTimestamp = 0
41                                   // status = 'A' (Active)
B3 4F                                // CRC16 checksum
// Total: 49 bytes

// Example hive record (closed - dead)
Hive Number: 234567
Yard Number: 100001
Created: 1706140800
Closed: 1707955200
Status: 'D' (Dead)
RFID Tag: (deleted)

NVS Key: "ym:hive:234567"
NVS Value (hex):
00 03 94 E7                          // hiveNumber = 234567
00 01 86 A1                          // yardNumber = 100001
00                                   // originType = SWARM
00 00 00 00                          // originHiveNumber = 0
00 00 00 00 00 00 00 00 00 00 00 00  // rfidTagId = "" (deleted)
00 00 00 00 00                       // rfidTagId continued (empty)
65 B2 E0 00                          // creationTimestamp
65 CD 50 00                          // closureTimestamp
44                                   // status = 'D' (Dead)
C8 91                                // CRC16 checksum
// Total: 49 bytes

// Example closed yard
Yard Number: 100002
Nickname: "Old Site"
Created: 1705536000
Closed: 1707782400
Status: 'C' (Closed)

NVS Key: "ym:yard:100002"
NVS Value (hex):
00 01 86 A2                          // yardNumber = 100002
4F 6C 64 20 53 69 74 65 00           // nickname = "Old Site\0"
00 00 00 00 00 00 00 00 00 00 00 00  // nickname padding
45 42 38 43 32 44 31 46 39 33 36 41  // rfidTagId (kept for reference)
37 42 32 38 00                       // rfidTagId continued
65 A8 D0 00                          // creationTimestamp
65 C4 A0 00                          // closureTimestamp
43                                   // status = 'C' (Closed)
D2 1A                                // CRC16 checksum
// Total: 53 bytes
```

### Appendix C: RFID Tag Format

**Supported Tag Types:**
- NTAG213, NTAG215, NTAG216
- MIFARE Classic 1K, 4K
- MIFARE Ultralight

**Tag ID Format:**
- 8 bytes (64 bits)
- Hexadecimal representation: 16 characters
- Example: "9A3F2B1087654321"
- Stored as null-terminated string in records

**Scanning Process:**
1. Initialize RFID reader
2. Set timeout (10 seconds)
3. Wait for tag presence
4. Read tag UID
5. Convert to hex string
6. Validate format and uniqueness
7. Store in record or reject

**Error Handling:**
- Timeout: "No tag detected"
- Read error: "Scan failed, retry?"
- Duplicate: "Tag already in use"
- Invalid format: "Invalid tag"

### Appendix D: Statistics Calculation Details

**Queenright Hive Count:**
```cpp
uint16_t countQueenrightHives(uint32_t yardNumber) {
    uint16_t count = 0;
    HiveRecord* hives = getActiveHivesInYard(yardNumber);
    
    for (each hive in hives) {
        // Only count active hives (status 'A')
        if (hive.status != 'A') continue;
        
        InspectionRecord inspection = getLatestInspection(hive.hiveNumber);
        if (inspection.isComplete && 
            inspection.queenRight.isSet &&
            inspection.queenRight.status == QUEEN_RIGHT_YES) {
            count++;
        }
    }
    
    return count;
}
```

**Total Super Count:**
```cpp
uint16_t countTotalSupers(uint32_t yardNumber) {
    uint16_t total = 0;
    HiveRecord* hives = getActiveHivesInYard(yardNumber);
    
    for (each hive in hives) {
        // Only count active hives (status 'A')
        if (hive.status != 'A') continue;
        
        InspectionRecord inspection = getLatestInspection(hive.hiveNumber);
        if (inspection.isComplete && inspection.supers.isSet) {
            total += inspection.supers.superCount;
        }
    }
    
    return total;
}
```

**Average Fill Percentage:**
```cpp
float calculateAverageFillPercentage(uint32_t yardNumber) {
    float totalFill = 0.0f;
    uint16_t superCount = 0;
    
    HiveRecord* hives = getActiveHivesInYard(yardNumber);
    
    for (each hive in hives) {
        // Only process active hives (status 'A')
        if (hive.status != 'A') continue;
        
        InspectionRecord inspection = getLatestInspection(hive.hiveNumber);
        if (inspection.isComplete && inspection.supers.isSet) {
            for (int i = 0; i < inspection.supers.superCount; i++) {
                FillPercentage fill = inspection.supers.fillLevels[i];
                if (fill != FILL_UNKNOWN) {
                    // Convert enum to percentage
                    float percent = 0.0f;
                    switch (fill) {
                        case FILL_0:   percent = 0.0f; break;
                        case FILL_25:  percent = 25.0f; break;
                        case FILL_50:  percent = 50.0f; break;
                        case FILL_75:  percent = 75.0f; break;
                        case FILL_100: percent = 100.0f; break;
                    }
                    totalFill += percent;
                    superCount++;
                }
            }
        }
    }
    
    if (superCount == 0) return 0.0f;
    return totalFill / superCount;
}

// Helper function - returns only hives with status 'A'
HiveRecord* getActiveHivesInYard(uint32_t yardNumber) {
    // Query all hive records where:
    //   yardNumber matches AND status == 'A'
    // Return array of matching HiveRecord structures
}
```

### Appendix E: Color Reference

```cpp
// Color definitions (RGB565 format)
// Same as Beehive Inspection app for consistency
#define COLOR_BACKGROUND     0x1082  // Dark blue #1a1a2e
#define COLOR_TEXT_PRIMARY   0xFFFF  // White
#define COLOR_TEXT_SECONDARY 0x8410  // Gray
#define COLOR_ACCENT_ORANGE  0xFB60  // M5Stack orange #ff6b35
#define COLOR_ICON_GOLD      0xFFE0  // Gold #ffd700
#define COLOR_SUCCESS        0x07E0  // Green
#define COLOR_ERROR          0xF800  // Red
#define COLOR_HIGHLIGHT_BG   0x4208  // Dark highlight
#define COLOR_BORDER         0x632C  // Border/separator
```

### Appendix F: Interaction Timing Constants

```cpp
#define DEBOUNCE_TIME_MS        50    // Button debounce
#define CLICK_MAX_MS            300   // Max duration for single click
#define DOUBLE_CLICK_WINDOW_MS  500   // Window for detecting double click
#define LONG_PRESS_MS           800   // Duration for long press
#define ROTATION_COOLDOWN_MS    100   // Min time between rotations
#define TRANSITION_ANIM_MS      200   // Screen transition duration
#define FEEDBACK_FLASH_MS       100   // Visual feedback duration
#define RFID_SCAN_TIMEOUT_MS    10000 // RFID scan timeout (10 seconds)
#define RFID_RETRY_DELAY_MS     500   // Delay between RFID retry attempts
```

### Appendix G: References

1. M5Stack Dial Product Page: https://docs.m5stack.com/en/core/Dial
2. MOONCAKE Framework Documentation: (internal project docs)
3. ESP32-S3 Technical Reference: https://www.espressif.com/en/products/socs/esp32-s3
4. PN532 RFID Module Documentation: https://www.nxp.com/docs/en/user-guide/141520.pdf
5. ISO14443A Standard: (RFID tag specification)
6. Beehive Inspection App PRD: PRD_Beehive_Inspection_Complete.md

---

## Document Revision History

| Version | Date | Author | Changes |
|---------|------|--------|---------|
| 0.1 | 2026-02-11 | Claude | Initial draft |
| 1.0 | 2026-02-11 | Claude | Complete PRD for review |
| 1.1 | 2026-02-12 | Claude | Updated numbering system: yards and hives now use 6-digit numbers (100000-999999); status is separate field ('A'/'C' for yards, 'A'/'S'/'D'/'O' for hives); yards have nicknames; all numbers are user-assigned |

---

**End of Document**
