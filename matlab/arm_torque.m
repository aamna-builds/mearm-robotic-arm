function tau = arm_torque(P, q)
%ARM_TORQUE Static gravity torque [N m] on the shoulder and forearm servos
%   for joint angles q (deg), using the assumed masses in config/arm.json.
%   Each link's mass acts at its middle; the gripper (with its servo) and the
%   payload act at the gripper tip.
    g = 9.81; m = P.m;
    L1 = P.L1 / 1000; L2 = P.L2 / 1000; L3 = P.L3 / 1000;
    c1 = cosd(q(2)); c2 = cosd(q(3));
    tip = m.gripper_with_servo_kg + m.payload_kg;
    % forearm servo: everything beyond the elbow, horizontal lever arms
    tau_f = g * (m.forearm_kg * L2/2 * c2 + tip * (L2 * c2 + L3));
    % shoulder servo: everything beyond the shoulder
    tau_s = g * (m.upper_arm_kg * L1/2 * c1 + m.forearm_kg * (L1*c1 + L2/2*c2) + tip * (L1*c1 + L2*c2 + L3));
    tau = [abs(tau_s), abs(tau_f)];
end
