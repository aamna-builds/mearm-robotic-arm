<sub>[← Back to README](../README.md)</sub>

# Firmware

`mearm_ik/` is an Arduino Uno sketch that controls the arm by gripper position. It needs only the standard `Servo` library.

1. **Measure:** check the link lengths and joint limits on your arm, and put them in `config/arm.json`.
2. **Generate the header:** run `python3 tools/gen_firmware_config.py`.
3. **Calibrate:** for each servo, find the command that puts its joint at the reference pose:
   - base: pointing straight ahead
   - upper arm: vertical
   - forearm: horizontal

   Write these in as `servo_center_deg`, and flip `sign` for any servo that turns the wrong way.
4. **Flash and talk to it:** upload, open the Serial Monitor at 115200 baud, and send commands:

| Command | |
|---|---|
| `P x y z` | move the gripper tip to (x, y, z) mm in a straight line |
| `J b s f` | move to joint angles (deg) |
| `G 1` / `G 0` | close / open the gripper |
| `H` | home |
| `D` | pick-and-place demo |
| `?` | position and joint angles |

Power the servos from a separate 5 V supply with its ground tied to the Uno's ground.
