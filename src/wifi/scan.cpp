#include "wifi/scan.h"

namespace WifiMgr {
    std::vector<Scanner::Network> Scanner::networks(bool uniqueSsids) {
        std::vector<Network> result;

        WiFi.mode(WIFI_STA);
        WiFi.disconnect(false, false);
        delay(100);

        int count = WiFi.scanNetworks(false, false);

        for (int i = 0; i < count; i++) {
            String ssid = WiFi.SSID(i);

            if (ssid.length() == 0) {
                continue;
            }

            if (uniqueSsids) {
                bool exists = false;
                for (const auto& n : result) {
                    if (n.ssid == ssid) {
                        exists = true;
                        break;
                    }
                }

                if (exists) {
                    continue;
                }
            }

            result.push_back(Network{
                ssid,
                WiFi.RSSI(i),
                WiFi.encryptionType(i),
                WiFi.channel(i)
            });
        }

        WiFi.scanDelete();
        return result;
    }
}