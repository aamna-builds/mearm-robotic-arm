%EXPORT_TABLE Joint angles for a grid of test points; the firmware tests
%   compare their own inverse kinematics against this file.
P = arm_params();
out = fullfile(fileparts(mfilename('fullpath')), 'output');
if ~exist(out, 'dir'), mkdir(out); end
fid = fopen(fullfile(out, 'ik_table.csv'), 'w');
fprintf(fid, 'x_mm,y_mm,z_mm,base_deg,shoulder_deg,forearm_deg\n');
n = 0;
for x = 80:20:220
    for y = -80:40:80
        for z = -40:20:120
            [q, ok] = arm_ik(P, [x y z]);
            if ok
                fprintf(fid, '%g,%g,%g,%.4f,%.4f,%.4f\n', x, y, z, q); n = n + 1;
            end
        end
    end
end
fclose(fid);
fprintf('wrote output/ik_table.csv (%d reachable points)\n', n);
