# BLE Communication Specification

**Version:** 1.1
**Date:** 2026-03-20
**Device:** M5Dial Bee Inspection Report

## Overview

This document describes the BLE (Bluetooth Low Energy) GATT service used by the M5Dial device for transferring bee inspection and yard management data to companion mobile applications.

## Service Information

| Property | Value |
|----------|-------|
| Service Name | Bee Data Service |
| Service UUID (128-bit) | `12345678-1234-5678-1234-56789ABCDEF0` |
| Device Name Pattern | `BeeData-XXXXXXXX` (where X is derived from MAC address) |

## Characteristics

### Command Characteristic (0xFF01)

| Property | Value |
|----------|-------|
| UUID | `0xFF01` |
| Properties | Write, Write Without Response |
| Description | Send commands to the device |

### Response Characteristic (0xFF02)

| Property | Value |
|----------|-------|
| UUID | `0xFF02` |
| Properties | Read, Notify |
| Description | Receive data responses from the device |

**Important:** Subscribe to notifications on this characteristic to receive responses.

### Status Characteristic (0xFF03)

| Property | Value |
|----------|-------|
| UUID | `0xFF03` |
| Properties | Read |
| Description | Read connection and transfer status |

**Status Format (4 bytes):**
| Byte | Description |
|------|-------------|
| 0 | Connected (0 or 1) |
| 1 | Subscribed (0 or 1) |
| 2 | Transfer in progress (0 or 1) |
| 3 | Transfer progress (0-100%) |

---

## Command Reference

### Query Commands

| Command | Code | Parameters | Description |
|---------|------|------------|-------------|
| `CMD_PING` | `0xFF` | None | Test connection, returns "PONG" |
| `CMD_GET_DEVICE_INFO` | `0x01` | None | Get device name and version |
| `CMD_GET_INSPECTION_COUNT` | `0x02` | None | Get count of inspection records |
| `CMD_GET_INSPECTION_IDS` | `0x03` | None | Get list of all inspection IDs |
| `CMD_GET_INSPECTION` | `0x04` | 4-byte ID (little-endian) | Get specific inspection by ID |
| `CMD_GET_YARD_COUNT` | `0x05` | None | Get count of yards |
| `CMD_GET_YARD_LIST` | `0x06` | None | Get list of yard numbers |
| `CMD_GET_YARD` | `0x07` | 4-byte yard number (LE) | Get specific yard |
| `CMD_GET_HIVE_COUNT` | `0x08` | None | Get count of hives |
| `CMD_GET_HIVES_IN_YARD` | `0x09` | 4-byte yard number (LE) | Get hives in a yard |
| `CMD_GET_HIVE` | `0x0A` | 4-byte hive number (LE) | Get specific hive |
| `CMD_GET_ALL_DATA` | `0x0B` | None | Stream all data |
| `CMD_GET_EQUIPMENT_PLAN_COUNT` | `0x0C` | None | Get count of equipment plans |
| `CMD_GET_EQUIPMENT_PLAN` | `0x0D` | 4-byte yard number (LE) | Get equipment plan for yard |
| `CMD_GET_EQUIPMENT_PLANS` | `0x0E` | None | Get all equipment plans |

### Action Commands

| Command | Code | Parameters | Description |
|---------|------|------------|-------------|
| `CMD_SET_TIME` | `0x20` | 4-byte Unix timestamp (LE) | Set device RTC time |
| `CMD_CLEAR_EQUIPMENT_PLANS` | `0x30` | None | Clear all equipment plans |

---

## Response Format

All responses start with a status byte followed by command-specific data.

### Response Status Codes

| Code | Name | Description |
|------|------|-------------|
| `0x00` | `RESP_OK` | Success |
| `0x01` | `RESP_ERROR` | Error occurred |
| `0x02` | `RESP_NOT_FOUND` | Requested data not found |
| `0x03` | `RESP_BUSY` | Device busy |
| `0x10` | `RESP_DATA_START` | Start of multi-packet data |
| `0x11` | `RESP_DATA_CONT` | Continuation of data |
| `0x12` | `RESP_DATA_END` | End of data transfer |

### Response Packet Structure

**Simple Response:**
```
[Status: 1 byte][Command: 1 byte][Data: variable]
```

**Multi-packet Data:**
```
Start:    [0x10][Command][Header data...]
Continue: [0x11][Marker][JSON data...]
End:      [0x12][Command]
```

---

## Detailed Command Specifications

### CMD_PING (0xFF)

**Request:** `[0xFF]`

**Response:** `[0x00]['P']['O']['N']['G']`

---

### CMD_GET_DEVICE_INFO (0x01)

**Request:** `[0x01]`

**Response:** `[0x00][0x01][JSON string]`

**JSON Format:**
```json
{"name":"BeeData-XXXXXXXX","version":"1.0"}
```

---

### CMD_GET_INSPECTION_COUNT (0x02)

**Request:** `[0x02]`

**Response:** `[0x00][0x02][count_lo][count_hi]`

---

### CMD_GET_YARD_COUNT (0x05)

**Request:** `[0x05]`

**Response:** `[0x00][0x05][count_lo][count_hi]`

---

### CMD_GET_HIVE_COUNT (0x08)

**Request:** `[0x08]`

**Response:** `[0x00][0x08][count_lo][count_hi]`

