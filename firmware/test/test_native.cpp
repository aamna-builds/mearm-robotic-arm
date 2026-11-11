// PC tests for the MeArm kinematics: g++ -std=c++17 test_native.cpp ../mearm_ik/kinematics.cpp
#include <cmath>
#include <cstdio>
#include <fstream>
#include <sstream>
#include <string>

#include "../mearm_ik/config.h"
#include "../mearm_ik/kinematics.h"

static int pass = 0, fail = 0;
#define CHECK(c, ...) do { if (c) ++pass; else { ++fail; std::printf("FAIL: " __VA_ARGS__); std::printf("\n"); } } while (0)

int main(int argc, char **argv) {
  // 1. round trip over a grid
  int n = 0;
  for (float x = 60; x <= 230; x += 10)
    for (float y = -100; y <= 100; y += 25)
      for (float z = -60; z <= 160; z += 10) {
        Joints q;
        if (arm::ik({x, y, z}, q) != IkResult::Ok) continue;
        Point p = arm::fk(q);
        CHECK(std::hypot(std::hypot(p.x - x, p.y - y), p.z - z) < 0.01f, "round trip %g %g %g", x, y, z);
        ++n;
      }
  CHECK(n > 500, "only %d reachable points", n);

  // 2. agreement with the MATLAB table
  const char *path = argc > 1 ? argv[1] : "matlab/output/ik_table.csv";
  std::ifstream in(path);
  if (!in) { std::printf("skip: %s not found\n", path); }
  std::string line; std::getline(in, line);
  int rows = 0;
  while (std::getline(in, line)) {
    float v[6]; char c;
    std::stringstream ss(line);
    ss >> v[0] >> c >> v[1] >> c >> v[2] >> c >> v[3] >> c >> v[4] >> c >> v[5];
    Joints q;
    CHECK(arm::ik({v[0], v[1], v[2]}, q) == IkResult::Ok, "reachable %g %g %g", v[0], v[1], v[2]);
    CHECK(std::fabs(q.base - v[3]) < 0.01f && std::fabs(q.shoulder - v[4]) < 0.01f && std::fabs(q.forearm - v[5]) < 0.01f,
          "angles at %g %g %g", v[0], v[1], v[2]);
    ++rows;
  }
  CHECK(rows > 100 || !in, "table rows %d", rows);

  // 3. limits and home
  Joints q;
  CHECK(arm::ik({ARM_HOME_X, ARM_HOME_Y, ARM_HOME_Z}, q) == IkResult::Ok, "home reachable");
  CHECK(arm::ik({400, 0, 0}, q) == IkResult::OutOfReach, "far point rejected");
  CHECK(arm::ik({-150, 0, 0}, q) == IkResult::BaseLimit, "behind the base rejected");
  for (float a = -200; a <= 200; a += 7) {
    int s = arm::servo_base(a);
    CHECK(s >= 0 && s <= 180, "servo command clamped");
  }
  std::printf("mearm native tests: %d passed, %d failed\n", pass, fail);
  return fail ? 1 : 0;
}
