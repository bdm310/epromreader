/*
 * File:   main.c
 * Author: chell
 *
 * Created on November 3, 2017, 8:06 PM
 */

// CONFIG1L
#pragma config PLLDIV = 1       // PLL Prescaler Selection bits (No prescale (4 MHz oscillator input drives PLL directly))
#pragma config CPUDIV = OSC1_PLL2// System Clock Postscaler Selection bits ([Primary Oscillator Src: /1][96 MHz PLL Src: /2])
#pragma config USBDIV = 1       // USB Clock Selection bit (used in Full-Speed USB mode only; UCFG:FSEN = 1) (USB clock source comes directly from the primary oscillator block with no postscale)

// CONFIG1H
#pragma config FOSC = INTOSCIO_EC// Oscillator Selection bits (Internal oscillator, port function on RA6, EC used by USB (INTIO))
#pragma config FCMEN = OFF      // Fail-Safe Clock Monitor Enable bit (Fail-Safe Clock Monitor disabled)
#pragma config IESO = OFF       // Internal/External Oscillator Switchover bit (Oscillator Switchover mode disabled)

// CONFIG2L
#pragma config PWRT = OFF       // Power-up Timer Enable bit (PWRT disabled)
#pragma config BOR = ON         // Brown-out Reset Enable bits (Brown-out Reset enabled in hardware only (SBOREN is disabled))
#pragma config BORV = 3         // Brown-out Reset Voltage bits (Minimum setting 2.05V)
#pragma config VREGEN = OFF     // USB Voltage Regulator Enable bit (USB voltage regulator disabled)

// CONFIG2H
#pragma config WDT = OFF        // Watchdog Timer Enable bit (WDT disabled (control is placed on the SWDTEN bit))
#pragma config WDTPS = 32768    // Watchdog Timer Postscale Select bits (1:32768)

// CONFIG3H
#pragma config CCP2MX = ON      // CCP2 MUX bit (CCP2 input/output is multiplexed with RC1)
#pragma config PBADEN = OFF     // PORTB A/D Enable bit (PORTB<4:0> pins are configured as digital I/O on Reset)
#pragma config LPT1OSC = OFF    // Low-Power Timer 1 Oscillator Enable bit (Timer1 configured for higher power operation)
#pragma config MCLRE = ON       // MCLR Pin Enable bit (MCLR pin enabled; RE3 input pin disabled)

// CONFIG4L
#pragma config STVREN = ON      // Stack Full/Underflow Reset Enable bit (Stack full/underflow will cause Reset)
#pragma config LVP = OFF        // Single-Supply ICSP Enable bit (Single-Supply ICSP disabled)
#pragma config ICPRT = OFF      // Dedicated In-Circuit Debug/Programming Port (ICPORT) Enable bit (ICPORT disabled)
#pragma config XINST = OFF      // Extended Instruction Set Enable bit (Instruction set extension and Indexed Addressing mode disabled (Legacy mode))

// CONFIG5L
#pragma config CP0 = OFF        // Code Protection bit (Block 0 (000800-001FFFh) is not code-protected)
#pragma config CP1 = OFF        // Code Protection bit (Block 1 (002000-003FFFh) is not code-protected)
#pragma config CP2 = OFF        // Code Protection bit (Block 2 (004000-005FFFh) is not code-protected)

// CONFIG5H
#pragma config CPB = OFF        // Boot Block Code Protection bit (Boot block (000000-0007FFh) is not code-protected)
#pragma config CPD = OFF        // Data EEPROM Code Protection bit (Data EEPROM is not code-protected)

// CONFIG6L
#pragma config WRT0 = OFF       // Write Protection bit (Block 0 (000800-001FFFh) is not write-protected)
#pragma config WRT1 = OFF       // Write Protection bit (Block 1 (002000-003FFFh) is not write-protected)
#pragma config WRT2 = OFF       // Write Protection bit (Block 2 (004000-005FFFh) is not write-protected)

