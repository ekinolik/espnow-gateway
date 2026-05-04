#include "wifi/credentials.h"

Wifi::CredentialData Wifi::Credentials::m_credentials;

namespace Wifi {
    CredentialData Credentials::get() {
        if (m_credentials.isValid()) {
            return m_credentials;
        }

        // No valid credentials in RAM, try storage
        m_credentials = getFromStorage();

        return m_credentials;
    }

    CredentialData Credentials::getFromStorage() {
        Preferences prefs;

        String ssid = AppConfig::WIFI_SSID;
        String password = AppConfig::WIFI_PASSWORD;
        if (prefs.begin("wifi", true)) {
            ssid = prefs.getString("ssid", AppConfig::WIFI_SSID);
            password = prefs.getString("password", AppConfig::WIFI_PASSWORD);
            Serial.println("[CREDENTIALS] Loaded credentials from storage");
        }

        Serial.printf("[CREDENTIALS] Loaded credentials. SSID=%s, Password=%s\n", ssid.c_str(), password.c_str());

        return CredentialData{ssid, password};
    }

    bool Credentials::save(const CredentialData& creds) {
        if (!creds.isValid()) {
            return false;
        }

        Preferences prefs;
        prefs.begin("wifi", false); // read-write
        prefs.putString("ssid", creds.SSID);
        prefs.putString("password", creds.PASSWORD);

        String tmp_ssid = prefs.getString("ssid", "n/a");
        String tmp_pass = prefs.getString("password", "n/a");
        Serial.printf("[CREDENTIALS] Saved credentials. SSID=%s, Password=%s\n", tmp_ssid.c_str(), tmp_pass.c_str());

        m_credentials = creds;

        return true;
    }

    bool Credentials::set(const char* ssid, const char* password) {
        m_credentials = CredentialData{ssid, password};
        if (!m_credentials.isValid()) {
            return false;
        }

        return true;
    }

    bool Credentials::setAndSave(const char* ssid, const char* password) {
        if (!set(ssid, password)) {
            return false;
        }

        return save(m_credentials);
    }
}