// MeArm controller with inverse kinematics (Arduino Uno + 4 x MG90S).
//
// Instead of hard-coded servo angles, you give the gripper a position in
// millimetres and the sketch works out the joint angles, checks them against
// the joint and linkage limits, and moves there in a straight line at a set
// speed so the arm does not jerk.
//
// Serial, 115200 baud, one command per line; every command answers one line:
//   P x y z     move the gripper tip to (x, y, z) mm      -> ok | err <reason>
//   J b s f     move to joint angles in degrees            -> ok | err <reason>
//   G 1 / G 0   close / open the gripper                   -> ok
//   H           go to the home pose                        -> ok
//   D           run the pick-and-place demo                -> ok
//   ?           report position and joint angles
//
// Power the servos from a separate 5 V supply with its ground tied to the
// Arduino's ground; four servos on the Uno's 5 V pin will reset the board.

#include <Servo.h>

#include "config.h"
#include "kinematics.h"

namespace {

Servo s_base, s_shoulder, s_forearm, s_gripper;
Point g_pos = {ARM_HOME_X, ARM_HOME_Y, ARM_HOME_Z};
Joints g_q = {0, 90, 0};
char g_line[48];
uint8_t g_len = 0;

void write_joints(const Joints &q) {
  s_base.write(arm::servo_base(q.base));
  s_shoulder.write(arm::servo_shoulder(q.shoulder));
  s_forearm.write(arm::servo_forearm(q.forearm));
  g_q = q;
}

void gripper(bool closed) {
  s_gripper.write(closed ? ARM_GRIPPER_CLOSED : ARM_GRIPPER_OPEN);
  delay(ARM_SETTLE_MS);
}

// Straight-line move with a smooth (cosine) speed profile. The whole path is
// checked first; if any point is not reachable the arm does not move at all.
IkResult move_to(Point target) {
  const float dx = target.x - g_pos.x, dy = target.y - g_pos.y, dz = target.z - g_pos.z;
  const float len = sqrtf(dx * dx + dy * dy + dz * dz);
  const int n = max(1, (int)ceilf(len / ARM_STEP_MM));
  Joints q;
  for (int i = 0; i <= n; ++i) {
    const float t = (float)i / n;
    const IkResult r = arm::ik({g_pos.x + t * dx, g_pos.y + t * dy, g_pos.z + t * dz}, q);
    if (r != IkResult::Ok) return r;
  }
  const float duration_ms = 1000.0f * len / ARM_SPEED_MM_S;
  const unsigned long t0 = millis();
  for (;;) {
    const float u = min(1.0f, (millis() - t0) / max(duration_ms, 1.0f));
    const float s = 0.5f - 0.5f * cosf(u * PI);            // eases in and out
    arm::ik({g_pos.x + s * dx, g_pos.y + s * dy, g_pos.z + s * dz}, q);
    write_joints(q);
    if (u >= 1.0f) break;
    delay(15);                                              // servos update at ~50 Hz anyway
  }
  g_pos = target;
  return IkResult::Ok;
}

bool parse3(const char *s, float &a, float &b, float &c) {
  char *e;
  a = strtod(s, &e); if (e == s) return false; s = e;
  b = strtod(s, &e); if (e == s) return false; s = e;
  c = strtod(s, &e); if (e == s) return false;
  while (*e == ' ' || *e == '\r') ++e;
  return *e == '\0';
}

void reply(IkResult r) {
  if (r == IkResult::Ok) Serial.println(F("ok"));
  else { Serial.print(F("err ")); Serial.println(arm::describe(r)); }
}

void demo() {
  // pick a light object on the right, place it on the left
  const Point above_pick = {150, -60, 50}, pick = {150, -60, -10};
  const Point above_place = {150, 60, 50}, place = {150, 60, -10};
  gripper(false);
  move_to(above_pick); move_to(pick); gripper(true); move_to(above_pick);
  move_to(above_place); move_to(place); gripper(false); move_to(above_place);
  move_to({ARM_HOME_X, ARM_HOME_Y, ARM_HOME_Z});
}

void handle(char *line) {
  while (*line == ' ') ++line;
  const char op = toupper(*line);
  float a, b, c;
  switch (op) {
    case '\0': return;
    case 'P':
      if (!parse3(line + 1, a, b, c)) { Serial.println(F("err syntax")); return; }
      reply(move_to({a, b, c}));
      return;
    case 'J': {
      if (!parse3(line + 1, a, b, c)) { Serial.println(F("err syntax")); return; }
      Point p = arm::fk({a, b, c});
      Joints q;
      const IkResult r = arm::ik(p, q);   // re-check limits for the requested joints
      if (r != IkResult::Ok) { reply(r); return; }
      reply(move_to(p));
      return;
    }
    case 'G': gripper(atoi(line + 1) != 0); Serial.println(F("ok")); return;
    case 'H': reply(move_to({ARM_HOME_X, ARM_HOME_Y, ARM_HOME_Z})); return;
    case 'D': demo(); Serial.println(F("ok")); return;
    case '?':
      Serial.print(F("pos ")); Serial.print(g_pos.x, 1); Serial.print(' '); Serial.print(g_pos.y, 1);
      Serial.print(' '); Serial.print(g_pos.z, 1);
      Serial.print(F(" joints ")); Serial.print(g_q.base, 1); Serial.print(' ');
      Serial.print(g_q.shoulder, 1); Serial.print(' '); Serial.println(g_q.forearm, 1);
      return;
    default: Serial.println(F("err unknown command"));
  }
}

}  // namespace

void setup() {
  Serial.begin(115200);
  Joints q;
  arm::ik(g_pos, q);
  write_joints(q);                       // write before attach: no jump on power-up
  s_base.attach(ARM_BASE_PIN);
  s_shoulder.attach(ARM_SHOULDER_PIN);
  s_forearm.attach(ARM_ELBOW_PIN);
  s_gripper.attach(ARM_GRIPPER_PIN);
  gripper(false);
  Serial.println(F("mearm ready"));
}

void loop() {
  while (Serial.available() > 0) {
    const char ch = (char)Serial.read();
    if (ch == '\n') { g_line[g_len] = '\0'; handle(g_line); g_len = 0; }
    else if (g_len < sizeof(g_line) - 1) g_line[g_len++] = ch;
  }
}
