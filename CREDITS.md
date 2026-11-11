# Credits and licences

## The team

The arm was built in July 2025 by **team Byte & Bolt (Group A)** during *SAE × Robonix*, a 15-day hands-on robotics challenge run by the Robotics Club of Ahmedabad University and the university's SAE student chapter.

| | |
|---|---|
| Team | Aamna Khan, Aryan Abhishekh, Khushi Shah, Nikkesh Parekh |
| Mentor | Dev Shah |
| Faculty | Prof. Maryam Kaveshghar |
| Organisers | Robotics Club, Ahmedabad University · SAE Student Chapter, Ahmedabad University |

The design changes, assembly, wiring, Tinkercad simulation, testing, report and photos from the challenge were the team's joint work.

The kinematics toolbox (`matlab/`), the position-control firmware (`firmware/mearm_ik/`), its tests and this repository's documentation were added afterwards by Aamna Khan.

## The MeArm design

The arm's mechanical design is the **MeArm**, an open-source robot arm created by **Benjamin Gray and Jack Howard** at phenoptix. The project later continued as Mime Industries and then MeArm Robotics.

- Project: https://github.com/MeArm/MeArm
- Build guide (v0.4): https://www.instructables.com/Pocket-Sized-Robot-Arm-meArm-V04/
- Licence: the MeArm hardware is released under a Creative Commons **Attribution-ShareAlike** licence.

We cut our parts from the MeArm cutting files, with fit adjustments for 4 mm acrylic. This repository does **not** redistribute the MeArm cutting files; get them from the links above. Any modified cutting files that we publish later will carry the same CC BY-SA licence and this attribution.

## Other references we used while building

- DroneBot Workshop, *Assembling the MeArm Robotic Arm*: https://dronebotworkshop.com/assembling-the-mearm-robotic-arm/
- Arduino `Servo` library: https://www.arduino.cc/reference/en/libraries/servo/
- Tower Pro MG90S datasheet, for the 1.8 kg·cm stall torque used in the load analysis

## This repository

- **Code and documentation** written for this repository (`matlab/`, `firmware/`, `tools/`, `docs/`, the READMEs): MIT License, see [LICENSE](LICENSE). It doesn't reuse code from the MeArm project.
- **Photos and videos** in `media/`: taken by the team during the build.
