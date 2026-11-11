%RUN_TESTS Self-checks: octave --eval run_tests
P = arm_params(); n = 0;
for x = 60:10:230
    for y = -100:25:100
        for z = -60:10:160
            [q, ok] = arm_ik(P, [x y z]);
            if ~ok, continue; end
            p = arm_fk(P, q);
            assert(norm(p - [x y z]) < 1e-9, 'FK(IK) mismatch at %g %g %g', x, y, z);
            n = n + 1;
        end
    end
end
assert(n > 500, 'too few reachable points (%d)', n);
[~, okHome] = arm_ik(P, P.home); assert(okHome, 'home pose must be reachable');
[~, okFar] = arm_ik(P, [P.L1 + P.L2 + P.L3 + 20, 0, 0]); assert(~okFar, 'beyond reach must fail');
t = arm_torque(P, arm_ik(P, P.home)); assert(all(t > 0 & t < P.stall), 'home load out of range');
fprintf('run_tests: %d IK/FK round trips passed\n', n);
