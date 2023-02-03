#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiUdp.h>
#include <AppleMIDI_Debug.h>
#include "Freenove_WS2812_Lib_for_ESP32.h"
#include <AppleMIDI.h>

#define SerialMon Serial
#define LEDS_COUNT 1
#define LEDS_PIN 2
#define CHANNEL 0
#define BTN_GPIO 9

// RGB_LED_BUTTON_G2
int BTN_State = 0;
Freenove_ESP32_WS2812 strip = Freenove_ESP32_WS2812(LEDS_COUNT, LEDS_PIN, CHANNEL, TYPE_GRB);
u8 m_color[5][3] = {
  { 255, 0, 0 }, { 0, 255, 0 }, { 0, 0, 255 }, { 255, 255, 255 }, { 0, 0, 0 }
};
int delayval = 10;

// Wifi
char ssid[] = "HOUSE";             //  your network SSID (name)
char pass[] = "wifiwifiwifi1992";  // your network password (use for WPA, or use as key for WEP)

// Midi
unsigned long t0 = millis();
int8_t isConnected = 0;
byte note = 43;
byte velocity = 55;
byte channel = 1;

APPLEMIDI_CREATE_DEFAULTSESSION_INSTANCE();

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void setup() {
  DBG_SETUP(115200);
  DBG("Booting");

  // WIFI
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    DBG("Establishing connection to WiFi..");
  }
  DBG("Connected to network");
  DBG(F("OK, now make sure you an rtpMIDI session that is Enabled"));
  DBG(F("Add device named Arduino with Host"), WiFi.localIP(), "Port", AppleMIDI.getPort(), "(Name", AppleMIDI.getName(), ")");
  DBG(F("Select and then press the Connect button"));
  DBG(F("Then open a MIDI listener and monitor incoming notes"));
  DBG(F("Listen to incoming MIDI commands"));

  // MIDI
  MIDI.begin();
  AppleMIDI.setHandleConnected([](const APPLEMIDI_NAMESPACE::ssrc_t& ssrc, const char* name) {
    isConnected++;
    DBG(F("Connected to session"), ssrc, name);
  });
  AppleMIDI.setHandleDisconnected([](const APPLEMIDI_NAMESPACE::ssrc_t& ssrc) {
    isConnected--;
    DBG(F("Disconnected"), ssrc);
  });
  MIDI.setHandleNoteOn([](byte channel, byte note, byte velocity) {
    DBG(F("NoteOn"), note);
  });
  MIDI.setHandleNoteOff([](byte channel, byte note, byte velocity) {
    DBG(F("NoteOff"), note);
  });
  DBG(F("Sending NoteOn/Off of note 45, every second"));

  //LED
  pinMode(BTN_GPIO, INPUT);
  strip.begin();
  strip.setBrightness(10);
}

// -----------------------------------------------------------------------------
//
// -----------------------------------------------------------------------------
void loop() {

  BTN_State = digitalRead(BTN_GPIO);
  MIDI.read();

  if (BTN_State == 0) {
    // Midi
   
    if ((isConnected > 0) && (millis() - t0) > 1000) {
      t0 = millis();
      MIDI.sendNoteOn(note, velocity, channel);
      MIDI.sendNoteOff(note, velocity, channel);
    }
    //Led
    for (int j = 0; j < 5; j++) {
      for (int i = 0; i < LEDS_COUNT; i++) {
        strip.setLedColorData(i, m_color[j][0], m_color[j][1], m_color[j][2]);
        strip.show();
        delay(delayval);
      }
      delay(50);
    }
  }
}
