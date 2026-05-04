#pragma once

#include "app_config.h"

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DNSServer.h>
#include <Preferences.h>

namespace Wifi {
    class Portal {
    public:
        void factoryResetWifi(); // Clears SSID/password from NVS

        struct Config {
            const char* hostname = AppConfig::PORTAL_HOSTNAME;

            // Fallback AP
            const char* apSSID = AppConfig::PORTAL_AP_SSID;
            const char* apPass = AppConfig::PORTAL_AP_PASS;

            // Captive portal subnet
            IPAddress apIP = IPAddress(172, 16, 0, 1);
            IPAddress apGW = IPAddress(172, 16, 0, 1);
            IPAddress apMask = IPAddress(255, 255, 255, 0);

            // STA connect timeout
            uint32_t staConnectTimeoutMs = 15000;

        };

        enum class Mode { STA, AP };

        explicit Portal(const Config& cfg);

        // Begin Wifi:
        // If saved creds exist and connect works -> STA
        // Else start AP + captive portal -> AP
        void begin();

        void loop();

        bool isApMode() const {return mode_ == Mode::AP;}
        bool isStaMode() const {return mode_ == Mode::STA;}

        WebServer& web() {return server_;}

        IPAddress staIP() const { return WiFi.localIP(); }
        IPAddress apIP() const { return WiFi.softAPIP(); }
        const char* hostname() const { return cfg_.hostname; }

        // helpers
        String saveSSID() const;

    private:
        Config cfg_;
        Mode mode_ = Mode::AP;

        WebServer server_{80};
        DNSServer dns_;
        Preferences prefs_;

        bool connectSta_(const String& ssid, const String& pass);
        void startApPortal_();

        void registerCommonRoutes_();
        void registerApOnlyRoutes_();
        void handlePortalRoot_();
        void handleWifiForm_();
        void handleSave_();
    };
}