#include <cmath>
#include <iomanip>
#include <iostream>
#include <fstream>
#include <algorithm>
#include "consts.h"

double g(double u1){
    double m1 = preset[active_preset][6];
    double m0 = preset[active_preset][5];
    return m2*u1 + 0.5*(m1-m0)*(std::abs(u1-Bp)-std::abs(u1+Bp)) + 0.5*(m2-m1)*(std::abs(u1-B0)-std::abs(u1+B0));
}

void chua_solver(double* state, double* model){
    double G = preset[active_preset][2];
    double C1 = preset[active_preset][0];
    double C2 = preset[active_preset][1];
    double L = preset[active_preset][4];
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
        temp[i] = state[i] + 0.5*h*k[0][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[1][i] = model[i];
    }

    // k3
    for(int i=0;i<3;i++){
        temp[i] = state[i] + 0.5*h*k[1][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[2][i] = model[i];
    }

    // k4 
    for(int i=0;i<3;i++){
        temp[i] = state[i] + h*k[2][i];
    }
    chua_solver(temp, model);
    for(int i=0;i<3;i++){
        k[3][i] = model[i];
    }

    // new
    for (int i=0;i<3;i++){
        state[i] = state[i] + h * (k[0][i] + 2*k[1][i] + 2*k[2][i] + k[3][i]) / 6;
    }
}

int main(){
    std::ofstream file("basins_8.csv");
    file << std::setprecision(15);
    file << "u2_start,i_start,min_u1,max_u1,color\n";

    int tTest = max_rk4/2;
    
    for(double u2 = u2_start; u2 <= u2_end; u2+=delta){
        for(double i=i_start;i<=i_end;i+=delta){
            state[0] = u2;
            state[1] = 3.0;
            state[2] = i;
            
            double min_u1 = 0;
            double max_u1 = 0;
            int color = 0;

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
                
                if (std::abs(state[1] > uMax)){
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

            file << u2 << ',' << i << ','
                << min_u1 << ',' << max_u1 << ','
                << color << '\n';
        }
    }

    file.close();
    return 0;
}