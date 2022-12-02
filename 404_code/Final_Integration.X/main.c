#include "mcc_generated_files/system.h"
#include "mcc_generated_files/spi2.h"
#include "mcc_generated_files/pin_manager.h"
#include "mcc_generated_files/spi1.h"
#include "mcc_generated_files/interrupt_manager.h"
#include "mcc_generated_files/uart1.h"
#include <libpic30.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <inttypes.h>
//------------------------------------------------------------------------------
#define DECODE_MODE_REG     0x09
#define INTESITY_REG        0x0A
#define SCAN_LIMIT_REG      0x0B
#define SHUTDOWN_REG        0x0C
#define DISPLAY_TEST_REG    0x0F
//------------------------------------------------------------------------------
#define DISABLE_DECODE      0x00
#define DECODE_ALL          0xFF
#define BRIGHTNESS          0x05
#define SCAN_LOW_NIBBLE     0x03
#define SCAN_ALL_DIGITS     0x04
#define SHUTDOWN_MODE       0x00
#define NORMAL_OPERATION    0x01
#define DISABLE_TEST_MODE   0x00
#define ENABLE_TEST_MODE    0x01
//------------------------------------------------------------------------------
#define BUF_SIZE 10
#define BACKSPACE 0x8 // ASCII backspace character code
//------------------------------------------------------------------------------
static const struct{
	char   ascii;
	char   segs;
} MAX7219_Font[] = {
    {'A',0b1110111},{'B',0b1111111},{'C',0b1001110},{'D',0b1111110},{'E',0b1001111},
    {'F',0b1000111},{'G',0b1011110},{'H',0b0110111},{'I',0b0110000},{'J',0b0111100},
    {'L',0b0001110},{'N',0b1110110},{'O',0b1111110},{'P',0b1100111},{'R',0b0000101},
    {'S',0b1011011},{'T',0b0001111},{'U',0b0111110},{'Y',0b0100111},{'[',0b1001110},
    {']',0b1111000},{'_',0b0001000},{'a',0b1110111},{'b',0b0011111},{'c',0b0001101},
    {'d',0b0111101},{'e',0b1001111},{'f',0b1000111},{'g',0b1011110},{'h',0b0010111},       
    {'i',0b0010000},{'j',0b0111100},{'l',0b0001110},{'n',0b0010101},{'o',0b1111110},
    {'p',0b1100111},{'r',0b0000101},{'s',0b1011011},{'t',0b0001111},{'u',0b0011100},
    {'y',0b0100111},{'-',0b0000001},{' ',0b0000000},{'0',0b1111110},{'1',0b0110000},
    {'2',0b1101101},{'3',0b1111001},{'4',0b0110011},{'5',0b1011011},{'6',0b1011111},
    {'7',0b1110000},{'8',0b1111111},{'9',0b1111011},{'/0',0b0000000}
};
//------------------------------------------------------------------------------
void MAX7219_Initialize();                      // initialize display
void MAX7219_Display(uint32_t num);             // display digit
void MAX7219_DisplayOpt(uint32_t num, bool opt);// display 
void MAX7219_DisplayChar(char chr);             // display character
void MAX7219_Clear();                           // clear display
uint32_t ADC_Read();                            // reading 16-bit value from ADC
void Set_Gain(int gain);                        // set gain value of photometer
int Get_Integ(int integ);                       // get integration value equiv.
char *getStringU1(char *s, int len);            // reading string from console

