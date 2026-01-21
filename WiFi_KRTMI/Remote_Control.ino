#include "Config.h"

// HTTP Handlers
void handleStatus() {
  String json;
  json.reserve(240);

  json = "{";
  json += "\"ok\":true,";
  json += "\"ip\":\"192.168.4.1\",";
  json += "\"lastDir\":\"" + lastDir + "\",";
  json += "\"lastSpd\":" + String(lastSpd) + ",";
  json += "\"lift\":\"" + liftState + "\",";
  json += "\"ls_top\":" + String(topHit() ? "true" : "false") + ",";
  json += "\"ls_bottom\":" + String(bottomHit() ? "true" : "false") + ",";
  json += "\"gripper\":\"" + String(gripperClosed ? "close" : "open") + "\",";
  json += "\"servoAngle\":" + String(servoAngle);
  json += "}";

  sendJson(200, json);
}

void handleMove() {
  if (!server.hasArg("dir") || !server.hasArg("spd")) {
    sendJson(400, "{\"ok\":false,\"err\":\"missing dir/spd\"}");
    return;
  }

  touchCmd();
  movingActive = true;

  String dir = server.arg("dir");

  int spdPct = constrain(server.arg("spd").toInt(), 0, 100);
  int spdPwm = percentToPwm(spdPct);

  char sig[32];
  snprintf(sig, sizeof(sig), "%s|%d", dir.c_str(), spdPct);

  unsigned long now = millis();

  if ((now - lastMoveMs) < MOVE_DEDUP_MS && strcmp(sig, lastMoveSig) == 0) {
    sendJson(200, "{\"ok\":true,\"cmd\":\"move\",\"dedup\":true}");
    return;
  }

  strncpy(lastMoveSig, sig, sizeof(lastMoveSig) - 1);
  lastMoveSig[sizeof(lastMoveSig) - 1] = '\0';
  lastMoveMs = now;

  lastDir = dir;
  lastSpd = spdPct;

  driveByDir(dir, spdPwm);

  sendJson(200, "{\"ok\":true,\"cmd\":\"move\"}");
}

void handleStop() {
  touchCmd();
  movingActive = false;

  stopAllMotors();

  sendJson(200, "{\"ok\":true,\"cmd\":\"stop\"}");
}

void handleGripper() {
  if (!server.hasArg("pos")) {
    sendJson(400, "{\"ok\":false,\"err\":\"missing pos\"}");
    return;
  }

  touchCmd();
  String pos = server.arg("pos");
  pos.toLowerCase();

  if (pos == "open") gripperOpen();
  else if (pos == "close") gripperClose();
  else if (pos == "toggle") gripperToggle();
  else {
    sendJson(400, "{\"ok\":false,\"err\":\"pos must be open/close/toggle\"}");
    return;
  }

  sendJson(200, "{\"ok\":true,\"cmd\":\"gripper\"}");
}

void handleLift() {
  if (!server.hasArg("dir")) {
    sendJson(400, "{\"ok\":false,\"err\":\"missing dir\"}");
    return;
  }

  touchCmd();
  String dir = server.arg("dir");
  dir.toLowerCase();

  if (dir == "up") liftUp();
  else if (dir == "down") liftDown();
  else if (dir == "stop") liftStop();
  else {
    sendJson(400, "{\"ok\":false,\"err\":\"dir must be up/down/stop\"}");
    return;
  }

  sendJson(200, "{\"ok\":true,\"cmd\":\"lift\"}");
}

void handleServo() {
  if (!server.hasArg("pos")) {
    sendJson(400, "{\"ok\":false,\"err\":\"missing pos\"}");
    return;
  }

  touchCmd();
  int pos = server.arg("pos").toInt();
  int target = angleForPos(pos);

  if (target < 0) {
    sendJson(400, "{\"ok\":false,\"err\":\"pos must be 1..3\"}");
    return;
  }

  target = constrain(target, 0, 180);

  servoAngle = target;
  servo1.write(servoAngle);

  String json;
  json.reserve(90);

  json = "{\"ok\":true,\"cmd\":\"servo\",\"pos\":";
  json += String(pos);
  json += ",\"angle\":";
  json += String(servoAngle);
  json += "}";

  sendJson(200, json);
}

void handleNotFound() {
  server.send(404, "text/plain", "Not Found");
}