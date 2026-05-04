#pragma once

#include <WiFi.h>
#include <vector>

namespace Wifi {
    class Scanner {
        public:
        struct Network {
            String ssid;
            int32_t rssi;
            wifi_auth_mode_t encryptionType;
            int32_t channel;
        };

        static std::vector<Network> networks(bool uniqueSsids = true);
        static int16_t scan();
    };
}