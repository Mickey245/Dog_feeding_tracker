// Simple C code using MSP430 to keep track if my dog ate his food

#include <msp430.h>
#include <stdint.h>

#include <msp430.h>
#include "lcd.h"
#include "graphics.h"
#include "color.h"
#include "ports.h"

#define STATE_START       0
#define STATE_BREAKFAST   1
#define STATE_DINNER      2
#define STATE_RESET_MSG   3

volatile int BIT1_pressed = 0;
volatile int BIT2_pressed = 0;

int state = STATE_START;

void displayState(void)
{
    clearScreen(1);
    setBackgroundColor(COLOR_16_BLACK);
    setColor(COLOR_16_WHITE);

    if (state == STATE_START)
    {
        drawString(10, 40, FONT_MD, "Press White button");
    }
    else if (state == STATE_BREAKFAST)
    {
        setColor(COLOR_16_WHITE);
        drawString(10, 40, FONT_MD, "Has not had ");
        drawString(10, 60, FONT_MD, "breakfast yet!");

    }
    else if (state == STATE_DINNER)
    {
        setColor(COLOR_16_LIGHT_BLUE);
        drawString(10, 40, FONT_MD, "Lui ate breakfast");
        setColor(COLOR_16_WHITE);
        drawString(10, 65, FONT_MD, "Waiting for dinner");
    }
    else if (state == STATE_RESET_MSG)
    {
        setColor(COLOR_16_WHITE);
        drawString(10, 40, FONT_MD, "Lui ate dinner!!");
        drawString(10, 60, FONT_MD, "Press Red button");
        drawString(10, 80, FONT_MD, "to restart!");


    }
}

void initMSP430(void)
{
    WDTCTL = WDTPW | WDTHOLD;

    // LCD backlight PWM, if your display needs it
    P1DIR   |= BIT3;
    P1SEL0  |= BIT3;
    P1SEL1  &= ~BIT3;
    TA1CCR0  = 511;
    TA1CCTL2 = OUTMOD_7;
    TA1CCR2  = 255;
    TA1CTL   = TASSEL__ACLK | MC__UP | TACLR;

    // LCD SPI pins
    P2DIR  |= LCD_CS_PIN;
    P4DIR  |= LCD_DC_PIN;
    P2OUT  |= LCD_CS_PIN;
    P4OUT  &= ~LCD_DC_PIN;

    P1SEL0 |= LCD_MOSI_PIN | LCD_UCBCLK_PIN;
    P1SEL1 &= ~(LCD_MOSI_PIN | LCD_UCBCLK_PIN);

    UCB0CTLW0 = UCSWRST | UCSSEL__SMCLK | UCCKPH | UCMSB | UCMST | UCMODE_0 | UCSYNC;
    UCB0BR0 = 0x01;
    UCB0BR1 = 0;
    UCB0CTLW0 &= ~UCSWRST;

    PM5CTL0 &= ~LOCKLPM5;

    // External button setup: P4.0 and P4.1 as inputs with pull-up resistors
    P4DIR &= ~(BIT0 | BIT1);
    P4REN |= BIT0 | BIT1;
    P4OUT |= BIT0 | BIT1;

    // Interrupt on falling edge, button press pulls pin low
    P4IES |= BIT0 | BIT1;
    P4IFG &= ~(BIT0 | BIT1);
    P4IE  |= BIT0 | BIT1;

    __enable_interrupt();
}

#pragma vector=PORT4_VECTOR
__interrupt void Port_4(void)
{
    if (P4IFG & BIT0)
    {
        BIT1_pressed = 1;
        P4IFG &= ~BIT0;
    }

    if (P4IFG & BIT1)
    {
        BIT2_pressed = 1;
        P4IFG &= ~BIT1;
    }
}

void writeData(uint8_t data) {
    P4OUT |= LCD_DC_PIN;
    P2OUT &= ~LCD_CS_PIN;

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = data;
    while (UCB0STATW & UCBUSY);

    P2OUT |= LCD_CS_PIN;
}

void writeCommand(uint8_t command) {
    P4OUT &= ~LCD_DC_PIN;
    P2OUT &= ~LCD_CS_PIN;

    while (!(UCB0IFG & UCTXIFG));
    UCB0TXBUF = command;
    while (UCB0STATW & UCBUSY);

    P2OUT |= LCD_CS_PIN;
}
void main(void)
{
    initMSP430();

    __delay_cycles(100000);

    initLCD();

    state = STATE_START;
    displayState();

    while (1){
        if (BIT1_pressed)
        {
            BIT1_pressed = 0;
            __delay_cycles(80000);   // debounce delay

            // Confirm NEXT button on P4.0 is still pressed
            if ((P4IN & BIT0) == 0)
            {
                if (state == STATE_START)
                {
                    state = STATE_BREAKFAST;
                }
                else if (state == STATE_BREAKFAST)
                {
                    state = STATE_DINNER;
                }
                else if (state == STATE_DINNER)
                {
                    state = STATE_RESET_MSG;
                }
                else if (state == STATE_RESET_MSG)
                {
                    state = STATE_RESET_MSG;
                }

                displayState();

                // Wait until NEXT button is released
                while ((P4IN & BIT0) == 0);
                __delay_cycles(80000);
            }
        }

        if (BIT2_pressed)
        {
            BIT2_pressed = 0;
            __delay_cycles(80000);   // debounce delay

            // Confirm RED RESET button on P4.1 is still pressed
            if ((P4IN & BIT1) == 0)
            {
                state = STATE_START;
                displayState();

                // Wait until RED RESET button is released
                while ((P4IN & BIT1) == 0);
                __delay_cycles(80000);
            }
        }
    }
}
