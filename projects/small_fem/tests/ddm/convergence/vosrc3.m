%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% GetDP vs SmallFEM         %%
%% Vectorial OSRC: 4 domains %%
%% K = 3                     %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;

getdp = ...
    [
        8.062667716291e+01
        5.810755153149e+01
        4.729242365351e+01
        2.280150532701e+01
        1.036137935355e+01
        3.544728063324e+00
        1.536068857315e+00
        8.495937358121e-01
        3.719246241784e-01
        1.957190081800e-01
        8.495845225345e-02
        4.150646444228e-02
        2.287323234518e-02
        1.121206366245e-02
        6.360271230924e-03
        3.637724729628e-03
        2.448312483816e-03
        1.356430889082e-03
        9.246241129813e-04
        5.398900201709e-04
        3.555129061932e-04
        2.082401203808e-04
        1.388963127300e-04
        7.320940409505e-05
        4.700098187249e-05
        2.755608723121e-05
        1.858126690306e-05
        1.095564392976e-05
        7.490472781144e-06
        4.353029822607e-06
        2.888446145317e-06
        1.633829622176e-06
        1.115049752148e-06
        7.184000973000e-07
        4.992331931659e-07
        2.904646281884e-07
        1.951159790691e-07
        1.161354021329e-07
        7.958813903292e-08
    ];

smallfem = ...
    [
        5.041014220351e+00
        3.604816336092e+00
        2.936867326511e+00
        1.150913467149e+00
        5.048064554895e-01
        1.965279668062e-01
        7.972939797427e-02
        4.448905424500e-02
        1.915087791619e-02
        9.944706948748e-03
        4.338348187028e-03
        2.090847658396e-03
        1.177839113162e-03
        5.676861083470e-04
        3.360228915641e-04
        1.954732634664e-04
        1.339714344724e-04
        7.357584349142e-05
        5.121367293018e-05
        2.904155315036e-05
        1.931203075194e-05
        1.113828769332e-05
        7.517096275162e-06
        3.893444350352e-06
        2.588198495214e-06
        1.507856585101e-06
        1.027230092076e-06
        5.976771282000e-07
        4.112851412108e-07
        2.324101527339e-07
        1.567539718185e-07
        8.731880576424e-08
        6.106398273071e-08
        3.870211452940e-08
        2.704802716703e-08
        1.532583200927e-08
        1.051350327229e-08
        6.132643786054e-09
        4.295316912150e-09
    ];

it = [1:size(smallfem, 1)];

semilogy(it, ...
         getdp,             '-o', 'linewidth', 3, ...
         smallfem,          '-o', 'linewidth', 3, ...
         getdp ./ smallfem, '-*', 'linewidth', 3);
legend({'GetDP', 'SmallFEM', 'GetDP / SmallFEM'}, 'location', 'SouthWest');
title('Vectorial OSRC: 4 domains -- K = 3');
xlabel('Iteration');
ylabel('Residual');

print('vosrc3.png');