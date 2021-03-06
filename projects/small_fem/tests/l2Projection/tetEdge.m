close all;
clear all;

h = [1, 0.5, 0.25, 0.125, 0.0625];
p = [1:4];

l2 = ...
    [...
        +1.133908e+00 , +6.864914e-01 , +1.830571e-01 , +5.099297e-02 , +1.275090e-02 ; ...
        +1.104692e+00 , +2.524348e-01 , +4.827834e-02 , +6.439655e-03 , +8.565387e-04 ; ...
        +7.135950e-01 , +9.923852e-02 , +5.938545e-03 , +3.870427e-04 , +2.306161e-05 ; ...
        +3.400753e-01 , +2.107130e-02 , +1.001332e-03 , +3.386298e-05 , +1.081343e-06 ; ...
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
title('tet: Edge');

xlabel('1/h [-]');
ylabel('L2 Error [-]');
