# ESP-NOW Wi-Fi Gateway (ESP32-C6)

Firmware for an ESP32-C6 that acts as a minimal ESP-NOW receiver/bridge endpoint.  

## What It Does

- Starts Wi-Fi in STA mode and initializes ESP-NOW on a fixed channel
- Receives ESP-NOW packets, validates packet size/version, and queues parsed messages
- Tracks per-device sequence numbers to suppress duplicate processing
- Sends ESP-NOW ACK packets back to the sender (including duplicate packets)
- Prints boot state and received packet details over serial for visibility/debugging

## Hardware Requirements

- ESP32-C6 board (for example ESP32-C6-DevKitM-1)
- USB cable for flashing and serial monitor


## Getting Started

### Prerequisites

- [PlatformIO Core](https://docs.platformio.org/en/latest/core/installation/index.html) or VS Code with the [PlatformIO IDE extension](https://marketplace.visualstudio.com/items?itemName=platformio.platformio-ide)
- ESP32-C6 development board

### Build

```bash
platformio run -e esp32-c6
```

### Flash

```bash
platformio run -e esp32-c6 --target upload
```

### Monitor Serial Output

```bash
platformio device monitor -e esp32-c6 -b 115200
```

### Example Startup Output

```text
[BOOT] Boot count: 1
[BOOT] Wake cause: UNDEFINED
[BOOT] Last shutdown reason: NONE
[BOOT] Reset reason 1
Boot Complete
Starting Bridge Setup...
[ESPNOW] Ready, MAC=AA:BB:CC:DD:EE:FF
[MAIN] ESPNOW Receiver init OK
```

## High-Level Flow

1. `setup()` initializes `BootState` and `EspNowReceiver`.
2. ESP-NOW receive callback copies raw packets into an internal RX queue.
3. `EspNowReceiver::update()` parses and validates packets into parsed-message queue.
4. `loop()` drains parsed messages, deduplicates by `(deviceID, sequence)`, and sends ACK.

## Project Modules

### `src/main.cpp`

- Entry point and main processing loop
- Duplicate detection (`isDuplicate`) and sequence tracking (`markSeen`)
- Packet printing and ACK send control

### `src/espnow_receiver.cpp` / `include/espnow_receiver.h`

- ESP-NOW init and receive callback registration
- Raw RX queue + parsed-message queue
- Packet validation/parsing and peer auto-add behavior
- ACK packet construction and transmission

### `src/boot_state.cpp` / `include/boot_state.h`

- RTC-backed boot counter and previous shutdown reason
- Boot summary printing on startup

### `include/log_protocol.h`

Defines packet formats used over ESP-NOW:

- `LogMessageV1` (data message structure)
- `AckMessageV1` (ACK structure)
- protocol version/message type constants

> Note: this protocol uses `Log*` message type names.

## Configuration

Tunable values are defined in `include/app_config.h`:

| Constant | Default | Description |
|----------|---------|-------------|
| `ESPNOW_CHANNEL` | `1` | Wi-Fi/ESP-NOW channel used by the gateway |
| `ESPNOW_RX_QUEUE_SIZE` | `8` | Size of raw/parsed receive queues |
| `ESPNOW_REQUIRE_KNOWN_PEERS` | `false` | If `false`, unknown peers are auto-added before ACK |
| `ESPNOW_ACK_TIMEOUT_MS` | `400` | ACK timeout value for sender-side retry policy |
| `ESPNOW_MAX_RETRIES` | `5` | Suggested sender-side retry count |
