#pragma once

#include "app_config.h"

#include <Preferences.h>

namespace WifiMgr {
    struct CredentialData {
        String SSID;
        String PASSWORD;

        bool isValid() const {
            return SSID.length() > 0;
        }
    };

    class Credentials {
        public:
        static const WifiMgr::CredentialData credentials;

        static WifiMgr::CredentialData get();

        static bool save(const WifiMgr::CredentialData& creds);
        static bool set(const char* ssid, const char* password);
        static bool setAndSave(const char* ssid, const char* password);

        static bool clearPrefs();

        private:
        static WifiMgr::CredentialData m_credentials;

        static WifiMgr::CredentialData getFromStorage();
    };
}