---

### CMD_GET_EQUIPMENT_PLAN_COUNT (0x0C)

**Request:** `[0x0C]`

**Response:** `[0x00][0x0C][count_lo][count_hi]`

---

### CMD_GET_ALL_DATA (0x0B)

Streams all yards, hives, inspections, and equipment plans.

**Request:** `[0x0B]`

**Response Sequence:**

1. **Header:**
   ```
   [0x10][0x0B][yard_count_lo][yard_count_hi][hive_count_lo][hive_count_hi][inspection_count_lo][inspection_count_hi][equipment_plan_count_lo][equipment_plan_count_hi]
   ```

2. **Yards (marker 'Y'):**
   ```
   [0x11]['Y'][JSON...]
   ```
   ```json
   {"type":"yard","num":123456,"name":"North Field","rfid":"ABCD1234","status":"A","hives":5}
   ```

3. **Hives (marker 'H'):**
   ```
   [0x11]['H'][JSON...]
   ```
   ```json
   {"type":"hive","num":100001,"yard":123456,"rfid":"EF567890","status":"A","origin":0}
   ```

4. **Inspections (marker 'I'):**
   ```
   [0x11]['I'][JSON...]
   ```
   ```json
   {"type":"inspection","id":1,"hive":100001,"ts":1710892800,"datetime":"20 Mar 26 10:00","qr":1,"sc":0,"ssc":0,"supers":2,"temp":1,"brood":4,"treat":0,"pests":0}
   ```

5. **Equipment Plans (marker 'E'):**
   ```
   [0x11]['E'][JSON...]
   ```
   ```json
   {"type":"equipment_plan","yard":123456,"ts":1710892800,"brood_boxes":5,"supers":10,"bottom_boards":5,"inner_lids":5,"outer_lids":5,"queen_excluders":5,"hive_stands":5,"insulation":0,"entrance_reducers":5,"feeders":3}
   ```

6. **End:**
   ```
   [0x12][0x0B]
   ```

---

### CMD_GET_EQUIPMENT_PLANS (0x0E)

Streams all equipment plans.

**Request:** `[0x0E]`

**Response Sequence:**

1. **Header:**
   ```
   [0x10][0x0E][count_lo][count_hi]
   ```

2. **Equipment Plans (marker 'E'):**
   ```
   [0x11]['E'][JSON...]
   ```

3. **End:**
   ```
   [0x12][0x0E]
   ```

---

### CMD_SET_TIME (0x20)

Sets the device RTC to the specified Unix timestamp.

**Request:** `[0x20][timestamp_byte0][timestamp_byte1][timestamp_byte2][timestamp_byte3]`

Timestamp is a 32-bit Unix timestamp in little-endian format.

**Response:** `[0x00][0x20][timestamp echo...]`

---

### CMD_CLEAR_EQUIPMENT_PLANS (0x30)

Clears all equipment plans from the device.

**Request:** `[0x30]`

**Response:** `[0x00][0x30]` on success, `[0x01][0x30]` on error

---

## Data Type Definitions

### Yard Status Codes
| Code | Meaning |
|------|---------|
| `A` | Active |
| `C` | Closed |

### Hive Status Codes
| Code | Meaning |
|------|---------|
| `A` | Active |
| `S` | Sold |
| `D` | Dead |
| `O` | Absconded |

### Hive Origin Types
| Value | Meaning |
|-------|---------|
| `0` | Swarm |
| `1` | Split |

### Temperament Values
| Value | Meaning |
|-------|---------|
| `0` | Calm |
| `1` | Nervous |
| `2` | Aggressive |

### Treatment Values
| Value | Meaning |
|-------|---------|
| `0` | None |
| `1` | OA Dribble |
| `2` | OA Vaporize |
| `3` | Thymol |
| `4` | Amitraz |
| `5` | Other |

### Equipment Types (Equipment Plan Index)
| Index | Equipment Type |
|-------|---------------|
| 0 | Brood Boxes |
| 1 | Supers |
| 2 | Bottom Boards |
| 3 | Inner Lids |
| 4 | Outer Lids |
| 5 | Queen Excluders |
| 6 | Hive Stands |
| 7 | Insulation |
| 8 | Entrance Reducers |
| 9 | Feeders |

---

## Implementation Notes

### Connection Flow

1. Scan for devices with name starting with `BeeData-`
2. Connect to the device
3. Discover services and characteristics
4. Subscribe to notifications on Response characteristic (0xFF02)
5. Send commands via Command characteristic (0xFF01)
6. Process responses via notifications

### Data Transfer Recommendations

- Add a 20ms delay between processing each received packet
- Large transfers (CMD_GET_ALL_DATA) may take several seconds
- Monitor the Status characteristic for transfer progress
- Handle BLE MTU limitations (default 20 bytes, negotiate higher if possible)

### Error Handling

- If no response within 5 seconds, consider the command failed
- On disconnect during transfer, restart the transfer from the beginning
- Validate JSON parsing for each received data packet

---

## Version History

| Version | Date | Changes |
|---------|------|---------|
| 1.0 | 2026-03-19 | Initial release |
| 1.1 | 2026-03-20 | Added equipment plan commands (0x0C, 0x0D, 0x0E, 0x30), updated CMD_GET_ALL_DATA to include equipment plans |
