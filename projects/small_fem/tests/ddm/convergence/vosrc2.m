%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% GetDP vs SmallFEM         %%
%% Vectorial OSRC: 4 domains %%
%% K = 2                     %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;

getdp = ...
    [
        8.189114178712e+01
        5.892411513563e+01
        4.770721121231e+01
        2.168574949466e+01
        9.317656848197e+00
        3.718932216074e+00
        1.393013898653e+00
        6.379361261519e-01
        1.924994272548e-01
        9.372900786038e-02
        4.166909356779e-02
        1.948706491722e-02
        8.296491880276e-03
        4.124424736391e-03
        2.432951278363e-03
        1.496032619035e-03
        8.691153668049e-04
        4.875541559670e-04
        3.144515092799e-04
        1.991931089970e-04
        1.260590732709e-04
        7.436767395683e-05
        4.679649080135e-05
        2.662826214929e-05
        1.778880079556e-05
        9.649783370310e-06
        6.352688483996e-06
        3.493095630298e-06
        2.365533793873e-06
        1.537568355250e-06
        1.132819925834e-06
        6.245599670381e-07
        3.619406141647e-07
        1.957883105944e-07
        1.332652605179e-07
        8.417297950589e-08
%5.939303227790e-08
    ];

smallfem = ...
    [
        5.132388376611e+00
        3.669344113327e+00
        2.978380922431e+00
        1.121255591139e+00
        4.736182274962e-01
        2.089042383732e-01
        7.019404317594e-02
        3.209107304048e-02
        9.876968929229e-03
        4.834201377791e-03
        2.072946289977e-03
        9.569305199008e-04
        4.204135514946e-04
        2.137793586479e-04
        1.315729677120e-04
        8.158221707608e-05
        4.659706837848e-05
        2.609826391242e-05
        1.740364996699e-05
        1.089824456907e-05
        6.916211810891e-06
        4.050579191188e-06
        2.633711451949e-06
        1.470351482681e-06
        1.002557692582e-06
        5.332782803164e-07
        3.572800066623e-07
        1.948268686927e-07
        1.344337489702e-07
        8.707678164374e-08
        6.429811049617e-08
        3.358760762001e-08
        1.981188566258e-08
        1.084647627249e-08
        7.570711558372e-09
        4.733584534087e-09
    ];

it = [1:size(smallfem, 1)];

semilogy(it, ...
         getdp,             '-o', 'linewidth', 3, ...
         smallfem,          '-o', 'linewidth', 3, ...
         getdp ./ smallfem, '-*', 'linewidth', 3);
legend({'GetDP', 'SmallFEM', 'GetDP / SmallFEM'}, 'location', 'SouthWest');
title('Vectorial OSRC: 4 domains -- K = 2');
xlabel('Iteration');
ylabel('Residual');

print('vosrc2.png');