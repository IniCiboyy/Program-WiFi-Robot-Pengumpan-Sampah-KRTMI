#ifndef CONFIG_H
#define CONFIG_H

#include <Arduino.h>

// WiFi Access Point
static const char* SSID     = "Robot ITNY";     // Nama Wifi
static const char* PASS     = "RobotITNY";      // Password Wifi
static const int   CHANNEL  = 6;
static const int   MAX_USER = 1;                // Maximal yang bisa connect
static const bool  HIDDEN   = false;

// BTS7960 PWM
static const uint32_t PWM_FREQ = 20000;
static const uint8_t  PWM_RES  = 8;

// Struktur BTS + mapping channel LEDC
struct BTS {
  int rpwm;
  int lpwm;
  int ren;
  int len;
  uint8_t ch_rpwm;
  uint8_t ch_lpwm;
};

// Lift/Motor Gearbox (Relay)
static const int PIN_LIFT_A = 19;
static const int PIN_LIFT_B = 21;

// Limit switch (NC)
static const int PIN_LS_TOP    = 22;
static const int PIN_LS_BOTTOM = 23;

static const int LIMIT_ACTIVE_LEVEL = HIGH;
static const int RELAY_ACTIVE       = LOW;
static const int RELAY_INACTIVE     = HIGH;

static const uint16_t LIFT_DEADTIME_MS = 80;

// Gripper/Pneumatic Solenoid Valve (Relay)
static const int PIN_GRIPPER = 15;

static const bool GRIPPER_ACTIVE_LOW = true;
static const int  GRIPPER_ACTIVE_LEVEL   = GRIPPER_ACTIVE_LOW ? LOW  : HIGH;
static const int  GRIPPER_INACTIVE_LEVEL = GRIPPER_ACTIVE_LOW ? HIGH : LOW;

// Servo
static const int PIN_SERVO = 16;

// Move dedup
static const unsigned long MOVE_DEDUP_MS = 25;

// Failsafe
static const unsigned long CMD_TIMEOUT_MS = 900;

#endif