#include "mcc_generated_files/system.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <libpic30.h>

#define BUF_SIZE 10
#define BACKSPACE 0x8 // ASCII backspace character code

char *getStringU2(char *s, int len); //Reading string from console

int main(void) {
    char comand[BUF_SIZE];
    
    // initialize the device
    SYSTEM_Initialize();
    
    printf("Testing UART Program!\n");
    
    while (1) {
        getStringU2(comand, sizeof(comand));
        
        char SSMODE[10];
        char SGAIN[10];
        char SI[10];
        char SCOUNT[10];
        char SM[10];
        char SEND[10];

        sprintf(SSMODE, "%.*s", 6, comand );
        sprintf(SGAIN, "%.*s", 5, comand);
        sprintf(SI, "%.*s", 2, comand);
        sprintf(SCOUNT, "%.*s", 6, comand);
        sprintf(SM, "%.*s", 2, comand);
        sprintf(SEND, "%.*s", 4, comand);
        
        if(strcmp(SSMODE, "SSMODE") == 0){
            MODE_SetHigh();
            printf("!");
        }
        else if(strcmp(SGAIN, "SGAIN") == 0) {
            char gain[1];
            sprintf(gain, "%.*s", 1, comand + 5);
            
            if(gain[0] > 48 && gain[0] < 52) {
                if(atoi(gain) == 3){
                    GAIN_0_SetHigh();
                    GAIN_1_SetHigh();
                    printf("!");
                }
                else if(atoi(gain) == 2){
                    GAIN_0_SetLow();
                    GAIN_1_SetHigh();
                    printf("!");
                }
                else{
                    GAIN_0_SetHigh();
                    GAIN_1_SetLow();
                    printf("!");
                }
            }
            else {
                GAIN_0_SetHigh();
                GAIN_1_SetLow();
            }
        }
        else if(strcmp(SI, "SI") == 0) {
            char integration[4];
            sprintf(integration, "%.*s", 4, comand + 2);
            
            if((integration[0] > 47 && integration[0] < 58) &&
            (integration[1] > 47 && integration[1] < 58) &&
            (integration[2] > 47 && integration[2] < 58) &&
            (integration[3] > 47 && integration[3] < 58)) {
                if(atoi(integration) >= 6666){
                    INTE_0_SetHigh();
                    INTE_1_SetHigh();
                    printf("!");
                }
                else if(atoi(integration) >= 3333){
                    INTE_0_SetLow();
                    INTE_1_SetHigh();
                    printf("!");
                }
                else {
                    INTE_0_SetHigh();
                    INTE_1_SetLow();
                    printf("!");
                }
            }
            else {
                INTE_0_SetHigh();
                INTE_1_SetLow();
            }
        } 
        else if(strcmp(SCOUNT, "SCOUNT") == 0) {
            unsigned int reading ;
            
            while(1){
                reading = rand()%65535;
                printf("%d", reading);                
            }
        }
        else if(strcmp(SM, "SM") == 0){
            int reading = rand()%65535;
            char no_readings[4];
            sprintf(no_readings, "%.*s", 4, comand + 2);
            
            if((no_readings[0] > 47 && no_readings[0] < 58) &&
            (no_readings[1] > 47 && no_readings[1] < 58) &&
            (no_readings[2] > 47 && no_readings[2] < 58) &&
            (no_readings[3] > 47 && no_readings[3] < 58)) {
                for(int i = 0; i < atoi(no_readings); i++) {
                    reading = rand()%65535;             
                    printf("%d\n", reading);
                }
                printf("!");
            }
            else {
                printf("%s", comand);
            }
        }
        else if(strcmp(SEND, "SEND") == 0) {
            MODE_SetLow();
            printf("END !");
        }
        else {
            printf("%s", comand);
        }
        
        printf("\r\n");
    }
    
    return 1;
}

char *getStringU2(char *s, int len){
    char *p = s; // copy the buffer pointer
    do{
        *s = (char)UART2_Read(); // wait for a new character
        UART2_Write(*s); // echo character
        if ((*s == BACKSPACE) && (s > p)){
            UART2_Write(' '); // overwrite the last character
            UART2_Write(BACKSPACE);
            len++;
            s--; // back the pointer
            continue;
        }
        if (*s == '\n') // line feed, ignore it
            continue;
        if (*s == '\r') // end of line, end loop
            break;
        s++; // increment buffer pointer
        len--;
    } while (len > 1); // until buffer full
    
    *s = '\0'; // null terminate the string
    return p; // return buffer pointer
}