int main(void) 
{
    // program variables
    uint8_t hard_integ = 1;
    uint16_t soft_integ = 1;
    uint8_t gain = 1;
    uint8_t start = 0;
    bool display = false;
    char comand[BUF_SIZE];
    
    // initialize the device
    SYSTEM_Initialize();
    MAX7219_Initialize();
    
    while (1) 
    {
        /*-----------------------Reading Device Inputs------------------------*/
        
        // obtain Gain and Integration value from user input
        if (GAIN_BTN_GetValue()) {
            while (GAIN_BTN_GetValue()) {}
            display = false;
            gain = gain%3;
            gain += 1;
            __delay_ms(50);
        }
        if (INTEG_BTN_GetValue()) {
            while (INTEG_BTN_GetValue()) {}
            display = true;
            hard_integ = hard_integ%3;
            hard_integ += 1;
            __delay_ms(50);
        }
        if (START_BTN_GetValue()) {
            while (START_BTN_GetValue()) {}
            start = 1;
            __delay_ms(50);
        }
        
        // Display Integration value based on user input
        if ((hard_integ == 1) && display) {
            __delay_ms(50);
            MAX7219_Display(1);
        }
        else if ((hard_integ == 2) && display) {
            __delay_ms(50);
            MAX7219_Display(5);
        }
        else if ((hard_integ == 3) && display) {
            __delay_ms(50);
            MAX7219_Display(10);
        }
        
        // Display Gain value based on user input
        if ((gain == 1) && !display) {
            __delay_ms(50);
            MAX7219_Display(1);
            Set_Gain(gain);
        }
        else if ((gain == 2) && !display) {
            __delay_ms(50);
            MAX7219_Display(10);
            Set_Gain(gain);
        }
        else if ((gain == 3) && !display) {
            __delay_ms(50);
            MAX7219_Display(100);
            Set_Gain(gain);
        }
        
        // Begin taking voltage readings if Start button is pressed
        if (start) 
        {
            int count = 1;
            uint32_t total = 0;
            uint32_t buff;
            
            while (START_BTN_GetValue() != 1) 
            {
                MAX7219_Display(total);
                total = 0;
                while (count <= 100*Get_Integ(hard_integ)) 
                {
                    __delay_ms(5);
                    if(count%100 == 0) total += ADC_Read();
                    count++;
                }
                if(total > 65535) total = 65535;
                count = 1;
            }
            while (START_BTN_GetValue()) {}
            start = 0;
            __delay_ms(50);
        }
    
        /*---------------------Reading PC Inputs------------------------------*/
        if(UART1_IsRxReady()) 
        {
        
        if(UART1_Read() == 'S')  // sequence is started when an "S" is received
        {
        
        getStringU1(comand, sizeof(comand));    // read a a command
        
        // variables used to store commands
        char SSMODE[10];
        char SGAIN[10];
        char SI[10];
        char SCOUNT[10];
        char SM[10];
        char SEND[10];

        // format commands
        sprintf(SSMODE, "%.*s", 5, comand );
        sprintf(SGAIN, "%.*s", 4, comand);
        sprintf(SI, "%.*s", 1, comand);
        sprintf(SCOUNT, "%.*s", 5, comand);
        sprintf(SM, "%.*s", 1, comand);
        sprintf(SEND, "%.*s", 3, comand);
        
        // perform action based on received command
        if(strcmp(SSMODE, "SMODE") == 0)    // verify connection
        {
            //MODE_SetHigh();
            printf("!");
        }
        else if(strcmp(SGAIN, "GAIN") == 0) // set gain
        {
            char gain[1];
            sprintf(gain, "%.*s", 1, comand + 4);
            
            if(gain[0] > 48 && gain[0] < 52) 
            {
                if(atoi(gain) == 3)
                {
                    Set_Gain(3);
                    printf("!");
                }
                else if(atoi(gain) == 2)
                {
                    Set_Gain(2);
                    printf("!");
                }
                else
                {
                    Set_Gain(1);
                    printf("!");
                }
            }
            else 
            {
                Set_Gain(1);
            }
        }
        else if(strcmp(SI, "I") == 0)   // set integration
        {
            char integration[4];
            sprintf(integration, "%.*s", 4, comand + 1);
            
            if(atoi(integration) > 0 && atoi(integration) < 10000) 
            {
                soft_integ = atoi(integration);
                printf("!");
            }
            else 
            {
                printf("!");
            }
        } 
        else if(strcmp(SCOUNT, "COUNT") == 0)   // take a single reading
        {
            uint32_t reading = 0;
            reading = ADC_Read();
            printf("C=%05" PRIu32, reading);
        }
        else if(strcmp(SM, "M") == 0)       // take "nnnn" readings
        {
            uint32_t reading = 0;
            char no_readings[4];
            sprintf(no_readings, "%.*s", 4, comand + 1);
            
            if((no_readings[0] > 47 && no_readings[0] < 58) &&
            (no_readings[1] > 47 && no_readings[1] < 58) &&
            (no_readings[2] > 47 && no_readings[2] < 58) &&
            (no_readings[3] > 47 && no_readings[3] < 58)) 
            {   
                
                for(int i = 0; i < atoi(no_readings); i++) 
                {
                    // break loop if "SS" is received
                    if(UART1_IsRxReady()) 
                    {
                        if((char)UART1_Read() == 'S') {
                            if((char)UART1_Read() == 'S') 
                            {
                                printf("!\n\r");
                                break;
                            }
                        }
                    }
                    
                    reading = ADC_Read();
                    printf("%" PRIu32 "\n\r", reading);
                }
                printf("!");
            }
            else 
            {
                continue;
            }
        }
        else if(strcmp(SEND, "END") == 0)   // end connection
        {
            printf("END !");
        }
        else 
        {
            continue;
        }
        
        printf("\n\r");
    }
    }
    }
    
         
    return 1;
       
}

