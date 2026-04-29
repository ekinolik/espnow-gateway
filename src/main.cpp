#include "app_config.h"
#include "boot_state.h"
#include "espnow_receiver.h"

#include <Arduino.h>

void markSeen(const char* deviceID, uint32_t sequence);
bool isDuplicate(const char* deviceID, uint32_t sequence);

EspNowReceiver espNowReceiver;

struct DeviceSequenceState {
    String deviceID;
    uint32_t lastSequence;
    bool used = false;
};

static constexpr size_t MAX_DEVICES = 16;
DeviceSequenceState g_deviceSeq[MAX_DEVICES];

static void printPacket(const EspNowReceiver::ReceivedMessage& rx);

void setup() {
    Serial.begin(115200);
    delay(2000);

    BootState::begin();
    BootState::printBootSummary(Serial);

    Serial.println("Boot Complete");
    Serial.println("Starting Bridge Setup...");

    if (!espNowReceiver.begin()) {
        Serial.println("[MAIN] ESPNOW Receiver init failed");
        return;
    }
    Serial.println("[MAIN] ESPNOW Receiver init OK");
}

void loop() {
    espNowReceiver.update();

    EspNowReceiver::ReceivedMessage rx;

    while (espNowReceiver.dequeueMessage(rx)) {
        bool accepted = false;
        bool pktWritten = false;
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

        if (!espNowReceiver.sendAck(rx.mac, rx.msg.deviceID, rx.msg.sequence, accepted)) {
            Serial.println("[MAIN] Failed to send ACK");
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