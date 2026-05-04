#include "app_config.h"
#include "boot_state.h"
#include "espnow_receiver.h"
#include "wifi/connector.h"
#include "wifi/portal.h"

#include <Arduino.h>
#include <WiFi.h>
#include <esp_wifi.h>

void markSeen(const char* deviceID, uint32_t sequence);
bool isDuplicate(const char* deviceID, uint32_t sequence);

EspNowReceiver espNowReceiver;

struct DeviceSequenceState {
    String deviceID;
    uint32_t lastSequence;
    bool used = false;
};

Wifi::Portal::Config cfg;
Wifi::Portal portal(cfg);
bool apMode = false;

static constexpr size_t MAX_DEVICES = 16;
DeviceSequenceState g_deviceSeq[MAX_DEVICES];

static void printPacket(const EspNowReceiver::ReceivedMessage& rx);
static void loadPortal();

void quietUnusedHeaderPins() {
  const gpio_num_t pins[] = {
    GPIO_NUM_0,   // D0 / A0
    GPIO_NUM_1,   // D1 / A1
    GPIO_NUM_2,   // D2 / A2
    GPIO_NUM_21,  // D3 / A3
    GPIO_NUM_22,  // D4 / SDA
    GPIO_NUM_23,  // D5 / SCL
    GPIO_NUM_16,  // D6 / TX
    GPIO_NUM_17,  // D7 / RX
    GPIO_NUM_19,  // D8 / SCK
    GPIO_NUM_20,  // D9 / MISO
    GPIO_NUM_18,  // D10 / MOSI
    GPIO_NUM_15   // D15 / LED
  };


  for (gpio_num_t pin : pins) {
    gpio_reset_pin(pin);
    gpio_set_direction(pin, GPIO_MODE_OUTPUT);
    gpio_set_level(pin, 0);
  }

  pinMode(15, OUTPUT); // LED
  digitalWrite(15, LOW); 
}

void setup() {
    quietUnusedHeaderPins();

    pinMode(3, OUTPUT);
    digitalWrite(3, LOW);
    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH); // HIGH == external Antenna

    Serial.begin(115200);
    delay(2000);

    BootState::begin();
    BootState::printBootSummary(Serial);

    Serial.println("Boot Complete");
    Serial.println("Starting Bridge Setup...");

    Wifi::Connector wifiConnector;
    wifiConnector.begin(!wifiConnector.connected());
    if (!wifiConnector.connect()) {
        Serial.println("[MAIN] WIFI init failed, will load portal");
        loadPortal();
        apMode = true;

        return;
    }

    Serial.println("[MAIN] WIFI init OK");

    if (!espNowReceiver.begin(!wifiConnector.connected())) {
        Serial.println("[MAIN] ESPNOW Receiver init failed");
        return;
    }
    Serial.println("[MAIN] ESPNOW Receiver init OK");
}

void loop() {
    if (apMode) {
        portal.loop();
        return;
    }

    espNowReceiver.update();

    EspNowReceiver::ReceivedMessage rx;

    while (espNowReceiver.dequeueMessage(rx)) {
        bool accepted = false;
        bool duplicatePkt = isDuplicate(rx.msg.deviceID, rx.msg.sequence);

        if (duplicatePkt) {
            Serial.printf("[MAIN] Duplicate packet from %s seq=%lu\n",
                rx.msg.deviceID, (unsigned long)rx.msg.sequence
            );
            accepted = true; // ACK duplicate but do not process again
        } else {
            printPacket(rx);
            markSeen(rx.msg.deviceID, rx.msg.sequence);
            accepted = true;
        }

        if (!espNowReceiver.sendAck(rx.mac, rx.msg.deviceID, rx.msg.sequence, accepted)) {
            Serial.println("[MAIN] Failed to send ACK");
        }

        if (!accepted) {
            Serial.println("[MAIN] Packet rejected, NACK sent");
        }
    }
}

bool isDuplicate(const char* deviceID, uint32_t sequence) {
    for (size_t i = 0; i < MAX_DEVICES; i++) {
        if (!g_deviceSeq[i].used) continue;
        if (g_deviceSeq[i].deviceID == deviceID) {
            return sequence <= g_deviceSeq[i].lastSequence;
        }
    }

    return false;
}

void markSeen(const char* deviceID, uint32_t sequence) {
    for (size_t i = 0; i < MAX_DEVICES; i++) {
        if (g_deviceSeq[i].used && g_deviceSeq[i].deviceID == deviceID) {
            g_deviceSeq[i].lastSequence = sequence;

            return;
        }
    }

    for (size_t i = 0; i < MAX_DEVICES; i++) {
        if (!g_deviceSeq[i].used) {
            g_deviceSeq[i].used = true;
            g_deviceSeq[i].deviceID = deviceID;
            g_deviceSeq[i].lastSequence = sequence;

            return;
        }
    }
}

static void printPacket(const EspNowReceiver::ReceivedMessage& rx) {
    Serial.println("----- ESPNOW RX ------");
    Serial.printf("From MAC: %02X:%02X:%02X:%02X:%02X:%02X\n",
        rx.mac[0], rx.mac[1], rx.mac[2], rx.mac[3], rx.mac[4], rx.mac[5]
    );
    Serial.printf("Device ID: %s\n", rx.msg.deviceID);
    Serial.printf("Sequence : %lu\n", (unsigned long)rx.msg.sequence);
    Serial.printf("Timestamp: %lu\n", (unsigned long)rx.msg.timestampMs);
    Serial.printf("RSSI     : %d dBm\n", rx.rssi);
    Serial.printf("Payload:   %s\n", rx.msg.jsonLine);
}

static void loadPortal() {
    portal.begin();
}