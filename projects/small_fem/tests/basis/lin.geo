cl  = 1000000;
msh = 2;

Point(1) = {-1, 0, 0, cl};
Point(2) = { 1, 0, 0, cl};

Line(1) = {1, 2};

Transfinite Line {1} = msh Using Progression 1;

Physical Line(7) = {1};
