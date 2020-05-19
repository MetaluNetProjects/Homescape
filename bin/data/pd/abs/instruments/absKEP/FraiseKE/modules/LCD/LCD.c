/*********************************************************************
 *
 *                LCD for Fraise
 *
 *********************************************************************
 * Author               Date        Comment
 *~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Antoine Rousseau  oct 2013     Original.
 ********************************************************************/

/*
# This program is free software; you can redistribute it and/or
# modify it under the terms of the GNU General Public License
# as published by the Free Software Foundation; either version 2
# of the License, or (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.

# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
# MA  02110-1301, USA.
*/
#include <core.h>
#include <LCD.h>

byte LCDbuf[LCDBUFLEN];
byte LCDbufin=0;
byte LCDbufout=0;

void LCD_putchar(byte c)
{
	byte in = LCDbufin + 1;
	
	if(in == sizeof(LCDbuf)) in = 0;
	if(in == LCDbufout) {
		printf("C!LCD overload !\n");
		return; // ring buffer overload !!
	}
	//printf("CLCDbuf got %d\n",c);
	LCDbuf[LCDbufin] = c;
	LCDbufin = in;
}

//#define RING_GETBYTE(buf,dest) do { dest = ##buf##[ ##buf##out ] ; ##buf##out ++ ; if( ##buf##out == sizeof(##buf##)) ##buf##out = 0 ; } while(0)
#define RING_GETBYTE(buf,dest) do { dest = buf[ buf##out ] ; buf##out ++ ; if( buf##out == sizeof(buf)) buf##out = 0 ;} while(0)

void LCD_sendnibble(byte lcd,byte b)
{
	DigitalWrite(LCD_D0, b&1);
	DigitalWrite(LCD_D1, b&2);
	DigitalWrite(LCD_D2, b&4);
	DigitalWrite(LCD_D3, b&8);
	
	// pulse E:
	if(lcd & 1) DigitalSet(LCD_E2);
	else DigitalSet(LCD_E1);
	
	Nop();Nop();Nop();Nop();
	
	if(lcd & 1) DigitalClear(LCD_E2);
	else DigitalClear(LCD_E1);	
}

void LCD_sendchar(byte lcd,byte b)
{
	LCD_sendnibble(lcd,b>>4);
	//FraiseService();
	LCD_sendnibble(lcd,b);
}

void LCD_Service(void)
{
	static BYTE b;
	static byte lcd=0; // bit 0 : which lcd ; bit 7 : if 1, next byte is control byte
	static unsigned long nexttime=0;
	
	if(nexttime == 0) nexttime = GetTime();
	if(Elapsed(nexttime)>0xFFFFFFF) return ; // not yet
	
	if(LCDbufin == LCDbufout) {
		nexttime = GetTime();
		return ; //buffer is empty
	}
	
	RING_GETBYTE(LCDbuf,b._byte);
	
	if(((lcd & 128) == 0) && (b._byte > 127)) {
		lcd = b._byte & 129 ;
		return ;
	}
	
	if(lcd & 128) {
		DigitalClear(LCD_RS);
	} else DigitalSet(LCD_RS);
	
	LCD_sendchar(lcd,b._byte);
	
	if((lcd & 128) && ((b._byte & 0b11111100) == 0)) nexttime = GetTime() + Micros(1520);
	else nexttime = GetTime() + Micros(38);
	
	lcd &= 127; // clear eventual control bit.nexttime
}

#define WAIT(t,us) do{ t = GetTime(); while(Elapsed(t) < Micros(us)); } while(0)

void LCD_Init(void)
{
	unsigned long t;
	
	SetPinDigiOut(LCD_D0);  
	SetPinDigiOut(LCD_D1);  
	SetPinDigiOut(LCD_D2);  
	SetPinDigiOut(LCD_D3);  
	SetPinDigiOut(LCD_E1);  
	SetPinDigiOut(LCD_E2);  
	SetPinDigiOut(LCD_RS);  
	  
	WAIT(t,15000);
	DigitalClear(LCD_RS);
	LCD_sendnibble(0,0b0011);
	LCD_sendnibble(1,0b0011);
	WAIT(t,5000);
	LCD_sendnibble(0,0b0011);
	LCD_sendnibble(1,0b0011);
	WAIT(t,200);
	LCD_sendchar(0,0b00110010); // 4 bits interface
	LCD_sendchar(1,0b00110010); // 4 bits interface
	WAIT(t,40);
	LCD_sendchar(0,0b00101000); // set nblines/font
	LCD_sendchar(1,0b00101000); // set nblines/font
	WAIT(t,40);
	LCD_sendchar(0,0b00010000); // off
	LCD_sendchar(1,0b00010000); // off
	WAIT(t,40);
	LCD_sendchar(0,0b00000001); // clear
	LCD_sendchar(1,0b00000001); // clear
	WAIT(t,1600);
	LCD_sendchar(0,0b00000110); // incr, no shift
	LCD_sendchar(1,0b00000110); // incr, no shift
	WAIT(t,40);
	LCD_sendchar(0,0b00001100); // no cursor, no blink
	LCD_sendchar(1,0b00001100); // no cursor, no blink
	WAIT(t,40);	
}

