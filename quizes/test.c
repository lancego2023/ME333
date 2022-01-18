#include <stdio.h>


struct car {
    int id;
    char brand[50];
    float miles[10];
}; 

void getBrand(char *brand);
struct car milesVal(struct car car0);
void milesRef(struct car *car1);

int main(){
    struct car volvo;
    struct car *volvo_pointer;

    volvo_pointer = &volvo;

    volvo.id = 1;
    getBrand(volvo_pointer->brand);
    volvo.miles[0] = 1.0;
    milesRef(volvo_pointer);
    for(int ii = 0; ii < 10; ii++){
        printf("%f \n", volvo.miles[ii]);
    }

    return 0;
}

void getBrand(char *brand){
    printf("enter brand: \n");
    scanf("%50s", brand);

    return;
}

struct car milesVal(struct car car0){
    float miles1 = car0.miles[0];
    for (int jj = 1; jj < 10; jj++){
        miles1 = miles1 * 2;
        car0.miles[jj] = miles1;
    }

    return car0;
}

void milesRef(struct car *car1){
    float miles1 = car1->miles[0];
    for (int jj = 1; jj < 10; jj++){
        miles1 = miles1 * 2;
        car1->miles[jj] = miles1;
    }
    return;
}
