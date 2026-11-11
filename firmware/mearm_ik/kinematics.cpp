#include "kinematics.h"

#include <math.h>

#include "config.h"

namespace {
const float kDeg = 57.2957795f;
inline float clampf(float v, float lo, float hi) { return v < lo ? lo : (v > hi ? hi : v); }
int to_servo(float deg, float center, float sign) {
  return (int)lroundf(clampf(center + sign * deg, 0.0f, 180.0f));
}
}  // namespace

namespace arm {

IkResult ik(Point p, Joints &q) {
  const float psi = atan2f(p.y, p.x) * kDeg;
  const float rw = sqrtf(p.x * p.x + p.y * p.y) - ARM_L3;   // wrist distance from the base axis
  const float zw = p.z;
  const float d = sqrtf(rw * rw + zw * zw);
  if (d > ARM_L1 + ARM_L2 || d < fabsf(ARM_L1 - ARM_L2) || d < 1e-6f) return IkResult::OutOfReach;
  const float a = atan2f(zw, rw) * kDeg;
  const float b = acosf(clampf((ARM_L1 * ARM_L1 + d * d - ARM_L2 * ARM_L2) / (2 * ARM_L1 * d), -1, 1)) * kDeg;
  const float gam = acosf(clampf((ARM_L1 * ARM_L1 + ARM_L2 * ARM_L2 - d * d) / (2 * ARM_L1 * ARM_L2), -1, 1)) * kDeg;
  q.base = psi;
  q.shoulder = a + b;                       // elbow-up solution
  q.forearm = q.shoulder - (180.0f - gam);
  if (q.base < ARM_BASE_MIN || q.base > ARM_BASE_MAX) return IkResult::BaseLimit;
  if (q.shoulder < ARM_SHOULDER_MIN || q.shoulder > ARM_SHOULDER_MAX) return IkResult::ShoulderLimit;
  if (q.forearm < ARM_ELBOW_MIN || q.forearm > ARM_ELBOW_MAX) return IkResult::ForearmLimit;
  if (gam < ARM_INTERIOR_MIN || gam > ARM_INTERIOR_MAX) return IkResult::LinkageLimit;
  return IkResult::Ok;
}

Point fk(Joints q) {
  const float r = ARM_L1 * cosf(q.shoulder / kDeg) + ARM_L2 * cosf(q.forearm / kDeg) + ARM_L3;
  const float z = ARM_L1 * sinf(q.shoulder / kDeg) + ARM_L2 * sinf(q.forearm / kDeg);
  return {r * cosf(q.base / kDeg), r * sinf(q.base / kDeg), z};
}

const char *describe(IkResult r) {
  switch (r) {
    case IkResult::Ok: return "ok";
    case IkResult::OutOfReach: return "out of reach";
    case IkResult::BaseLimit: return "base limit";
    case IkResult::ShoulderLimit: return "shoulder limit";
    case IkResult::ForearmLimit: return "forearm limit";
    case IkResult::LinkageLimit: return "linkage limit";
  }
  return "?";
}

int servo_base(float deg) { return to_servo(deg, ARM_BASE_CENTER, ARM_BASE_SIGN); }
int servo_shoulder(float deg) { return to_servo(deg - 90.0f, ARM_SHOULDER_CENTER, ARM_SHOULDER_SIGN); }
int servo_forearm(float deg) { return to_servo(deg, ARM_ELBOW_CENTER, ARM_ELBOW_SIGN); }

}  // namespace arm
