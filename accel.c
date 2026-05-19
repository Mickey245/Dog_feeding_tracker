/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Accelerometer routines
Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20201108 Initial Version
--------------------------------------------------------*/

#include "accel.h"

#include <msp430.h>

unsigned int accel_x = 0, accel_y = 0;

void accel_init() {
    // set up UCB1 for I2C

    P4SEL1 |= BIT0 | BIT1;
    P4SEL0 &= ~(BIT0 | BIT1);

    UCB1CTLW0 = UCSWRST;
    UCB1CTLW0 |= UCMST | UCMODE_3 | UCSYNC | UCSSEL__SMCLK;
    UCB1BRW = 10;
    UCB1I2CSA = MSA311_I2CADDR_DEFAULT;
    UCB1CTLW0 &= ~UCSWRST;
}

int accel_setup() {
    unsigned char id;

	// Initialize the sensor
    // set power mode and bandwidth
    // set data rate, enable X and Y axis
    // set range and resolution

    while (UCB1CTLW0 & UCTXSTP);

    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);
    UCB1TXBUF = 0x01;
    while (!(UCB1IFG & UCTXIFG0));

    UCB1CTLW0 &= ~UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);
    UCB1CTLW0 |= UCTXSTP;
    while (!(UCB1IFG & UCRXIFG0));
    id = UCB1RXBUF;
    while (UCB1CTLW0 & UCTXSTP);

    while (UCB1CTLW0 & UCTXSTP);

    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);
    UCB1TXBUF = 0x11;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1TXBUF = 0x09;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1CTLW0 |= UCTXSTP;
    while (UCB1CTLW0 & UCTXSTP);

    while (UCB1CTLW0 & UCTXSTP);

    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);
    UCB1TXBUF = 0x10;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1TXBUF = 0x09;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1CTLW0 |= UCTXSTP;
    while (UCB1CTLW0 & UCTXSTP);

    while (UCB1CTLW0 & UCTXSTP);

    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);
    UCB1TXBUF = 0x0F;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1TXBUF = 0x01;
    while (!(UCB1IFG & UCTXIFG0));
    UCB1CTLW0 |= UCTXSTP;
    while (UCB1CTLW0 & UCTXSTP);

    return id; // or number for error
}

void accel_update() {
    unsigned char x_l;
    unsigned char x_h;
    unsigned char y_l;
    unsigned char y_h;

    // X LSB
    // X MSB
    // Y LSB
    // Y MSB

    while (UCB1CTLW0 & UCTXSTP);

    UCB1CTLW0 |= UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);

    UCB1TXBUF = 0x02;
    while (!(UCB1IFG & UCTXIFG0));

    UCB1CTLW0 &= ~UCTR;
    UCB1CTLW0 |= UCTXSTT;
    while (UCB1CTLW0 & UCTXSTT);

    while (!(UCB1IFG & UCRXIFG0));
    x_l = UCB1RXBUF;

    while (!(UCB1IFG & UCRXIFG0));
    x_h = UCB1RXBUF;

    while (!(UCB1IFG & UCRXIFG0));
    y_l = UCB1RXBUF;

    UCB1CTLW0 |= UCTXSTP;

    while (!(UCB1IFG & UCRXIFG0));
    y_h = UCB1RXBUF;

    while (UCB1CTLW0 & UCTXSTP);

    accel_x = (((unsigned int)x_h) << 4) | (x_l & 0x0F);
    accel_y = (((unsigned int)y_h) << 4) | (y_l & 0x0F);
}