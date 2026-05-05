#include "wifi/connector.h"

//WifiConnector::WifiConnector() : m_connected(false) {};

namespace WifiMgr {
    bool Connector::m_connected = false;
    CredentialData Connector::m_credentials = {};

    void Connector::begin(bool needInit) {
        if (needInit) {
            Connector::init();
        }

        loadCredentials();
    }

    void Connector::init() {
        WiFi.mode(WIFI_STA);
        WiFi.disconnect();
        delay(100);
    }

    bool Connector::connect() {
        if (!WifiMgr::Connector::m_credentials.isValid()) {
            Serial.println("[WIFI] No valid credentials, cannot connect");
            m_connected = false;
            return false;
        }

        WiFi.mode(WIFI_STA);
        WiFi.persistent(AppConfig::WIFI_PERSISTENT);
        WiFi.setSleep(AppConfig::WIFI_SLEEPY_MODE);

        Serial.printf("[WIFI] Connecting to SSID=%s\n", m_credentials.SSID.c_str());
        Serial.printf("[WIFI] SSID: %s, Password: %s\n", m_credentials.SSID.c_str(), m_credentials.PASSWORD.c_str());
        WiFi.begin(m_credentials.SSID.c_str(), m_credentials.PASSWORD.c_str());

        uint32_t start = millis();
        while (WiFi.status() != WL_CONNECTED && millis() - start < AppConfig::WIFI_CONNECT_TIMEOUT) {
            delay(250);
            Serial.print(".");
        }
        Serial.println();

        if (WiFi.status() != WL_CONNECTED) {
            Serial.printf("[WIFI] Failed to connect status=%d\n", WiFi.status());
            m_connected = false;
            return false;
        }

        uint8_t primary;
        wifi_second_chan_t secondary;
        esp_wifi_get_channel(&primary, &secondary);

        Serial.printf("[WIFI] Connected! IP=%s, RSSI=%d, Channel=%d\n",
            WiFi.localIP().toString().c_str(),
            WiFi.RSSI(),
            primary
        );

        m_connected = true;

        return true;
    }

    bool Connector::loadCredentials() {
        m_credentials = Credentials::get();
        if (!m_credentials.isValid()) {
            Serial.println("[WIFI] No valid credentials found");
            return false;
        }

        Serial.printf("[WIFI] Loaded credentials: SSID=%s\n", m_credentials.SSID.c_str());
        return true;
    }
}