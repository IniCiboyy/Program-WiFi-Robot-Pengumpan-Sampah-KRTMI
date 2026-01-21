#include <WiFi.h>
#include <WebServer.h>
#include <math.h>
#include <ESP32Servo.h>
#include "Config.h"

WebServer server(80);

// Hardware instances
Servo servo1;

// Pin Driver BTS7960
// rpwm, lpwm, ren, len
BTS m1 = {32, 33, 25, 26, 0, 1};
BTS m2 = {18,  5,  4,  2, 2, 3};
BTS m3 = {14, 27, 13, 12, 4, 5};

// States
String lastDir = "stop";
int lastSpd = 0;

String liftState = "stop";
bool gripperClosed = false;

// Default Servo
int servoAngle = 90;

bool lastTopHit = false;
bool lastBottomHit = false;

// Move dedup
unsigned long lastMoveMs = 0;
char lastMoveSig[32] = {0};

// Failsafe
unsigned long lastCmdMs = 0;
bool movingActive = false;

// Helper functions
static inline void touchCmd() { lastCmdMs = millis(); }

static inline void sendJson(int code, const String& json) {
  server.sendHeader("Access-Control-Allow-Origin", "*");
  server.sendHeader("Cache-Control", "no-store");
  server.send(code, "application/json", json);
}

static inline int percentToPwm(int pct) {
  pct = constrain(pct, 0, 100);
  return (pct * 255 + 50) / 100;
}

// Remote_Control.ino
void handleStatus();
void handleMove();
void handleStop();
void handleLift();
void handleGripper();
void handleServo();
void handleNotFound();

// Jalan.ino
void btsSetup(BTS &m);
void stopAllMotors();
void driveByDir(const String& dir, int spd);

// Actuators.ino
bool topHit();
bool bottomHit();
void liftStop();
void liftLimitWatchdog();
void gripperOpen();

// Failsafe
void commFailsafeWatchdog();

// Failsafe Watchdog
void commFailsafeWatchdog() {
  if (millis() - lastCmdMs > CMD_TIMEOUT_MS) {
    if (movingActive) {
      stopAllMotors();
      movingActive = false;
    }
    if (liftState != "stop") {
      liftStop();
    }
  }
}

// setup & loop
void setup() {
  delay(150);

  // Servo
  ESP32PWM::allocateTimer(3);
  servo1.setPeriodHertz(50);
  servo1.attach(PIN_SERVO, 500, 2400);
  servo1.write(servoAngle);

  // Motor
  btsSetup(m1);
  btsSetup(m2);
  btsSetup(m3);
  stopAllMotors();

  // Lift
  pinMode(PIN_LIFT_A, OUTPUT);
  pinMode(PIN_LIFT_B, OUTPUT);

  // Limit Switch
  pinMode(PIN_LS_TOP, INPUT_PULLUP);
  pinMode(PIN_LS_BOTTOM, INPUT_PULLUP);
  liftStop();

  // Gripper
  pinMode(PIN_GRIPPER, OUTPUT);
  gripperOpen();

  // WiFi AP
  WiFi.mode(WIFI_AP);
  WiFi.setSleep(false);
  WiFi.setTxPower(WIFI_POWER_19_5dBm);
  WiFi.softAP(SSID, PASS, CHANNEL, HIDDEN, MAX_USER);

  // Routes
  server.on("/status",  HTTP_GET, handleStatus);
  server.on("/move",    HTTP_GET, handleMove);
  server.on("/stop",    HTTP_GET, handleStop);
  server.on("/lift",    HTTP_GET, handleLift);
  server.on("/gripper", HTTP_GET, handleGripper);
  server.on("/servo",   HTTP_GET, handleServo);

  server.onNotFound(handleNotFound);
  server.begin();

  // init failsafe
  lastCmdMs = millis();
  movingActive = false;
}

void loop() {
  server.handleClient();
  liftLimitWatchdog();
  commFailsafeWatchdog();
}