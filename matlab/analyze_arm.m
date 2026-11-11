%ANALYZE_ARM Reachable workspace and servo load for the MeArm.
%
%   Side view (x-z plane, base at 0): every point the gripper tip can reach
%   with the joint and linkage limits in config/arm.json, coloured by the
%   higher of the two static servo loads as a fraction of MG90S stall torque.
%   The load peaks with the arm stretched out level, which is where the
%   build jittered under load.
%
%   Output: figures/workspace_and_load.png and a console summary.

P = arm_params();
[X, Z] = meshgrid(0:2:240, -80:2:200);
L = nan(size(X)); why = cell(size(X));
for k = 1:numel(X)
    [q, ok] = arm_ik(P, [X(k), 0, Z(k)]);
    if ok
        t = arm_torque(P, q);
        L(k) = max(t) / P.stall;
    end
end

f = figure('Visible', 'off', 'Position', [60 60 1150 460]);
subplot(1, 2, 1); hold on;
shown = L; shown(isnan(L)) = -0.05;
imagesc(X(1,:), Z(:,1), shown); axis xy equal tight;
colormap([0.92 0.92 0.92; arm_cmap(63)]); caxis([-0.05 1]);
cb = colorbar; ylabel(cb, 'servo load / stall torque');
% draw the arm at home and stretched out
for target = {P.home, [215 0 10]}
    [q, ok] = arm_ik(P, target{1});
    if ~ok, continue; end
    [tip, el, wr] = arm_fk(P, q);
    pts = [0 0; el(1) el(3); wr(1) wr(3); tip(1) tip(3)];
    plot(pts(:,1), pts(:,2), '-o', 'LineWidth', 2.5, 'Color', [0.15 0.15 0.15], 'MarkerFaceColor', 'w');
end
xlabel('reach x [mm]'); ylabel('height z [mm]');
title('Side view: reachable area and servo load');

subplot(1, 2, 2); hold on; grid on;
reach = 60:2:230; peak = nan(size(reach));
for i = 1:numel(reach)
    [q, ok] = arm_ik(P, [reach(i) 0 0]);
    if ok, peak(i) = max(arm_torque(P, q)) / P.stall; end
end
plot(reach, 100*peak, 'LineWidth', 2);
plot([reach(1) reach(end)], [100 100], '--', 'Color', [0.8 0.2 0.2]);
plot([reach(1) reach(end)], [50 50], ':', 'Color', [0.9 0.6 0.1]);
xlabel('horizontal reach at shoulder height [mm]'); ylabel('load on the busier servo [% of stall]');
legend('static load', 'stall', '50 % (comfortable limit)', 'Location', 'northwest');
title('Load grows with reach');
arm_save(f, 'workspace_and_load');

valid = ~isnan(L);
fprintf('reachable side-view area   : %.0f cm^2\n', sum(valid(:)) * 4 / 100);
fprintf('max horizontal reach (z=0) : %.0f mm\n', max(reach(~isnan(peak))));
fprintf('worst static servo load    : %.0f %% of stall\n', 100 * max(L(:)));
fprintf('load at home pose          : %.0f %% of stall\n', 100 * max(arm_torque(P, arm_ik(P, P.home))) / P.stall);
