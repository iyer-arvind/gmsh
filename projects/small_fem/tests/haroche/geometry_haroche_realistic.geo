Include "parameters_gmsh_getdp.dat";

cl__1 = paramaille_air;
cl__2 = cl__1;
cl__m = paramaille_mir;

Point(1) = {0, 0, 0, cl__1};
Point(2) = {-9.944132009076509e-18, -9.650216777281143e-18, 0.013785, cl__m};
Point(3) = {-7.752647649704726e-18, 0.025, 0.004837586097078846, cl__m};
Point(4) = {1.530808498934192e-18, 0.025, 0.0152, cl__1};
Point(5) = {0, 0, 0.0152, cl__1};
Point(6) = {0, 0, 0.0189, cl__1};
Point(7) = {0, 0.0255, 0.0189, cl__1};
Point(8) = {0, 0.0255, 0, cl__1};
Point(9) = {0.0255, 0, 0.0189, cl__1};
Point(10) = {0.0255, 0, 0, cl__1};
Point(11) = {0.025, -6.123233995736766e-18, 0.0152, cl__1};
Point(12) = {0.02500000000009871, -7.886725386508955e-18, 0.005174998436730936, cl__m};
Point(13) = {0.0255, 0.0255, 0.0189, cl__1};
Point(14) = {0.0255, 0.0255, 0, cl__1};
Point(15) = {0.03136689481203155, 0, 0, cl__2};
Point(16) = {0.03136689481203155, 0.0255, 0, cl__2};
Point(17) = {0.03136689481203155, 0.03136689481203155, 0, cl__2};
Point(18) = {0, 0.03136689481203155, 0, cl__2};
Point(19) = {0.0255, 0.03136689481203155, 0, cl__2};
Point(20) = {0.03136689481203155, 0, 0.0189, cl__2};
Point(21) = {0.03136689481203155, 0.0255, 0.0189, cl__2};
Point(22) = {0.03136689481203155, 0.03136689481203155, 0.0189, cl__2};
Point(23) = {0, 0.03136689481203155, 0.0189, cl__2};
Point(24) = {0.0255, 0.03136689481203155, 0.0189, cl__2};
Point(25) = {0.03136689481203155, 0, 0.02476689481203155, cl__2};
Point(26) = {0.03136689481203155, 0.0255, 0.02476689481203155, cl__2};
Point(27) = {0.03136689481203155, 0.03136689481203155, 0.02476689481203155, cl__2};
Point(28) = {0, 0.03136689481203155, 0.02476689481203155, cl__2};
Point(29) = {0.0255, 0.03136689481203155, 0.02476689481203155, cl__2};
Point(30) = {0, 0, 0.02476689481203155, cl__2};
Point(31) = {0.0255, 0.0255, 0.02476689481203155, cl__2};
Point(32) = {0.0255, 0, 0.02476689481203155, cl__2};
Point(33) = {0, 0.0255, 0.02476689481203155, cl__2};
Line(1) = {1, 2};
p2 = newp;
Point(p2 + 1) = {-2.939152317953663e-19, 0, -0.025615};
Circle(2) = {2, p2 + 1, 3};
Line(3) = {3, 4};
Line(4) = {4, 5};
Line(5) = {5, 6};
Line(6) = {6, 7};
Line(7) = {8, 7};
Line(8) = {1, 8};
Line(9) = {10, 9};
Line(10) = {6, 9};
Line(11) = {5, 11};
Line(12) = {12, 11};
p13 = newp;
Point(p13 + 1) = {0, -9.650216777281143e-18, -0.02681500000000001};
Circle(13) = {2, p13 + 1, 12};
Line(14) = {1, 10};
p15 = newp;
Point(p15 + 1) = {0.02490379809193748, 0.002191082352467963, 0.005172711206637307,cl__m};
Point(p15 + 2) = {0.02461935203442672, 0.004345975552739354, 0.0051659784967942,cl__m};
Point(p15 + 3) = {0.02415775423095669, 0.006434509569626753, 0.005155149756454234,cl__m};
Point(p15 + 4) = {0.02353441838001276, 0.008433928010544885, 0.005140721967841496,cl__m};
Point(p15 + 5) = {0.02276575671184125, 0.01033055318841244, 0.005123249241037044,cl__m};
Point(p15 + 6) = {0.02186693155908324, 0.0121176440136178, 0.005103281960438248,cl__m};
Point(p15 + 7) = {0.02085140871447837, 0.01379198190033251, 0.005081352056676177,cl__m};
Point(p15 + 8) = {0.01973034677886372, 0.01535296115153555, 0.005057958763660923,cl__m};
Point(p15 + 9) = {0.01851285856938153, 0.0168010138513829, 0.005033576671567203,cl__m};
Point(p15 + 10) = {0.017205822320911, 0.01813724565457987, 0.005008656857028924,cl__m};
Point(p15 + 11) = {0.01581456529843362, 0.01936232224262593, 0.00498364691948941,cl__m};
Point(p15 + 12) = {0.01434302259085311, 0.02047627175369039, 0.00495900022351399,cl__m};
Point(p15 + 13) = {0.01279360285751708, 0.02147844806189237, 0.004935181284140782,cl__m};
Point(p15 + 14) = {0.0111680512583645, 0.02236681976967362, 0.00491268628939188,cl__m};
Point(p15 + 15) = {0.009467481130914559, 0.02313799484564215, 0.004892046437547984,cl__m};
Point(p15 + 16) = {0.007693600629868411, 0.02378672955869316, 0.004873840331423474,cl__m};
Point(p15 + 17) = {0.005849370719997872, 0.024306066510349, 0.00485868403072895,cl__m};
Point(p15 + 18) = {0.003941490837984046, 0.02468733770736251, 0.004847214745372073,cl__m};
Point(p15 + 19) = {0.001984565764542832, 0.02492110557820537, 0.004840035000748784,cl__m};
Spline(15) = {12, p15 + 1, p15 + 2, p15 + 3, p15 + 4, p15 + 5, p15 + 6, p15 + 7, p15 + 8, p15 + 9, p15 + 10, p15 + 11, p15 + 12, p15 + 13, p15 + 14, p15 + 15, p15 + 16, p15 + 17, p15 + 18, p15 + 19, 3};
p16 = newp;
Point(p16 + 1) = {0, 0, 0.0152};// centre cercle
Circle(16) = {11, p16 + 1, 4};
Line(17) = {7, 13};
Line(18) = {9, 13};
Line(19) = {8, 14};
Line(20) = {14, 13};
Line(21) = {10, 14};
Line Loop(1) = {1, 2, 3, 4, 5, 6, -7, -8};
Plane Surface(1) = {1};
Line Loop(2) = {9, -10, -5, 11, -12, -13, -1, 14};
Plane Surface(2) = {2};
Line Loop(3) = {2, -15, -13};
Ruled Surface(3) = {3};
Line Loop(4) = {12, 16, -3, -15};
Ruled Surface(4) = {4};
Line Loop(5) = {11, 16, 4};
Plane Surface(5) = {5};
Line Loop(6) = {6, 17, -18, -10};
Plane Surface(6) = {6};
Line Loop(7) = {19, 20, -17, -7};
Plane Surface(7) = {7};
Line Loop(8) = {8, 19, -21, -14};
Plane Surface(8) = {8};
Line Loop(9) = {9, 18, -20, -21};
Plane Surface(9) = {9};
Surface Loop(1) = {1, 2, 3, 4, 5, 6, 7, 8, 9};
Volume(1) = {1};
Line(22) = {10, 15};
Line(23) = {15, 16};
Line(24) = {16, 17};
Line(25) = {14, 16};
Line(26) = {14, 19};
Line(27) = {19, 17};
Line(28) = {8, 18};
Line(29) = {18, 19};
Line(30) = {7, 23};
Line(31) = {23, 24};
Line(32) = {24, 22};
Line(33) = {22, 21};
Line(34) = {21, 13};
Line(35) = {13, 24};
Line(36) = {21, 20};
Line(37) = {9, 20};
Line(38) = {30, 33};
Line(39) = {33, 28};
Line(40) = {28, 29};
Line(41) = {29, 27};
Line(42) = {27, 26};
Line(43) = {26, 25};
Line(44) = {25, 32};
Line(45) = {32, 31};
Line(46) = {31, 29};
Line(47) = {31, 33};
Line(48) = {33, 31};
Line(49) = {31, 26};
Line(50) = {30, 32};
Line(51) = {15, 20};
Line(52) = {20, 25};
Line(53) = {9, 32};
Line(54) = {16, 21};
Line(55) = {21, 26};
Line(56) = {17, 22};
Line(57) = {22, 27};
Line(58) = {13, 31};
Line(59) = {19, 24};
Line(60) = {24, 29};
Line(61) = {18, 23};
Line(62) = {23, 28};
Line Loop(63) = {22, 23, -25, -21};
Plane Surface(64) = {63};
Line Loop(65) = {24, -27, -26, 25};
Plane Surface(66) = {65};
Line Loop(67) = {26, -29, -28, 19};
Plane Surface(68) = {67};
Line Loop(69) = {51, -37, -9, 22};
Plane Surface(70) = {69};
Line Loop(71) = {37, 52, 44, -53};
Plane Surface(72) = {71};
Line Loop(73) = {18, 58, -45, -53};
Plane Surface(74) = {73};
Line Loop(75) = {37, -36, 34, -18};
Plane Surface(76) = {75};
Line Loop(77) = {43, 44, 45, 49};
Plane Surface(78) = {77};
Line Loop(79) = {36, 52, -43, -55};
Plane Surface(80) = {79};
Line Loop(81) = {54, 34, -20, 25};
Plane Surface(82) = {81};
Line Loop(83) = {51, -36, -54, -23};
Plane Surface(84) = {83};
Line Loop(85) = {49, -55, 34, 58};
Plane Surface(86) = {85};
Line Loop(87) = {54, -33, -56, -24};
Plane Surface(88) = {87};
Line Loop(89) = {56, -32, -59, 27};
Plane Surface(90) = {89};
Line Loop(91) = {32, 33, 34, 35};
Plane Surface(92) = {91};
Line Loop(93) = {46, -60, -35, 58};
Plane Surface(94) = {93};
Line Loop(95) = {55, -42, -57, 33};
Plane Surface(96) = {95};
Line Loop(97) = {46, 41, 42, -49};
Plane Surface(98) = {97};
Line Loop(99) = {57, -41, -60, 32};
Plane Surface(100) = {99};
Line Loop(101) = {59, -35, -20, 26};
Plane Surface(102) = {101};
Line(103) = {7, 33};
Line(104) = {6, 30};
Line Loop(105) = {47, 39, 40, -46};
Plane Surface(106) = {105};
Line Loop(107) = {17, 35, -31, -30};
Plane Surface(108) = {107};
Line Loop(109) = {40, -60, -31, 62};
Plane Surface(110) = {109};
Line Loop(111) = {61, -30, -7, 28};
Plane Surface(112) = {111};
Line Loop(113) = {62, -39, -103, 30};
Plane Surface(114) = {113};
Line Loop(115) = {59, -31, -61, 29};
Plane Surface(116) = {115};
Line Loop(117) = {103, -38, -104, 6};
Plane Surface(118) = {117};
Line Loop(119) = {50, -53, -10, 104};
Plane Surface(120) = {119};
Line Loop(121) = {45, 47, -38, 50};
Plane Surface(122) = {121};
Surface Loop(123) = {116, 112, 68, 102, 108, 7};
Volume(124) = {123};
Surface Loop(125) = {90, 88, 66, 92, 82, 102};
Volume(126) = {125};
Surface Loop(127) = {100, 96, 98, 94, 86, 92};
Volume(128) = {127};
Line Loop(129) = {47, -103, 17, 58};
Plane Surface(130) = {129};
Surface Loop(131) = {108, 114, 110, 106, 94, 130};
Volume(132) = {131};
Surface Loop(133) = {122, 118, 120, 6, 74, 130};
Volume(134) = {133};
Surface Loop(135) = {72, 80, 78, 74, 86, 76};
Volume(136) = {135};
Surface Loop(137) = {64, 70, 84, 9, 82, 76};
Volume(138) = {137};

Physical Volume(138) = {1}; // Air
Physical Volume(139) = {138}; // PML X
Physical Volume(140) = {126}; // PMLXY
Physical Volume(141) = {124}; // PML Y
Physical Volume(142) = {134}; // PML Z
Physical Volume(143) = {128}; // PML XYZ
Physical Volume(144) = {136}; // PML XZ
Physical Volume(145) = {132}; // PML YZ

Physical Surface(146) = {2, 70, 72, 120};   // XOZ parallel faces (for sym, apply neumann/diri)
Physical Surface(147) = {1, 112, 118, 114}; // YOZ parallel faces (for sym, apply neumann/diri)
Physical Surface(149) = {8,64,66,68}; // XOY parallel faces (for sym, apply neumann/diri)
Physical Surface(148) = {3};   // mirror
Physical Point(1000000) = {1}; // YOZ parallel faces (for sym, apply neumann/diri)
