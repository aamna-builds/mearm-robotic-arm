<sub>[← Back to README](../README.md)</sub>

# MATLAB toolbox

The toolbox runs in MATLAB R2019b or later and in GNU Octave 7 or later. It reads [`config/arm.json`](../config/arm.json).

| File | |
|---|---|
| `arm_params.m` | load the config |
| `arm_ik.m`, `arm_fk.m` | inverse and forward kinematics, with joint and linkage limits |
| `arm_torque.m` | static gravity load on the shoulder and forearm servos (uses the assumed masses in the config) |
| `analyze_arm.m` | reachable area and servo load → `figures/workspace_and_load.png` |
| `export_table.m` | IK test points → `output/ik_table.csv`, used by the firmware tests |
| `run_tests.m` | FK(IK(p)) = p over the workspace, plus limit checks |
| `run_all.m` | all of the above |