void MAX7219_Initialize() 
{
    // Set to decode mode
    CS_SetLow();
    SPI2_Exchange8bit(DECODE_MODE_REG);
    SPI2_Exchange8bit(DECODE_ALL);
    CS_SetHigh();
    
    // Set brightness
    CS_SetLow();
    SPI2_Exchange8bit(INTESITY_REG);
    SPI2_Exchange8bit(BRIGHTNESS); // max brightness
    CS_SetHigh();
    
    // Set number of digits
    CS_SetLow();
    SPI2_Exchange8bit(SCAN_LIMIT_REG);
    SPI2_Exchange8bit(SCAN_ALL_DIGITS); // 5 digits
    CS_SetHigh();
    
    // Set to normal operation
    CS_SetLow();
    SPI2_Exchange8bit(SHUTDOWN_REG  );
    SPI2_Exchange8bit(NORMAL_OPERATION);
    CS_SetHigh();
    
    // Test 5 digit 7 segment display
    CS_SetLow();
    SPI2_Exchange8bit(DISPLAY_TEST_REG);
    SPI2_Exchange8bit(ENABLE_TEST_MODE);
    CS_SetHigh();
    
    // Exit test mode
    CS_SetLow();
    SPI2_Exchange8bit(DISPLAY_TEST_REG);
    SPI2_Exchange8bit(DISABLE_TEST_MODE);
    CS_SetHigh();
    
    // Clear all digits
    MAX7219_Clear();
}

void MAX7219_Display(uint32_t num) 
{
    MAX7219_Clear();
    for (int i = 5; (num > 0) || (i-5 >= 0); num /= 10, i--) 
    {
        CS_SetLow();
        SPI2_Exchange8bit(i);
        SPI2_Exchange8bit(num%10);
        CS_SetHigh();
    }
}

void MAX7219_DisplayOpt(uint32_t num, bool opt) 
{
    MAX7219_Clear();
    
    for (int i = 5; (num > 0) || (i-5 >= 0); num /= 10, i--) 
    {
        CS_SetLow();
        SPI2_Exchange8bit(i);
        SPI2_Exchange8bit(num%10);
        CS_SetHigh();
    }
    
    opt? MAX7219_DisplayChar('I'): MAX7219_DisplayChar('G');
}

void MAX7219_DisplayChar(char chr) 
{
    // turn off decode mode
    CS_SetLow();
    SPI2_Exchange8bit(DECODE_MODE_REG);
    SPI2_Exchange8bit(DISABLE_DECODE);
    CS_SetHigh();
    
    // send character to display
    CS_SetLow();
    SPI2_Exchange8bit(1);
    SPI2_Exchange8bit(MAX7219_Font[1].segs);
    CS_SetHigh();
    
    // turn on decode mode
    CS_SetLow();
    SPI2_Exchange8bit(DECODE_MODE_REG);
    SPI2_Exchange8bit(DECODE_ALL);
    CS_SetHigh();
}

void MAX7219_Clear() 
{
    for (int i = 1; i < 5; i++) {
        CS_SetLow();
        SPI2_Exchange8bit(i);
        SPI2_Exchange8bit(0x0F);
        CS_SetHigh();
    }
}

uint32_t ADC_Read() 
{
    uint32_t reading = 0;
    
    SS1_SetLow();
    reading = SPI1_Exchange16bit(0);
    SS1_SetHigh();
    __delay_ms(10);
    
    return reading;
}

void Set_Gain(int gain) 
{
    if (gain == 1) {
        GAIN_LED0_SetLow();
        GAIN_LED1_SetLow();
    }
    else if (gain == 2) {
        GAIN_LED0_SetHigh();
        GAIN_LED1_SetLow();
    }
    else if (gain == 3) {
        GAIN_LED0_SetLow();
        GAIN_LED1_SetHigh();
    }
}

int Get_Integ(int integ)
{
    if (integ == 2) { return 5; }
    else if (integ == 3) { return 10; }
    
    return 1;
}

char *getStringU1(char *s, int len)
{
    char *p = s; // copy the buffer pointer
    do
    {
        *s = (char)UART1_Read(); // wait for a new character
        //UART1_Write(*s); // echo character
        if ((*s == BACKSPACE) && (s > p))
        {
            //UART1_Write(' '); // overwrite the last character
            //UART1_Write(BACKSPACE);
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


