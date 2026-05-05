#pragma once

#include "app_config.h"
#include "wifi/credentials.h"

#include <WiFi.h>
#include <esp_wifi.h>

namespace WifiMgr {
    class Connector {
        public:
        static void begin(bool needInit);
        static void init();
        static bool connect();

        static bool connected() { return m_connected; }
        static bool loadCredentials();
        static bool areCredentialsValid() { return m_credentials.isValid(); }

        private:
        static bool m_connected;
        static CredentialData m_credentials;
    };
}