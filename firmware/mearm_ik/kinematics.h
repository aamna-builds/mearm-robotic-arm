// MeArm kinematics (plain C++, no Arduino calls; unit-tested on a PC).
// Same frame and conventions as matlab/arm_ik.m:
//   origin on the base yaw axis at shoulder-pivot height, x forward, y left, z up (mm)
//   q.base     yaw of the arm (deg)
//   q.shoulder upper-arm angle above horizontal (deg)
//   q.forearm  forearm angle above horizontal (deg) - absolute, set by the parallel linkage
#pragma once

struct Point { float x, y, z; };
struct Joints { float base, shoulder, forearm; };

enum class IkResult { Ok, OutOfReach, BaseLimit, ShoulderLimit, ForearmLimit, LinkageLimit };

namespace arm {
IkResult ik(Point p, Joints &q);
Point fk(Joints q);
const char *describe(IkResult r);
// Joint angle -> servo command angle (0..180), using the calibration in config.h
int servo_base(float deg);
int servo_shoulder(float deg);
int servo_forearm(float deg);
}  // namespace arm
