#include "wifi/credentials.h"

WifiMgr::CredentialData WifiMgr::Credentials::m_credentials;

namespace WifiMgr {
    CredentialData Credentials::get() {
        if (m_credentials.isValid()) {
            Serial.printf("[CREDENTIALS] Valid credentials for %s found in RAM\n", m_credentials.SSID.c_str());
            return m_credentials;
        }

        // No valid credentials in RAM, try storage
        m_credentials = getFromStorage();

        if (m_credentials.isValid()) {
            Serial.printf("[CREDENTIALS] Valid credentials for %s found in storage\n", m_credentials.SSID.c_str());
        } else {
            // No valid credentials in storage, use from config
            String ssid = AppConfig::WIFI_SSID;
            String password = AppConfig::WIFI_PASSWORD;
            m_credentials = CredentialData{ssid, password};
            Serial.printf("[CREDENTIALS] No valid credentials found in storage. Loaded credentials for %s from config\n", m_credentials.SSID.c_str());
        }

        return m_credentials;
    }

    CredentialData Credentials::getFromStorage() {
        Preferences prefs;

        String ssid;
        String password;
        if (prefs.begin("wifi", true)) {
            ssid = prefs.getString("ssid", AppConfig::WIFI_SSID);
            password = prefs.getString("password", AppConfig::WIFI_PASSWORD);
        }
        prefs.end();

        return CredentialData{ssid, password};
    }

    bool Credentials::clearPrefs() {
        Preferences prefs;
        if (!prefs.begin("wifi", false)) {
            Serial.println("[CREDENTIALS] Failed to open preferences for clearing");
            return false;
        }

        bool success = prefs.clear();
        prefs.end();

        if (success) {
            Serial.println("[CREDENTIALS] Cleared credentials from storage");
            m_credentials = {};
        } else {
            Serial.println("[CREDENTIALS] Failed to clear credentials from storage");
        }

        return success;
    }

    bool Credentials::save(const CredentialData& creds) {
        if (!creds.isValid()) {
            return false;
        }

        CredentialData tmp_credentials = Credentials::getFromStorage();
        bool newSSID = true, newPass = true;
        if (tmp_credentials.SSID == creds.SSID) {
            Serial.println("[CREDENTIALS] SSID is the same as stored value, not updating storage");
            newSSID = false;
        }
        if (tmp_credentials.PASSWORD == creds.PASSWORD) {
            Serial.println("[CREDENTIALS] Password is the same as stored value, not updating storage");
            newPass = false;
        }

        Preferences prefs;
        prefs.begin("wifi", false); // read-write
        // Only update storage if values are different to avoid unnecessary flash writes
        if (newSSID) prefs.putString("ssid", creds.SSID);
        if (newPass) prefs.putString("password", creds.PASSWORD);
        prefs.end();

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