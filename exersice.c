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

#define STATE_START 0
#define STATE_GAME  1
#define STATE_ACCEL 2
#define STATE_WIN 3


int id;

volatile unsigned int beep_time = 0;

//USing state (0 = Start state, 1 = In game, 2 = Accel coordinate screen)
int state = STATE_START;
volatile int s1_Button = 0;
volatile int s2_Button = 0;

//current ball location
int ball_x = 10;
int ball_y = 10;

// Velocity of the ball (Direction +/- and speed)
int ball_vx = 0; 
int ball_vy = 0;

//Starting Accel board should be saved and seen as zero
int base_x = 0;
int base_y = 0;

//Ball color 
unsigned int ball_color = COLOR_16_GREEN;


void introScreen(void) 
{
    unsigned int i;
	//Title
	clearScreen(1);
	setColor(COLOR_16_WHITE);
	drawString(10, 15, FONT_MD, "ECE 447 Lab 8");
	drawString(10, 35, FONT_SM, "Maze Game");
	//Button explanation 
	setColor(COLOR_16_WHITE);
	drawString(10, 65, FONT_SM, "S1: Start Game");
	drawString(10, 75, FONT_SM, "S2: Accel Values");
	//Game explanation
	setColor(COLOR_16_BLUE);
	drawString(10, 100, FONT_SM, "Tilt board to move ball!");
}

// Changes the unsigned accel values into signed  
int signed_con(unsigned int value) 
{
    int signedValue = value & 0x0FFF;

    if (signedValue & 0x0800) {
        signedValue -= 4096;
    }

    return signedValue;
}

//Resets the position of the ball and color when game starts
void init_game(void) {
    clearScreen(1);
    setBackgroundColor(COLOR_16_BLACK);
    ball_x = 10;
    ball_y = 10;
    ball_vx = 0;
    ball_vy = 0;
    ball_color = COLOR_16_GREEN;
    accel_update();
    base_x = signed_con(accel_x);
    base_y = signed_con(accel_y);


    //Maze
    setColor(COLOR_16_RED);

    drawLine(35, 20, 140, 20);
    drawLine(140, 20, 140, 60);
    drawLine(65, 45, 120, 45);
    drawLine(65, 45, 65, 80);
    drawLine(25, 80, 65, 80);
    drawLine(120, 45, 120, 95);
    drawLine(25, 100, 120, 100);
    drawLine(25, 100, 25, 120);
    drawLine(55, 115, 130, 115);

    setColor(COLOR_16_GREEN);
    fillCircle(145, 110, 7);

    //Ball
    setColor(ball_color);
    fillCircle(ball_x, ball_y, 5);
}

// Prepares Accel value into a string array so it can be wrtiten on the LCD
/*
	string[0] = x or y
	string[1] = ":"
	string[2] = either a positive or negative 
	string[3] = the thousands  placement 0xxx
	string[4] = the hundreds  placement  x0xx
	string[5] = the tens  placement		 xx0x
	string[6] = the ones  placement		 xxx0
	string[7] = Ends the string 
*/
void accel_string_make(char label, int value, char *string) 
{
    int magnitude;

    string[0] = label;
    string[1] = ':';

    if (value < 0) {
        string[2] = '-';
        magnitude = -value;
    } else {
        string[2] = '+';
        magnitude = value;
    }

    if (magnitude > 9999) {
        magnitude = 9999;
    }

    string[3] = (magnitude / 1000) % 10 + '0';
    string[4] = (magnitude / 100) % 10 + '0';
    string[5] = (magnitude / 10) % 10 + '0';
    string[6] = magnitude % 10 + '0';
    string[7] = '\0';
}

void display_accel_values(void) 
{
    char xString[] = "X:+0000";
    char yString[] = "Y:+0000";

    accel_string_make('X', signed_con(accel_x), xString);
    accel_string_make('Y', signed_con(accel_y), yString);

    setColor(COLOR_16_WHITE);
    drawString(25, 50, FONT_LG_BKG, xString);
    drawString(25, 80, FONT_LG_BKG, yString);

}

