function c = arm_cmap(n)
%ARM_CMAP Perceptual colormap available in both MATLAB and Octave.
    if exist('parula', 'file') || exist('parula', 'builtin'), c = parula(n); else, c = viridis(n); end
end
