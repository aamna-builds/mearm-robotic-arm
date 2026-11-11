function arm_save(fig, name)
%ARM_SAVE Save a figure to matlab/figures/<name>.png and close it.
    out = fullfile(fileparts(mfilename('fullpath')), 'figures');
    if ~exist(out, 'dir'), mkdir(out); end
    print(fig, fullfile(out, [name '.png']), '-dpng', '-r130');
    close(fig);
    fprintf('saved figures/%s.png\n', name);
end
