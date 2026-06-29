#include <esp_now.h>
#include <WiFi.h>
#include <esp_wifi.h>

// ── Config ──────────────────────────────────────────
uint8_t SLAVE_MAC[] = {0xA0, 0xF2, 0x62, 0xA6, 0x3B, 0x9C};
const uint8_t WIFI_CHANNEL   = 6;
const uint16_t TX_INTERVAL_MS = 50;

// ── Beacon packet struct ─────────────────────────────
struct BeaconPacket {
  uint32_t sequence;
  uint32_t timestamp_ms;
};
BeaconPacket beacon = {0, 0};

// ── Power levels (0.25 dBm units) ───────────────────
const int8_t powerLevels[] = {20, 28, 40, 52, 60, 68, 76, 84};
const uint8_t NUM_LEVELS = 8;

// ── Send callback ────────────────────────────────────
void onPacketSent(const wifi_tx_info_t *info, esp_now_send_status_t status) { }

// ── Power sweep ──────────────────────────────────────
void runPowerSweep() {
  Serial.println("\n=== POWER SWEEP RESULT ===");

  int8_t thresholdPower = powerLevels[NUM_LEVELS - 1];
  bool thresholdFound = false;

  for (uint8_t i = 0; i < NUM_LEVELS; i++) {
    esp_wifi_set_max_tx_power(powerLevels[i]);
    delay(50);

    uint8_t sent = 0;
    for (uint8_t p = 0; p < 10; p++) {
      beacon.sequence++;
      beacon.timestamp_ms = millis();
      esp_err_t r = esp_now_send(SLAVE_MAC, (uint8_t*)&beacon, sizeof(beacon));
      if (r == ESP_OK) sent++;
      delay(TX_INTERVAL_MS);
    }

    bool linkAlive = (sent >= 5);

    Serial.printf("  %.1f dBm: %s (%d/10 packets)\n",
                  powerLevels[i] * 0.25f,
                  linkAlive ? "ALIVE" : "DEAD ",
                  sent);

    if (linkAlive && !thresholdFound) {
      thresholdPower = powerLevels[i];
      thresholdFound = true;
    }
  }

  Serial.printf("  Link threshold: %.1f dBm\n", thresholdPower * 0.25f);
  Serial.println("  Higher threshold = more turbid");
  Serial.println("==========================\n");
}

// ── Setup ────────────────────────────────────────────
void setup() {
  Serial.begin(115200);
  delay(3000);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect();

  esp_wifi_set_promiscuous(true);
  esp_wifi_set_channel(WIFI_CHANNEL, WIFI_SECOND_CHAN_NONE);
  esp_wifi_set_promiscuous(false);

  esp_now_init();
  esp_now_register_send_cb(onPacketSent);

  esp_now_peer_info_t peer = {};
  memcpy(peer.peer_addr, SLAVE_MAC, 6);
  peer.channel = WIFI_CHANNEL;
  peer.encrypt = false;
  esp_now_add_peer(&peer);

  esp_wifi_set_max_tx_power(powerLevels[NUM_LEVELS - 1]); // Start at max
  Serial.println("Master ready — sweep runs every 10 seconds");
}

// ── Loop ─────────────────────────────────────────────
void loop() {
  static unsigned long lastSweep = 0;
  if (millis() - lastSweep > 10000) {
    lastSweep = millis();
    runPowerSweep();
  }
}