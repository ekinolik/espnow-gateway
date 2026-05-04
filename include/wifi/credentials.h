#pragma once

#include "app_config.h"

#include <Preferences.h>

namespace Wifi {
    struct CredentialData {
        String SSID;
        String PASSWORD;

        bool isValid() const {
            return SSID.length() > 0;
        }
    };

    class Credentials {
        public:
        static const Wifi::CredentialData credentials;

        static Wifi::CredentialData get();

        static bool save(const Wifi::CredentialData& creds);
        static bool set(const char* ssid, const char* password);
        static bool setAndSave(const char* ssid, const char* password);

        static bool clearPrefs();

        private:
        static Wifi::CredentialData m_credentials;

        static Wifi::CredentialData getFromStorage();
    };
}