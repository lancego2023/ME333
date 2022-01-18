#include<stdio.h>

int main(){
unsigned char q[5] = {2, 12, 'j', 128, 10}; 
unsigned char a; int b; 

    a = q[q[0]]+1;  
    b = q[4] / q[5];  

    printf("%d \n", b);

}