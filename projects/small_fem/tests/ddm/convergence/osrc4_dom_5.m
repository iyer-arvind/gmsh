%%%%%%%%%%%%%%%%%%%%%%%
%% GetDP vs SmallFEM %%
%% OSRC4: 5 domains  %%
%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;

getdp = ...
    [
        6.407826311389e+01
        4.548936343152e+01
        3.607435385850e+01
        3.065548155047e+01
        4.046709520817e+00
        2.873465199730e+00
        1.708208729895e+00
        1.332093079066e+00
        3.479768307835e-02
        1.410911674085e-02
        9.388781507713e-03
        8.352471695167e-03
        5.337951745683e-03
        2.936213876619e-03
        1.293620563392e-03
        5.409201074815e-04
        1.492866476615e-04
        5.440393073779e-05
        1.305607419910e-05
        5.130425513263e-06
        1.881356602893e-06
        5.337140129846e-07
        2.511065640107e-07
        1.825310157284e-07
        1.066744815043e-07
        4.157043871739e-08
    ];

smallfem = ...
    [
        2.413018987987e+01
        1.710713880557e+01
        1.343040410704e+01
        1.137341912408e+01
        8.203061713160e+00
        6.752864647843e+00
        4.073770404423e+00
        3.200781071332e+00
        1.593741111483e-01
        5.018665114351e-02
        9.823132897921e-03
        4.761923219880e-03
        1.732936821869e-03
        9.836956104881e-04
        4.774603135267e-04
        2.093094698933e-04
        7.542270572165e-05
        3.321743031278e-05
        1.267635283030e-05
        7.106510299915e-06
        2.749340147460e-06
        1.723227443263e-06
        6.610332951188e-07
        2.878715693494e-07
        9.855735856731e-08
        3.829302424708e-08
        1.436257443794e-08
    ];

it = [1:size(smallfem, 1)];

getdp(end:size(smallfem, 1)) = getdp(end);

semilogy(it, [getdp, smallfem], '-o');
legend({'GetDP', 'SmallFEM'});
title('OSRC4: 5 domains');
xlabel('Iteration');
ylabel('Residual');