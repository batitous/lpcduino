/* 
 * File:   shared-pinout.h
 * Author: baptiste
 *
 * Created on 15 septembre 2015, 18:20
 */

#ifndef SHARED_PINOUT_H
#define	SHARED_PINOUT_H

#ifdef	__cplusplus
extern "C" {
#endif

/** Digital input / output */
#define D0      GPIO0_24
#define D1      GPIO0_25
#define D2      GPIO0_23
#define D3      GPIO0_22
#define D4      GPIO0_1
#define D5      GPIO0_2
#define D6      GPIO0_0
#define D7      GPIO0_14
#define D8      GPIO0_9
#define D9      GPIO0_10
#define D10     GPIO0_11
#define D11     GPIO0_15
#define D12     GPIO0_13
#define D13     GPIO0_17 // WakeUp
#define D14     GPIO0_19
#define D15     GPIO0_20
    

/** Analog input */
#define A0      ANA0
#define A1      ANA1
#define A2      ANA2
#define A3      ANA3
#define A4      ANA5
#define A5      ANA6
#define A6      ANA7
#define A7      ANA10

//#define A8      //TODO ANA1_1
//#define A9      //TODO ANA1_2
//#define A10     //TODO ANA1_3
//#define A11     //TODO ANA1_6

    

/** If you need more GPIO pins, you can access specific pins with the next defines
 */
    
/** Analog output */
#define DAC_OUT GPIO0_12
    
/** Uart */
#define TX      GPIO0_25
#define RX      GPIO0_24
    
/** I2C */
#define SDA     GPIO0_23
#define SCL     GPIO0_22
    
/** SPI */
#define MOSI    GPIO0_28
#define SS      GPIO0_26
#define SCK     GPIO0_29
#define MISO    GPIO0_27

/** Button Programming and Reset */
#define ISP     GPIO0_4
#define RESET   GPIO0_21
#define RST     RESET
    

#define USB_VBUS GPIO0_16  

    
#ifdef	__cplusplus
}
#endif

#endif	/* SHARED_PINOUT_H */

