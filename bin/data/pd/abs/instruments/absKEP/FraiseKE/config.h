#ifndef _CONFIG_H_
#define _CONFIG_H_

//DMX config:
//#define DMX_UART_PORT 	2
//#define DMX_UART_PIN	LATBbits.LATB6
//#define DMX_UART_TRIS 	TRISBbits.TRISB6
//#define DMX_NBCHAN 		128

//#define	NOCLEAN

#define ANALOG_FILTER 3
#define ANALOG_THRESHOLD 8
#define ANALOG_MINMAX_MARGIN 100

#define I2CMASTER_PORT 2

#define LCD_D0 PA1
#define LCD_D1 PA2
#define LCD_D2 PA3
#define LCD_D3 PA4
#define LCD_E2 PA5
#define LCD_E1 PA6
#define LCD_RS PA7

#define LCDBUFLEN 128

#define INCA1 PA1
#define INCA2 PA2
#define INCB1 PA3
#define INCB2 PA4

#endif // _CONFIG_H_

