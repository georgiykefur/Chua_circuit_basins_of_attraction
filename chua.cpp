#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "consts.h"

double g(double u1){
    double m1 = preset[6];
    double m0 = preset[5];
    return m2*u1 + 0.5*(m1-m0)*(std::abs(u1-Bp)-std::abs(u1+Bp)) + 0.5*(m2-m1)*(std::abs(u1-B0)-std::abs(u1+B0));
}

void chua_solver(double* state, double* model){
    double G = preset[2];
    double C1 = preset[0];
    double C2 = preset[1];
    double L = preset[4];
    double u2 = state[0];
    double u1 = state[1];
    double i = state[2];

    // u1
    model[1] = (G*(u2 - u1)-g(u1))/C1;
    model[0] = (G*(u1 - u2) + i) / C2;
    model[2] = -u2/L;
}

void rk4_step(double* state){
    double model[3]; // u2 u1 i

    // k1
    chua_solver(state, model);
    double k[4][3];
    for (int i=0;i<3;i++){
        k[0][i] = model[i];
    }

    // k2
    double temp[3];
    for(int i=0;i<3;i++){
        temp[i] = state[i] + 0.5*h_rk4*k[0][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[1][i] = model[i];
    }

    // k3
    for(int i=0;i<3;i++){
        temp[i] = state[i] + 0.5*h_rk4*k[1][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[2][i] = model[i];
    }

    // k4 
    for(int i=0;i<3;i++){
        temp[i] = state[i] + h_rk4*k[2][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[3][i] = model[i];
    }

    // new
    for (int i=0;i<3;i++){
        state[i] = state[i] + h_rk4* (k[0][i] + 2*k[1][i] + 2*k[2][i] + k[3][i]) / 6;
    }
}

void rk4(double* state, double& min_u1, double& max_u1, int& color){
    int tTest = max_rk4 / 2;
    for(int n=0;n<max_rk4;n++){
        rk4_step(state);

        if (n == tTest) {
            min_u1 = state[1];
            max_u1 = state[1];
        }

        if (n >= tTest) {
            if (state[1] < min_u1) min_u1 = state[1];
            if (state[1] > max_u1) max_u1 = state[1];
        }
        
        if (std::abs(state[1]) > uMax){
            color = 3;
            break;
        }

        if (n > tTest) {
            double _u1 = state[1];
            double _u2 = state[0];
            double _i = state[2];

            // red
            if (_u1 > u1Min && _u1 < u1Max && _u2 > u2Min && _u2 < u2Max && _i > iMin && _i < iMax) {
                color = 1; 
                break; 
            }
            
            // green
            if (_u1 < -u1Min && _u1 > -u1Max && _u2 > u2Min && _u2 < u2Max && _i > iMin && _i < iMax) {
                color = 2; 
                break; 
            }
        }
    }
}

// dopri5
const double a[7][6] = {
    {},
    {1.0/5},
    {3.0/40, 9.0/40},
    {44.0/45, -56.0/15, 32.0/9},
    {19372.0/6561, -25360.0/2187, 64448.0/6561, -212.0/729},
    {9017.0/3168, -355.0/33, 46732.0/5247, 49.0/176, -5103.0/18656},
    {35.0/384, 0.0, 500.0/1113, 125.0/192, -2187.0/6784, 11.0/84}
};

const double b5[7] = {35.0/384, 0.0, 500.0/1113, 125.0/192, -2187.0/6784, 11.0/84, 0.0};

const double b4[7] = {5179.0/57600, 0.0, 7571.0/16695, 393.0/640, -92097.0/339200, 187.0/2100, 1.0/40};

double dopri5_step(double* state, double h){
    double model[3];
    double k[7][3];

    for(int s=0; s<7;s++){
        double temp[3];
        for(int i=0;i<3;i++){
            double curr = state[i];
            for(int j=0;j<s;j++){
                curr += h * a[s][j] * k[j][i];
            }
            temp[i] = curr;
        }
        chua_solver(temp, model);
        for(int j=0;j<3;j++){
            k[s][j] = model[j];
        }
    }

    double err_sq = 0.0;
    for(int i=0;i<3;i++){
        double y5 = state[i];
        double y4 = state[i];
        double err_i = 0.0;

        for (int s = 0; s < 7; ++s) {
            y5 += h * b5[s] * k[s][i];
            y4 += h * b4[s] * k[s][i];
            err_i += h * (b5[s] - b4[s]) * k[s][i];
        }
        double toli = a_tol + r_tol * std::max(std::abs(y4), std::abs(y5));
        double t = err_i / toli;
        err_sq += t * t;

        state[i] = y5; 
    }
    return std::sqrt(err_sq / 3.0);
}

void dopri5(double* state, double& min_u1, double& max_u1, int& color){
    double t = 0.0;
    double h = h_start;
    bool flag = false;
    while (t < t_end){
        if (t+h > t_end){
            h = t_end - t;
        }

        double state_prev[3] = { state[0], state[1], state[2] };
        double err = dopri5_step(state, h);

        if (err <= 1.0){
            t += h;
            h = 0.9 * h * std::pow(1.0 / err, 0.2);

            int tTest = 50;
            if (t >= tTest && !flag) {
                    min_u1 = state[1];
                    max_u1 = state[1];
                    flag = true;
                }

            if (flag) {
                if (state[1] < min_u1) min_u1 = state[1];
                if (state[1] > max_u1) max_u1 = state[1];
            }
            
            if (std::abs(state[1]) > uMax){
                color = 3;
                break;
            }

            if (t > tTest) {
                double _u1 = state[1];
                double _u2 = state[0];
                double _i = state[2];

                // red
                if (_u1 > u1Min && _u1 < u1Max && _u2 > u2Min && _u2 < u2Max && _i > iMin && _i < iMax) {
                    color = 1; 
                    break; 
                }
                
                // green
                if (_u1 < -u1Min && _u1 > -u1Max && _u2 > u2Min && _u2 < u2Max && _i > iMin && _i < iMax) {
                    color = 2; 
                    break; 
                }
            }
        }
        else{
            state[0] = state_prev[0];
            state[1] = state_prev[1];
            state[2] = state_prev[2];

            h = 0.9 * h * std::pow(1.0 / err, 0.2);
        }

        if (h < 1e-14) {
            std::cout << "step too small" << std::endl;
            break;
        }
    }
}


int main(){
    std::ofstream file("basins.csv");
    file << std::setprecision(15);
    file << "u2_start,i_start,min_u1,max_u1,color\n";

    for(double u2 = u2_start; u2 <= u2_end; u2+=delta){
        for(double i=i_start;i<=i_end;i+=delta){
            state[0] = u2;
            state[1] = 3.0;
            state[2] = i;
            
            double min_u1 = 0;
            double max_u1 = 0;
            int color = 0;

            // для выбора DOPRI5 нужно поменять фунцию rk4 на dopri5. 
            // аргументы функций одинаковые 
            rk4(state, min_u1, max_u1, color);

            file << u2 << ',' << i << ','
                << min_u1 << ',' << max_u1 << ','
                << color << '\n';

        }
    }

    file.close();
    return 0;
}