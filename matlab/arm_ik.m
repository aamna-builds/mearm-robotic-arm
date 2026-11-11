function [q, ok, why] = arm_ik(P, p)
%ARM_IK Joint angles for gripper-tip position p = [x y z] (mm).
%   q = [base, shoulder, forearm] in degrees:
%     base      yaw of the whole arm about the vertical axis
%     shoulder  upper-arm angle above the horizontal
%     forearm   forearm angle above the horizontal (absolute: the MeArm's
%               parallel linkage drives it directly and keeps the gripper level)
%   ok is false if the point is out of reach or breaks a joint/linkage limit;
%   why names the first limit that failed.
    x = p(1); y = p(2); z = p(3);
    q = nan(1, 3); ok = false; why = '';
    psi = atan2d(y, x);
    rw = hypot(x, y) - P.L3;           % wrist, horizontal distance from the base axis
    zw = z;
    d = hypot(rw, zw);
    if d > P.L1 + P.L2 || d < abs(P.L1 - P.L2) || d < 1e-9
        why = 'out of reach'; return
    end
    a = atan2d(zw, rw);
    b = acosd((P.L1^2 + d^2 - P.L2^2) / (2 * P.L1 * d));
    gam = acosd((P.L1^2 + P.L2^2 - d^2) / (2 * P.L1 * P.L2));   % elbow interior angle
    q1 = a + b;                         % elbow-up solution
    q2 = q1 - (180 - gam);
    q = [psi, q1, q2];
    if psi < P.base(1) || psi > P.base(2), why = 'base limit'; return; end
    if q1 < P.shoulder(1) || q1 > P.shoulder(2), why = 'shoulder limit'; return; end
    if q2 < P.elbow(1) || q2 > P.elbow(2), why = 'forearm limit'; return; end
    if gam < P.interior(1) || gam > P.interior(2), why = 'linkage limit'; return; end
    ok = true;
end
