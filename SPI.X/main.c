/**
  Generated main.c file from MPLAB Code Configurator

  @Company
    Microchip Technology Inc.

  @File Name
    main.c

  @Summary
    This is the generated main.c using PIC24 / dsPIC33 / PIC32MM MCUs.

  @Description
    This source file provides main entry point for system initialization and application code development.
    Generation Information :
        Product Revision  :  PIC24 / dsPIC33 / PIC32MM MCUs - 1.171.1
        Device            :  PIC24FV32KA302
    The generated drivers are tested against the following:
        Compiler          :  XC16 v1.70
        MPLAB 	          :  MPLAB X v5.50
*/

/*
    (c) 2020 Microchip Technology Inc. and its subsidiaries. You may use this
    software and any derivatives exclusively with Microchip products.

    THIS SOFTWARE IS SUPPLIED BY MICROCHIP "AS IS". NO WARRANTIES, WHETHER
    EXPRESS, IMPLIED OR STATUTORY, APPLY TO THIS SOFTWARE, INCLUDING ANY IMPLIED
    WARRANTIES OF NON-INFRINGEMENT, MERCHANTABILITY, AND FITNESS FOR A
    PARTICULAR PURPOSE, OR ITS INTERACTION WITH MICROCHIP PRODUCTS, COMBINATION
    WITH ANY OTHER PRODUCTS, OR USE IN ANY APPLICATION.

    IN NO EVENT WILL MICROCHIP BE LIABLE FOR ANY INDIRECT, SPECIAL, PUNITIVE,
    INCIDENTAL OR CONSEQUENTIAL LOSS, DAMAGE, COST OR EXPENSE OF ANY KIND
    WHATSOEVER RELATED TO THE SOFTWARE, HOWEVER CAUSED, EVEN IF MICROCHIP HAS
    BEEN ADVISED OF THE POSSIBILITY OR THE DAMAGES ARE FORESEEABLE. TO THE
    FULLEST EXTENT ALLOWED BY LAW, MICROCHIP'S TOTAL LIABILITY ON ALL CLAIMS IN
    ANY WAY RELATED TO THIS SOFTWARE WILL NOT EXCEED THE AMOUNT OF FEES, IF ANY,
    THAT YOU HAVE PAID DIRECTLY TO MICROCHIP FOR THIS SOFTWARE.

    MICROCHIP PROVIDES THIS SOFTWARE CONDITIONALLY UPON YOUR ACCEPTANCE OF THESE
    TERMS.
*/

/**
  Section: Included Files
*/
#include "mcc_generated_files/system.h"
#include "mcc_generated_files/mcc.h"
#include <libpic30.h>

int Digit(short unsigned int num ){ // To light the specific digit on
    short unsigned int digi = num % 10;
    short unsigned int code = 0;
    switch(digi){
        case 0:
            code = 175;
            break;
        case 1:
            code = 6;
            break;
        case 2:
            code = 203;
            break;
        case 3:
            code = 79;
            break;
        case 4:
            code = 102;
            break;
        case 5:
            code = 109;
            break;
        case 6:
            code = 253;
            break;
        case 7:
            code = 7;
            break;
        case 8:
            code = 239;
            break;
        case 9:
            code = 103;
            break;
        default:
            code = 175;
            break;
    }
    return code;
}

int DigitChoose(short unsigned int code, short unsigned int i){ //To choose the digit
    switch(i){
        case 0:
            code = code+256;
            break;
        case 1:
            code = code+512;
            break;
            
        case 2:
            LATA = 1;
            break;
        case 3:
            LATA = 2;
            break;
        case 4:
            code = code+4096;
            break;        
    }
    return code;
}


void codeDisplay(short unsigned int ADCNum){  // To Display the entire number
    short unsigned int count = 0;
    unsigned int digi;
    short unsigned int code;
    short unsigned int final;
    
    while(ADCNum != 0){
        digi = ADCNum % 10;
        ADCNum = ADCNum / 10;
        code = Digit(digi);
        final = DigitChoose(code, count);
        count++;
        
        LATB = final;
        __delay_ms(1);
        LATA = 0;
    }
    return 0;
}

/*
                         Main application
 */
int main(void)
{
    // initialize the device
    
    
    SYSTEM_Initialize();
    uint16_t MY_BUFFER_SIZE = 16;
    
    TRISBbits.TRISB0 = 0;
    TRISBbits.TRISB1 = 0;
    TRISBbits.TRISB2 = 0;
    TRISBbits.TRISB3 = 0;
    TRISBbits.TRISB5 = 0;
    TRISBbits.TRISB6 = 0;
    TRISBbits.TRISB7 = 0;
    TRISBbits.TRISB8 = 0;
    TRISBbits.TRISB9 = 0;
    TRISBbits.TRISB12 = 0;
    
    TRISAbits.TRISA0 = 0;
    TRISAbits.TRISA1 = 0;
    while (1)
    {
    uint16_t   myWriteBuffer[MY_BUFFER_SIZE];
    uint16_t   myReadBuffer[MY_BUFFER_SIZE];
    uint16_t writeData;
    uint16_t readData;
    SPI1_STATUS status;
    unsigned int total;
    SPI1_Initialize();
    total = 0;
    unsigned int numberOfBytesFactor = 2;
    do
    {
        total  = SPI1_Exchange16bitBuffer( &myWriteBuffer[total], (MY_BUFFER_SIZE - total)*numberOfBytesFactor, &myReadBuffer[total]);
        //codeDisplay(total);
        

    } while( total < MY_BUFFER_SIZE );

    readData = SPI1_Exchange16bit(writeData);
    for(int i = 0; i < 500; i++){
        codeDisplay(readData);
    }
    readData = 0;

    status = SPI1_StatusGet();
    
    }

    return 1;
}
/**
 End of File
*/

