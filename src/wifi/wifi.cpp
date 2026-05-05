#include "wifi/wifi.h"

namespace WifiMgr {

    static Portal s_portal{Portal::Config()};

    // Connector
    void begin(bool needInit) {
        Connector::begin(needInit);
    }

    bool connect() {
        return Connector::connect();
    }

    bool isConnected() {
        return Connector::connected();
    }

    bool hasCredentials() {
        return Connector::areCredentialsValid();
    }


    // Credentials helpers
    CredentialData getCredentials() {
        return Credentials::get();
    }

    bool clearCredentialPrefs() {
        return Credentials::clearPrefs();
    }

    bool saveCredentials(const CredentialData& creds) {
        return Credentials::save(creds);
    }

    bool setCredentials(const char* ssid, const char* password) {
        return Credentials::set(ssid, password);
    }

    bool setAndSaveCredentials(const char* ssid, const char* password) {
        return Credentials::setAndSave(ssid, password);
    }


    // Portal instance
    Portal& portal() {
        return s_portal;
    }
}