//Maze wall check:
int wall_hit(int x, int y)
{
    if ((x + 5 >= 35) && (x - 5 <= 140) && (y + 5 >= 20) && (y - 5 <= 20)) 
    {
        return 1;
    }

    if ((x + 5 >= 140) && (x - 5 <= 140) && (y + 5 >= 20) && (y - 5 <= 60)) 
    {
        return 1;
    }

    if ((x + 5 >= 65) && (x - 5 <= 120) && (y + 5 >= 45) && (y - 5 <= 45)) 
    {
        return 1;
    }

    if ((x + 5 >= 65) && (x - 5 <= 65) && (y + 5 >= 45) && (y - 5 <= 80)) 
    {
        return 1;
    }

    if ((x + 5 >= 25) && (x - 5 <= 65) && (y + 5 >= 80) && (y - 5 <= 80)) 
    {
        return 1;
    }

    if ((x + 5 >= 120) && (x - 5 <= 120) && (y + 5 >= 45) && (y - 5 <= 95)) 
    {
        return 1;
    }

    if ((x + 5 >= 25) && (x - 5 <= 120) && (y + 5 >= 100) && (y - 5 <= 100)) 
    {
        return 1;
    }

    if ((x + 5 >= 25) && (x - 5 <= 25) && (y + 5 >= 100) && (y - 5 <= 120)) 
    {
        return 1;
    }

    if ((x + 5 >= 55) && (x - 5 <= 130) && (y + 5 >= 115) && (y - 5 <= 115)) 
    {
        return 1;
    }

    return 0;
}

void updateGame(void) 
{
    int old_x;
    int old_y;
    int dv_x;
    int dv_y;
    int hit;

    accel_update();

    dv_x = signed_con(accel_x) - base_x;
    dv_y = signed_con(accel_y) - base_y;

	//If the board is move by morethan 150 form teh orginal base at the start increase or decrease the speed to one direction
    if (dv_x > 150) 
	{
        ball_vx++;
    } else if (dv_x < -150) 
	{
        ball_vx--;
    }

    if (dv_y > 150) 
	{
        ball_vy--;
    } else if (dv_y < -150) 
	{
        ball_vy++;
    }
	//Limits the speed
    if (ball_vx > 3)
	{		
		ball_vx = 3;
	}
	if (ball_vx < -3) 
	{
		 ball_vx = -3;
	}
    if (ball_vy > 3) 
	{
		ball_vy = 3;
    }
	if (ball_vy < -3) 
	{
		ball_vy = -3;
	}
		
	//saves the current board till for next reiteration 	
    old_x = ball_x;
    old_y = ball_y;

    ball_x += ball_vx;
    ball_y += ball_vy;

    hit = 0;

    if (wall_hit(ball_x, ball_y))
    {
        ball_x = old_x;
        ball_y = old_y;
        ball_vx = -ball_vx;
        ball_vy = -ball_vy;
        hit = 1;
    }
	
	//Makes sure that the ball edge hits the wall and bounce rather than the center. 
    if (ball_x <= 7) 
	{
        ball_x = 7;
        if (ball_vx < 0) 
		{
			ball_vx = -ball_vx;  //Bounce with the same speed to the right 
        }
		if (ball_vx == 0) 
		{	
			ball_vx = 2; //Bounce back to the right 
        }
		hit = 1;  //Change color flags the ball hit wall 
    }

    if (ball_x >= 152) 
	{
        ball_x = 152;
        if (ball_vx > 0) 
		{
			ball_vx = -ball_vx; //Bounce wuth the same speed to the left 
        }
		if (ball_vx == 0)
		{
			ball_vx = -2; //Bounce back to the left 
        }
		hit = 1;
    }

    if (ball_y <= 7) 
	{
        ball_y = 7;
        if (ball_vy < 0) 
		{	
			ball_vy = -ball_vy;
        }
		if (ball_vy == 0) 
		{	
			ball_vy = 2; //Bounce back to the bottom  
        }
		hit = 1;
    }

    if (ball_y >= 120) 
	{
        ball_y = 120;
        if (ball_vy > 0) 
		{
			ball_vy = -ball_vy;
		}
        if (ball_vy == 0) 
		{	
			ball_vy = -2; //Bounce back to the top  
		}
        hit = 1;
    }

    if (hit) 
	{
		ball_color = COLOR_16_RED;
        beep_time = 50;
    } 
	else 
	{
        ball_color = COLOR_16_GREEN;
    }

    if ((ball_x >= 138) && (ball_x <= 152) && (ball_y >= 103) && (ball_y <= 117))
    {
        clearScreen(1);
        setBackgroundColor(COLOR_16_BLACK);

        setColor(COLOR_16_GREEN);
        drawString(25, 45, FONT_LG, "YOU WIN!");

        setColor(COLOR_16_WHITE);
        drawString(10, 85, FONT_SM, "Press S2 reset");

        state = STATE_WIN;
        return;
    }

    if (old_x != ball_x || old_y != ball_y || hit) 
	{
		setColor(COLOR_16_BLACK); //Cleans the old circle (Same color as the background)
		fillCircle(old_x, old_y, 5);
		
        setColor(COLOR_16_RED);
        drawLine(35, 20, 140, 20);
        drawLine(140, 20, 140, 60);
        drawLine(65, 45, 120, 45);
        drawLine(65, 45, 65, 80);
        drawLine(25, 80, 65, 80);
        drawLine(120, 45, 120, 95);
        drawLine(25, 100, 120, 100);
        drawLine(25, 100, 25, 120);
        drawLine(55, 115, 130, 115);


        setColor(COLOR_16_GREEN);
        fillCircle(145, 110, 7);

        setColor(ball_color);
		fillCircle(ball_x, ball_y, 5);
    }
}



