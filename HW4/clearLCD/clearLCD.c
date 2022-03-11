#include "NU32.h"
#include "LCD.h"

int main() {

    char msg[16];

    // initialize necessary things
    NU32_Startup();
    LCD_Setup();

    while(1){
        // takes user input first
        NU32_WriteUART3("Input message now: \r\n");
        NU32_ReadUART3(msg, 16);

        // prints as long as USER not pressed
        while(NU32_USER){
            LCD_Move(0,0);
            LCD_WriteString(msg);
        }

        // clears when its done
        LCD_Clear();
        NU32_WriteUART3("Cleared Output.\r\n\r\r");
        // back to top
    }


return 0;
}
