// Ascii to print
// lance go

#include <stdio.h>
#include <stdlib.h>

int main() {

    char tests[] = "";
    int linebreak = 0;
    for(int ii = 33; ii < 129; ii++){

        *tests = (char) ii; // explicitly cast ii as a char

        printf("%d: %s     ", ii, tests); // print tests string
        if(linebreak == 5){ // print 6 to a line
            printf("\n");
            linebreak = 0;
        } else {
            linebreak++;
        }

    }

    return 0;
}