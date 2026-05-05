#pragma once

#include <Arduino.h>

#define BOARD "ESP32-C6"
#define BOARD_MAKER "seeed"
#define FW_NAME "espnow-gateway"
#define FW_VERSION_MAJOR 0
#define FW_VERSION_MINOR 0
#define FW_VERSION_BUILD 1
#define FW_VERSION_BUILD_TIME __DATE__ " " __TIME__
#define PROTOCOL_VERSION 1

namespace AppConfig {
    // ESPNOW
    static constexpr uint8_t ESPNOW_CHANNEL = 0; // 0 means to use current WiFi channel, otherwise set this to a fixed channel
    static constexpr size_t ESPNOW_RX_QUEUE_SIZE = 8;
    static constexpr bool ESPNOW_REQUIRE_KNOWN_PEERS = false; // start open

    static constexpr uint32_t ESPNOW_ACK_TIMEOUT_MS = 400;
    static constexpr uint8_t ESPNOW_MAX_RETRIES = 5;

    // WiFi
    static constexpr const char* WIFI_SSID = "";
    static constexpr const char* WIFI_PASSWORD = "";
    static constexpr uint32_t WIFI_CONNECT_TIMEOUT = 15000; // ms
    static constexpr bool WIFI_SLEEPY_MODE = false; // Whether to put WiFi in modem sleep when not actively transmitting (saves power, but may increase latency)
    static constexpr bool WIFI_PERSISTENT = false; // Should we save WiFi config in flash to persist across boots?


    // WIFI Portal
    static constexpr const char* PORTAL_HOSTNAME = "espnow-gateway";
    static constexpr const char* PORTAL_AP_SSID = "ESPNOW-GATEWAY-SETUP";
    static constexpr const char* PORTAL_AP_PASS = "configureme";
    static constexpr uint32_t PORTAL_STA_CONNECT_TIMEOUT_MS = 15000; // ms

    // Debug
    static const uint32_t DEBUG_MODE_DELAY_MS = 10000;
    static constexpr const bool CLEAR_WIFI_CREDS = false; // For testing: clear saved WiFi credentials
    static constexpr const bool FORCE_AP_MODE = false; // For testing: force start in AP mode even if credentials are valid

}