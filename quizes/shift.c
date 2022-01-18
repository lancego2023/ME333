#include <stdio.h>

int main(){
    char str[50];
    int val = 0;
    printf("enter your string: \n");
    scanf("%50s", str);

    while(!(val >= 1 && val <= 8)){  // prompts for user input until valid
        printf("enter your number: \n");
        scanf("%d", &val);
    }

    int ii = 0;    // shift part. Used shifts each character until null term reached
    while(str[ii] != '\0'){
        str[ii] = str[ii] + val;
        ii++;
    }

    printf("shifted string: %s\n", str);

    return 0;
}