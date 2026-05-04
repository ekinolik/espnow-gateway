#pragma once

#include "app_config.h"
#include "wifi/credentials.h"

#include <WiFi.h>
#include <esp_wifi.h>

namespace Wifi {
    class Connector {
        public:
        void begin(bool needInit);
        void init();
        bool connect();

        bool connected() const { return m_connected; }
        bool loadCredentials();
        bool areCredentialsValid() const { return m_credentials.isValid(); }

        private:
        bool m_connected = false;
        CredentialData m_credentials;
    };
}