#pragma once

#include "connector.h"
#include "credentials.h"
#include "portal.h"
#include "scan.h"

namespace WifiMgr {
    void begin(bool needInit = true);

    // Connector
    bool connect();
    bool isConnected();

    // Credentials
    bool hasCredentials();
    CredentialData getCredentials();
    bool clearCredentialPrefs();
    bool saveCredentials(const CredentialData& creds);
    bool setCredentials(const char* ssid, const char* password);
    bool setAndSaveCredentials(const char* ssid, const char* password);


    // Portal - instance based
    Portal& portal();
}