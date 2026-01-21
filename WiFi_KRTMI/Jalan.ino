#include "Config.h"

// BTS7960
void btsSetup(BTS &m) {
  pinMode(m.rpwm, OUTPUT);
  pinMode(m.lpwm, OUTPUT);
  pinMode(m.ren, OUTPUT);
  pinMode(m.len, OUTPUT);

  digitalWrite(m.ren, HIGH);
  digitalWrite(m.len, HIGH);

  ledcAttach(m.rpwm, PWM_FREQ, PWM_RES);
  ledcAttach(m.lpwm, PWM_FREQ, PWM_RES);

  ledcWrite(m.rpwm, 0);
  ledcWrite(m.lpwm, 0);
}

// btsSet dipakai internal modul jalan
static inline void btsSet(BTS &m, int pwm) {
  pwm = constrain(pwm, -255, 255);

  digitalWrite(m.ren, HIGH);
  digitalWrite(m.len, HIGH);

  if (pwm > 0) {
    ledcWrite(m.rpwm, pwm);
    ledcWrite(m.lpwm, 0);
  } else if (pwm < 0) {
    ledcWrite(m.rpwm, 0);
    ledcWrite(m.lpwm, -pwm);
  } else {
    ledcWrite(m.rpwm, 0);
    ledcWrite(m.lpwm, 0);
  }
}

void stopAllMotors() {
  btsSet(m1, 0);
  btsSet(m2, 0);
  btsSet(m3, 0);
  lastDir = "stop";
  lastSpd = 0;
}

// Omni Drive (Kiwi)
static void driveOmni(float vx, float vy, float w, int spd) {
  const float SQ3_2 = 0.8660254f;

  float w1 = (-0.5f * vx + SQ3_2 * vy + w);
  float w2 = (-0.5f * vx - SQ3_2 * vy + w);
  float w3 = ( 1.0f * vx + w);

  float maxv = max(1.0f, max(fabs(w1), max(fabs(w2), fabs(w3))));
  w1 /= maxv; w2 /= maxv; w3 /= maxv;

  int p1 = (int)round(w1 * spd);
  int p2 = (int)round(w2 * spd);
  int p3 = (int)round(w3 * spd);

  btsSet(m1, p1);
  btsSet(m2, p2);
  btsSet(m3, p3);
}

void driveByDir(const String& dir, int spd) {
  float vx = 0, vy = 0, w = 0;

  if (dir == "stop") { stopAllMotors(); return; }
  else if (dir == "maju") vy = -1;
  else if (dir == "mundur") vy = 1;
  else if (dir == "kiri") vx = -1;
  else if (dir == "kanan") vx = 1;
  else if (dir == "kiri_atas") { vx = -1; vy = -1; }
  else if (dir == "kanan_atas") { vx = 1; vy = -1; }
  else if (dir == "kiri_bawah") { vx = -1; vy = 1; }
  else if (dir == "kanan_bawah") { vx = 1; vy = 1; }
  else { stopAllMotors(); return; }

  driveOmni(vx, vy, w, spd);
}
