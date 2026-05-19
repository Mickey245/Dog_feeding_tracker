/*--------------------------------------------------------
GEORGE MASON UNIVERSITY
ECE 447 - Accelerometer definitions
Date:   Fall 2020
Author: Jens-Peter Kaps

Change Log:
20201108 Initial Version
--------------------------------------------------------*/

#ifndef ACCEL_H_
#define ACCEL_H_

#include <stdint.h>

void accel_init();
int accel_setup();
void accel_update();

extern unsigned int accel_x;
extern unsigned int accel_y;



#define MSA301_I2CADDR_DEFAULT (0x26) /// I2C address of MSA 301
#define MSA311_I2CADDR_DEFAULT (0x62) /// I2C address of MSA 311

/*=========================================================================
            Definitions that work for both MSA 301 and MSA 311
  =========================================================================*/

#define MSA301_REG_PARTID 0x01    // Register that contains the part ID
#define MSA301_REG_OUT_X_L 0x02   // Register address for X axis lower byte
#define MSA301_REG_OUT_X_H 0x03   // Register address for X axis higher byte
#define MSA301_REG_OUT_Y_L 0x04   // Register address for Y axis lower byte
#define MSA301_REG_OUT_Y_H 0x05   // Register address for Y axis higher byte
#define MSA301_REG_OUT_Z_L 0x06   // Register address for Z axis lower byte
#define MSA301_REG_OUT_Z_H 0x07   // Register address for Z axis higher byte
#define MSA301_REG_MOTIONINT 0x09 // Register address for motion interrupt
#define MSA301_REG_DATAINT 0x0A   // Register address for data interrupt
#define MSA301_REG_CLICKSTATUS  0x0B // Register address for click/doubleclick status
#define MSA301_REG_RESRANGE 0x0F  // Register address for resolution range
#define MSA301_REG_ODR 0x10       // Register address for data rate setting
#define MSA301_REG_POWERMODE 0x11 // Register address for power mode setting
#define MSA301_REG_INTSET0 0x16   // Register address for interrupt setting #0
#define MSA301_REG_INTSET1 0x17   // Register address for interrupt setting #1
#define MSA301_REG_INTMAP0 0x19   // Register address for interrupt map #0
#define MSA301_REG_INTMAP1 0x1A   // Register address for interrupt map #1
#define MSA301_REG_TAPDUR 0x2A    // Register address for tap duration
#define MSA301_REG_TAPTH 0x2B     // Register address for tap threshold


/** The accelerometer ranges */

#define MSA301_RANGE_2_G  0x00  // +/- 2g (default value)
#define MSA301_RANGE_4_G  0x01  // +/- 4g
#define MSA301_RANGE_8_G  0x02  // +/- 8g
#define MSA301_RANGE_16_G 0x03  // +/- 16g

/** The accelerometer read resolution */

#define MSA301_RESOLUTION_14 0x00 // 14-bit resolution
#define MSA301_RESOLUTION_12 0x04 // 12-bit resolution
#define MSA301_RESOLUTION_10 0x08 // 10-bit resolution
#define MSA301_RESOLUTION_8  0x0C //  8-bit resolution


/** The accelerometer axes */
#define MSA301_AXIS_X BIT7 // X axis bit
#define MSA301_AXIS_Y BIT6 // Y axis bit
#define MSA301_AXIS_Z BIT5 // Z axis bit

/** The accelerometer data rate */
#define MSA301_DATARATE_1_HZ     0x00 //     1 Hz
#define MSA301_DATARATE_1_95_HZ  0x01 //     1.95 Hz
#define MSA301_DATARATE_3_9_HZ   0x02 //     3.9 Hz
#define MSA301_DATARATE_7_81_HZ  0x03 //     7.81 Hz
#define MSA301_DATARATE_15_63_HZ 0x04 //    15.63 Hz
#define MSA301_DATARATE_31_25_HZ 0x05 //    31.25 Hz
#define MSA301_DATARATE_62_5_HZ  0x06 //    62.5 Hz
#define MSA301_DATARATE_125_HZ   0x07 //   125 Hz
#define MSA301_DATARATE_250_HZ   0x08 //   250 Hz
#define MSA301_DATARATE_500_HZ   0x09 //   500 Hz
#define MSA301_DATARATE_1000_HZ  0x0A //  1000 Hz


/** The accelerometer bandwidth */
#define MSA301_BANDWIDTH_1_95_HZ  0x00 //    1.95 Hz
#define MSA301_BANDWIDTH_3_9_HZ   0x06 //    3.9 Hz
#define MSA301_BANDWIDTH_7_81_HZ  0x08 //    7.81 Hz
#define MSA301_BANDWIDTH_15_63_HZ 0x0A //   15.63 Hz
#define MSA301_BANDWIDTH_31_25_HZ 0x0C //   31.25 Hz
#define MSA301_BANDWIDTH_62_5_HZ  0x0E //   62.5 Hz
#define MSA301_BANDWIDTH_125_HZ   0x10 //  125 Hz
#define MSA301_BANDWIDTH_250_HZ   0x12 //  250 Hz
#define MSA301_BANDWIDTH_500_HZ   0x14 //  500 Hz

/** The accelerometer power mode */
#define MSA301_NORMALMODE   0x00 // Normal (high speed) mode
#define MSA301_LOWPOWERMODE 0x40 // Low power (slow speed) mode
#define MSA301_SUSPENDMODE  0x80 // Suspend (sleep) mode



#endif /* ACCEL_H_ */
