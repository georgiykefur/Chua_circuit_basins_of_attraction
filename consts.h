#pragma once

const int active_preset = 0;

// PC3
const double preset[][7] = {
    // C1, C2, G, Rl, L, m0, m1
    {0.104443, 0.85, 1, 0, 0.0625, -1.143, -0.714}
};

double u2_start = -2;
 double u1_start = -2;
//double u1_start = -6;
double i_start = -6;

double delta_u2 = 4;
 double delta_u1 = 4;
//double delta_u1 = 12;
double delta_i = 12;

double u2_end = u2_start + delta_u2;
double u1_end = u1_start + delta_u1;
double i_end = i_start + delta_i;

// u2 [V], u1 [V], i [A]
 double state[3] = {u2_start, 3, i_start};
//double state[3] = {u2_start, u1_start, 3};

// yellow
// double state[3] = {1.3, 3, 3};

// red
// double state[3] = {-0.690910, 0.0, 0.709086};

// green
// double state[3] = {-0.681819, 0.0, 0.709086};

double delta = 0.005;

// 7
double m2 = 7;
// [V]
double B0 = 14;
double Bp = 1;

// h в RK4
double h = 0.005;
int max_rk4 = 40000;