/*
 *  Needs to write data to the device using spi. We will only want to write to
 *  the device we wont worry the reads.
 */

void writeData(uint8_t data) {
    P4OUT |= LCD_DC_PIN;
    P2OUT &= ~LCD_CS_PIN;

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

    P1DIR &= ~(BIT1 | BIT2);
    P1REN |= BIT1 | BIT2;
    P1OUT |= BIT1 | BIT2;
    P1SEL0 &= ~(BIT1 | BIT2);
    P1SEL1 &= ~(BIT1 | BIT2);
    P1IES |= BIT1 | BIT2;
    P1IFG &= ~(BIT1 | BIT2);
    P1IE  |= BIT1 | BIT2;

    //Buzzer inti
    P1DIR |= BIT5;
    P1OUT &= ~BIT5;
    TA0CTL = TASSEL__SMCLK | MC__UP | TACLR;
    TA0CCR0 = 1000;
    TA0CCTL0 = CCIE;

    __enable_interrupt();

}

//Button ISR:

#pragma vector=PORT1_VECTOR
__interrupt void Port_1(void) {
    if (P1IFG & BIT1) {
        s1_Button = 1;
        P1IFG &= ~BIT1;
    }

    if (P1IFG & BIT2) {
        s2_Button = 1;
        P1IFG &= ~BIT2;
    }
}

#pragma vector = TIMER0_A0_VECTOR
__interrupt void TA0_ISR(void)
{
    if (beep_time > 0)
    {
        P1OUT ^= BIT5;
        beep_time--;
    }
    else
    {
        P1OUT &= ~BIT5;
    }
}

void main(void)
{
	WDTCTL = WDTPW | WDTHOLD;
	
    initMSP430();
	
    __delay_cycles(80000);
	
    //accel_init();
    //id = accel_setup();
    initLCD();
    introScreen();

	//Different state, will do different functions 
	while (TRUE) 
    {
        if (state == STATE_START) 
		{
            if (s1_Button) 
			{
                s1_Button = 0;
                __delay_cycles(80000);
                state = STATE_GAME;
                init_game();
            } 
			else if (s2_Button) 
			{
                s2_Button = 0;
                __delay_cycles(80000);
                state = STATE_ACCEL;
				
                clearScreen(1);
				setBackgroundColor(COLOR_16_BLACK);
                accel_update();
                display_accel_values();
            }
        } 
		
		else if (state == STATE_GAME) 
		{
            if (s2_Button) 
			{
                s2_Button = 0;
                __delay_cycles(100000);
                state = STATE_START;
                introScreen();
            } 
			else 
			{
                updateGame();
                __delay_cycles(100000);
            }
        } 
		
		else if (state == STATE_ACCEL) 
		{
            if (s2_Button) {
                s2_Button = 0;
                __delay_cycles(80000);
                state = STATE_START;
                introScreen();
            } else {
                accel_update();
                display_accel_values();
                __delay_cycles(80000);
            }
        }
        else if (state == STATE_WIN)
        {
            if (s2_Button)
            {
                s2_Button = 0;
                __delay_cycles(80000);
                state = STATE_START;
                introScreen();
            }

        }
    }
}

