function [p, elbow, wrist] = arm_fk(P, q)
%ARM_FK Gripper-tip position for joint angles q = [base shoulder forearm] (deg).
    r_e = P.L1 * cosd(q(2));            z_e = P.L1 * sind(q(2));
    r_w = r_e + P.L2 * cosd(q(3));      z_w = z_e + P.L2 * sind(q(3));
    r_t = r_w + P.L3;
    c = cosd(q(1)); s = sind(q(1));
    p = [r_t * c, r_t * s, z_w];
    elbow = [r_e * c, r_e * s, z_e];
    wrist = [r_w * c, r_w * s, z_w];
end
