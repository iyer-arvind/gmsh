%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
%% GetDP vs SmallFEM         %%
%% Vectorial OSRC: 4 domains %%
%% K = 5                     %%
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

clear all;
close all;

getdp = ...
    [
        8.074052888114e+01
        5.854450379978e+01
        4.847226380087e+01
        2.711903317603e+01
        1.404929094389e+01
        4.795620917781e+00
        2.399736949100e+00
        1.318563505668e+00
        8.073962544727e-01
        5.054213146603e-01
        3.358908151167e-01
        1.992707302535e-01
        1.377351340290e-01
        9.852786294699e-02
        7.293091523118e-02
        5.062912766782e-02
        3.743905252999e-02
        2.769765533846e-02
        2.182073811755e-02
        1.748957637286e-02
        1.468875591054e-02
        1.262805347282e-02
        1.135897952824e-02
        1.060236607605e-02
        1.002653880076e-02
        9.228196308493e-03
        8.312024355137e-03
        6.981088388394e-03
        5.753568241856e-03
        4.695918328888e-03
        3.983443593140e-03
        3.428105683171e-03
        3.009378998470e-03
        2.692583585572e-03
        2.469938534316e-03
        2.324060980981e-03
        2.210425442217e-03
        2.099657601455e-03
        1.978011731839e-03
        1.794409053335e-03
        1.592415601071e-03
        1.333095187451e-03
        1.104504126732e-03
        8.759874233617e-04
        7.008330823270e-04
        5.368751129804e-04
        4.129869504096e-04
        3.027460309271e-04
        2.313171319368e-04
        1.717579470100e-04
        1.329271082588e-04
        9.750291829143e-05
        7.360106150519e-05
        5.448971983406e-05
        4.155170380206e-05
        3.069059134656e-05
        2.361700377026e-05
        1.741892062353e-05
        1.310617205467e-05
        9.565907874253e-06
        7.391028390413e-06
        5.667853296848e-06
        4.569049729449e-06
        3.498889016017e-06
        2.757599607281e-06
        2.158100317314e-06
        1.760211404825e-06
        1.433668707604e-06
        1.188066186308e-06
        9.522170195994e-07
        7.659778486025e-07
        6.014288424667e-07
        4.814439092626e-07
        3.667191612091e-07
        2.860752346918e-07
        2.141392544956e-07
        1.670793488309e-07
        1.266963075558e-07
        1.000322766746e-07
        7.449658062455e-08
    ];

smallfem = ...
    [
        5.024887230068e+00
        3.606998523206e+00
        2.977109690864e+00
        1.369313444296e+00
        6.707962579821e-01
        2.481236947475e-01
        1.230870059015e-01
        6.924046863191e-02
        4.304934329926e-02
        2.615664972015e-02
        1.725333424850e-02
        1.001259986608e-02
        6.971309164111e-03
        4.991370916727e-03
        3.740559466500e-03
        2.548254575769e-03
        1.883094929715e-03
        1.379457761357e-03
        1.089837206814e-03
        8.613406737186e-04
        7.240078444886e-04
        6.100646724682e-04
        5.456498315518e-04
        5.054945734909e-04
        4.789094814203e-04
        4.426528221683e-04
        4.042365442329e-04
        3.414627512884e-04
        2.845140593599e-04
        2.312404549531e-04
        1.961872131223e-04
        1.665464261784e-04
        1.459851980439e-04
        1.288872916697e-04
        1.178739219554e-04
        1.102863208431e-04
        1.049885459881e-04
        9.980068268184e-05
        9.438811960572e-05
        8.607533701996e-05
        7.715747268001e-05
        6.500938743734e-05
        5.446000722895e-05
        4.343168123832e-05
        3.528171088914e-05
        2.699536679470e-05
        2.108213987284e-05
        1.537121242512e-05
        1.181226257890e-05
        8.771647567235e-06
        6.871242863492e-06
        4.983960115892e-06
        3.785959603541e-06
        2.766618792936e-06
        2.115041346222e-06
        1.553045003372e-06
        1.207924934407e-06
        8.765440946843e-07
        6.623911355654e-07
        4.850621633815e-07
        3.778360033976e-07
        2.887612863662e-07
        2.336028504472e-07
        1.753409871200e-07
        1.377238485360e-07
        1.066861116665e-07
        8.669286336430e-08
        7.053220375004e-08
        5.894300629671e-08
        4.694166072010e-08
        3.786906534291e-08
        2.977234351318e-08
        2.409566143519e-08
        1.838477442433e-08
        1.447847133679e-08
        1.082617705195e-08
        8.501486892040e-09
        6.474474140035e-09
        5.140004697850e-09
        3.792439157406e-09
    ];

it = [1:size(smallfem, 1)];

semilogy(it, ...
         getdp,             '-o', 'linewidth', 3, ...
         smallfem,          '-o', 'linewidth', 3, ...
         getdp ./ smallfem, '-*', 'linewidth', 3);
legend({'GetDP', 'SmallFEM', 'GetDP / SmallFEM'}, 'location', 'SouthWest');
title('Vectorial OSRC: 4 domains -- K = 5');
xlabel('Iteration');
ylabel('Residual');

print('vosrc5.png');