#include "Config.h"

// Limit Switch
bool topHit() {
  return digitalRead(PIN_LS_TOP) == LIMIT_ACTIVE_LEVEL;
}

bool bottomHit() {
  return digitalRead(PIN_LS_BOTTOM) == LIMIT_ACTIVE_LEVEL;
}

// Lift
static inline void liftAllOff() {
  digitalWrite(PIN_LIFT_A, RELAY_INACTIVE);
  digitalWrite(PIN_LIFT_B, RELAY_INACTIVE);
}

void liftStop() {
  liftAllOff();
  liftState = "stop";
}

void liftUp() {
  if (topHit()) { liftStop(); return; }
  liftAllOff();
  delay(LIFT_DEADTIME_MS);
  digitalWrite(PIN_LIFT_A, RELAY_ACTIVE);
  digitalWrite(PIN_LIFT_B, RELAY_INACTIVE);
  liftState = "up";
}

void liftDown() {
  if (bottomHit()) { liftStop(); return; }
  liftAllOff();
  delay(LIFT_DEADTIME_MS);
  digitalWrite(PIN_LIFT_A, RELAY_INACTIVE);
  digitalWrite(PIN_LIFT_B, RELAY_ACTIVE);
  liftState = "down";
}

void liftLimitWatchdog() {
  bool tHit = topHit();
  bool bHit = bottomHit();

  lastTopHit = tHit;
  lastBottomHit = bHit;

  if (liftState == "up" && tHit) liftStop();
  if (liftState == "down" && bHit) liftStop();
}

// Gripper
void gripperOpen() {
  digitalWrite(PIN_GRIPPER, GRIPPER_INACTIVE_LEVEL);
  gripperClosed = false;
}

void gripperClose() {
  digitalWrite(PIN_GRIPPER, GRIPPER_ACTIVE_LEVEL);
  gripperClosed = true;
}

void gripperToggle() {
  if (gripperClosed) gripperOpen();
  else gripperClose();
}

// Servo
int angleForPos(int pos) {
  switch (pos) {
    case 1: return 120;
    case 2: return 90;
    case 3: return 45;
    default: return -1;
  }
}