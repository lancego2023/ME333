#include "NU32.h"
#include <stdio.h>

int main() {
    char c1=5, c2=6, c3;
    int i1=5, i2=6, i3;
    long long int j1=5, j2=6, j3;
    float f1=1.01, f2=2.02, f3;
    long double d1=1.01, d2=2.02, d3;

    // char calc
    c3 = c1+c2;
    c3 = c1-c2;
    c3 = c1*c2;
    c3 = c1/c2;

    // int calc
    i3 = i1+i2;
    i3 = i1-i2;
    i3 = i1*i2;
    i3 = i1/i2;

    // long long int calc
    j3 = j1+j2;
    j3 = j1-j2;
    j3 = j1*j2;
    j3 = j1/j2;

    // float calc
    f3 = f1+f2;
    f3 = f1-f2;
    f3 = f1*f2;
    f3 = f1/f2;

    // long double calc
    d3 = d1+d2;
    d3 = d1-d2;
    d3 = d1*d2;
    d3 = d1/d2;

    unsigned int u1=33, u2=17, u3;
    u3 = u1 & u2; // bitwise AND
    u3 = u1 | u2; // bitwise OR
    u3 = u2 << 4; // shift left 4 spaces, or multiply by 2ˆ4 = 16
    u3 = u1 >> 3; // shift right 3 spaces, or divide by 2ˆ3 = 8
}