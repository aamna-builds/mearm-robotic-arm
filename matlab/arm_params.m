function P = arm_params(cfgFile)
%ARM_PARAMS Load the arm description from config/arm.json.
%   Frame: origin on the base yaw axis at the height of the shoulder pivot,
%   x forward, y left, z up. Lengths in mm, angles in degrees.
    if nargin < 1
        here = fileparts(mfilename('fullpath'));
        cfgFile = fullfile(here, '..', 'config', 'arm.json');
    end
    C = jsondecode(fileread(cfgFile));
    P.L1 = C.geometry.upper_arm_mm;
    P.L2 = C.geometry.forearm_mm;
    P.L3 = C.geometry.gripper_offset_mm;
    J = C.joints;
    P.base  = [J.base.min_deg, J.base.max_deg];
    P.shoulder = [J.shoulder.min_deg, J.shoulder.max_deg];
    P.elbow = [J.elbow.min_deg, J.elbow.max_deg];
    P.interior = [C.linkage.min_elbow_interior_deg, C.linkage.max_elbow_interior_deg];
    P.home = C.motion.home_mm(:)';
    P.stall = C.servo.stall_torque_Nm;
    P.m = C.loads_assumed;
    P.raw = C;
end
