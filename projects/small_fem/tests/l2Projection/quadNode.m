close all;
clear all;

h = [1, 0.5, 0.25, 0.125, 0.0625];
p = [1:4];

l2 = ...
    [...
        +1.028260e+00 , +7.464563e-01 , +2.729451e-01 , +6.583108e-02 , +1.436498e-02 ; ...
        +8.537541e-01 , +2.664175e-01 , +5.293406e-02 , +8.911445e-03 , +1.333801e-03 ; ...
        +8.174877e-01 , +1.127773e-01 , +8.396360e-03 , +5.063733e-04 , +2.733107e-05 ; ...
        +2.876364e-01 , +2.284130e-02 , +1.048153e-03 , +4.036206e-05 , +1.377807e-06 ; ...
    ];

P = size(p, 2);
H = size(h, 2);

delta = zeros(P, H - 1);

for i = 1:H-1
  delta(:, i) = ...
    (log10(l2(:, i + 1)) - log10(l2(:, i))) / ...
    (log10(1/h(i + 1))   - log10(1/h(i)));
end

delta

figure;
loglog(1./h, l2', '-*');
grid;
title('quad: Nodal');

xlabel('1/h [-]');
ylabel('L2 Error [-]');
