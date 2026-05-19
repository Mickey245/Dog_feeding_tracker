/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Lab 8
  Requires TFT LCD to be connected as follows
  P1.3 LCD Backlight - TA1.2
  P1.4 SPI SCLK      - UCB0CLK
  P1.6 SPI MOSI      - UCB0SIMO
  P1.7 SPI MISO      - UCB0SOMI
  P2.5 SPI TFT CS
  P4.7 LCD D/C


Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20201101 Initial Version
--------------------------------------------------------*/

#include "lcd.h"
#include "graphics.h"
#include "color.h"
#include "ports.h"
#include "accel.h"
int id;


void introScreen(void) {
    unsigned int i;
    clearScreen(1);
    setColor(COLOR_16_WHITE);
    drawString(2, 15, FONT_LG, "ECE 447 Lab 8");
    drawString(2, 35, FONT_SM, "Maze Game");
    setColor(COLOR_16_RED);
    drawString(2, 45, FONT_SM, "Due in one week!");
    

    //Adding more thing 6-8
    setColor(COLOR_16_DARK_ORANGE);
    drawLine(2, 60, 120, 60);

    setColor(COLOR_16_SIENNA);
    drawRect(10, 75, 70, 30);

    setColor(COLOR_16_SEA_GREEN);
    drawString(15, 85, FONT_SM, "BLUE RECT");
}

/*
 *  Needs to write data to the device using spi. We will only want to write to
 *  the device we wont worry the reads.
 */

void writeData(uint8_t data) {
    P4OUT |= LCD_DC_PIN;
    P2OUT &= ~LCD_CS_PIN;  //Active low 

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = data;
    while (UCB0STATW & UCBUSY);

    P2OUT |= LCD_CS_PIN;
}

/*
 *  Needs to write commands to the device using spi
 */

void writeCommand(uint8_t command) {
    P4OUT &= ~LCD_DC_PIN;
    P2OUT &= ~LCD_CS_PIN; 

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = command;
    while (UCB0STATW & UCBUSY);

    P2OUT |= LCD_CS_PIN;
}

void initMSP430(void) {

    /**************************** PWM Backlight ****************************/

    P1DIR   |= BIT3;
    P1SEL0  |= BIT3;
    P1SEL1  &= ~BIT3;
    TA1CCR0  = 511;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR2  = 255;
    TA1CTL   = TASSEL__ACLK | MC__UP | TACLR;

    /******************************** SPI **********************************/

    P2DIR  |=   LCD_CS_PIN;                     // DC and CS
    P4DIR  |=   LCD_DC_PIN;
    P2OUT  |=   LCD_CS_PIN;
    P4OUT  &=  ~LCD_DC_PIN;

    P1SEL0 |=   LCD_MOSI_PIN | LCD_UCBCLK_PIN;      // MOSI and UCBOCLK
    P1SEL1 &= ~(LCD_MOSI_PIN | LCD_UCBCLK_PIN);

    UCB0CTLW0 |= UCSWRST;       // Reset UCB0

    /*
     * UCBxCTLW0    - eUSCI_Bx Control Register 0
     * UCSSEL__SMCLK - SMCLK in master mode
     * UCCKPL       - Clock polarity select
     * UCMSB        - MSB first select
     * UCMST        - Master mode select
     * UCMODE_0     - eUSCI mode 3-pin SPI select
     * UCSYNC       - Synchronous mode enable
     */
    //UCB0CTLW0 |= UCSSEL__SMCLK | UCCKPL | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    UCB0CTLW0 = UCSWRST | UCSSEL__SMCLK | UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    //    UCB0CTLW0 = UCCKPH + UCMSB + UCMST + UCSYNC + UCSSEL_2; // 3-pin, 8-bit SPI master


    UCB0BR0   = 0x01;         // Clock = SMCLK/1
    UCB0BR1    = 0;
    UCB0CTLW0  &= ~UCSWRST;     // Clear UCSWRST to release the eUSCI for operation
    PM5CTL0   &= ~LOCKLPM5;    // Unlock ports from power manager

    __enable_interrupt();

}

void main(void) {
    WDTCTL = WDTPW | WDTHOLD;   // kill the watchdog

    initMSP430();

    __delay_cycles(10);
    
    initLCD();

    introScreen();

    while (TRUE) {
        _nop();
    }
}