// CONFIG6H
#pragma config WRTC = OFF       // Configuration Register Write Protection bit (Configuration registers (300000-3000FFh) are not write-protected)
#pragma config WRTB = OFF       // Boot Block Write Protection bit (Boot block (000000-0007FFh) is not write-protected)
#pragma config WRTD = OFF       // Data EEPROM Write Protection bit (Data EEPROM is not write-protected)

// CONFIG7L
#pragma config EBTR0 = OFF      // Table Read Protection bit (Block 0 (000800-001FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR1 = OFF      // Table Read Protection bit (Block 1 (002000-003FFFh) is not protected from table reads executed in other blocks)
#pragma config EBTR2 = OFF      // Table Read Protection bit (Block 2 (004000-005FFFh) is not protected from table reads executed in other blocks)

// CONFIG7H
#pragma config EBTRB = OFF      // Boot Block Table Read Protection bit (Boot block (000000-0007FFh) is not protected from table reads executed in other blocks)

#define _XTAL_FREQ 8000000

#include <xc.h>

void init() {
    //Run the internal osc at 8MHz
    OSCCON |= 0b01110000;
    
    //Start with everything low
    PORTA = 0;
    PORTB = 0;
    PORTC = 0;
    LATA = 0;
    LATB = 0;
    LATC = 0;
    
    //Setup address lines as outputs
    TRISA &= 0b11000000;
    TRISB &= 0b00000000;
    TRISC &= 0b11111100;
    
    //Set serial TX as output
    TRISC &= 0b10111111;
    
    //Setup EUSART for 115200bps, 8 bit
    TXSTA = 0b00100100;
    RCSTA = 0b10010000;
    BAUDCON = 0b00001000;
    SPBRGH = 0;
    SPBRG = 16;
}

unsigned char hextochar(unsigned char *hex) {
    switch( *hex ) {
        case '0':
            return 0;
        case '1':
            return 1;
        case '2':
            return 2;
        case '3':
            return 3;
        case '4':
            return 4;
        case '5':
            return 5;
        case '6':
            return 6;
        case '7':
            return 7;
        case '8':
            return 8;
        case '9':
            return 9;
        case 'A':
        case 'a':
            return 10;
        case 'B':
        case 'b':
            return 11;
        case 'C':
        case 'c':
            return 12;
        case 'D':
        case 'd':
            return 13;
        case 'E':
        case 'e':
            return 14;
        case 'F':
        case 'f':
            return 15;
    }
    
    return 0xFF;
}

// Extract address bits and put them on the appropriate pins
void setaddr( unsigned int addr ) {
    LATB = (char)addr;
    addr >>= 4;
    LATA = LATA & ((char)addr | 0b11001111);
    LATA = LATA | ((char)addr & 0b00110000);
    addr >>= 6;
    LATC = LATC & ((char)addr | 0b11111100);
    LATC = LATC | ((char)addr & 0b00000011);
    addr >>= 2;
    LATA = LATA & ((char)addr | 0b11110000);
    LATA = LATA | ((char)addr & 0b00001111);
}

void main(void) {
    init();
    
    while(1) {
        __delay_ms(5000);
        
        //Roll through all addresses and output data contents at each one
        unsigned int addr = 0;
        do {
            setaddr( addr );
            addr ++;
            
            TXREG = PORTD;

            while(!TXSTAbits.TRMT) {
            }
        } while( addr );
    
        //User input loop for address testing and looping back to read
        while(1) {
            if(PIR1bits.RCIF) {
                unsigned int output = 0;
                unsigned char rec;

                rec = RCREG;
                TXREG = rec;

                if( rec == 'r' ) break;

                rec = hextochar( &rec );

                if ( rec != 0xFF ) {
                    output = 1;
                    output <<= rec;
                }

                setaddr( output );

                while(!PIR1bits.TXIF) {
                }
            }
        }
    }
    
    return;
}