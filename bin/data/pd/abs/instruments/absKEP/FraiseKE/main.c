/*********************************************************************
 *
 *                KE01 for FraiseKE
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

#define BOARD FraiseKE

//#include <core.h>
//#include <fraisedevice.h>
#include <fruit.h>
#include <analog.h>
//#include <switch.h>
#include <i2c_master.h>
#include <LCD.h>
#include <PCA9655.h>
//#include <eeparams.h>

unsigned char PERIOD=255;
unsigned char t,t2=0;	

//void LowInterrupts() {}
//void HighInterrupts() {}

/*char test1[256];
char test2[256];
char test3[256];
char test4[256];
char test5[256];
char test6[256];
char test7[256];
char test8[256];
char test9[256];
char test10[256];
char test11[163];*/

BYTE Leds[2];
BYTE Boutons; //But1,(...),But6, PA8_IN,NULL
BYTE PortABoutons;
//PA8_DIRECTION(0=out,1=in)
BYTE ButLeds; //LedB1,(...)LedB5, LedV, LedR, PA8_OUT
BYTE PotLedsL,PotLedsH;

BYTE PBout, PBin, PBdir;
unsigned char IncA, oldIncA, IncB, oldIncB;

//saved in EEPROM :
char PortAMode; //0 = input(PortABoutons+incA/B) , 1 = LCD
char K1Mode; //0 = none , 1 = AN

//							 AD2 AD1 AD0
#define PCA1ADD (0x4E>>1) // VDD VDD VDD onboard
#define PCA2ADD (0x5C>>1) // VDD VDD SCL onboard
#define PCA3ADD (0x40>>1) // GND GND GND 1st 8-buttons-of-16
#define PCA4ADD (0x42>>1) // GND GND VDD 2nd 8-buttons-of-16
#define PCA5ADD (0x44>>1) // GND VDD GND 8-buttons

PCA9655 pca1, pca2, pca3, pca4, pca5;

void PotLeds_Service(void)
{
	int realdist;
	char dist,absdist,count,bit = 1,n = 0;
	static char counts[6];
	
	PotLedsH._byte = PotLedsL._byte = 0;
	while(n < 6) {
		realdist = Analog_GetDist(n);
		dist = realdist >> 7;
		//if(realdist == 0) continue;
		if(realdist > 0) dist++;
		else if(realdist < 0) dist--;
		absdist = dist < 0 ? -dist : dist;
		count = counts[n] + 1;
		if(count > absdist) {
			count = 0;
			if(dist > 0) PotLedsL._byte |= bit;
			else if(dist < 0) PotLedsH._byte |= bit;
		}
		counts[n] = count;
		n++;
		bit<<=1;
	}	
}

void I2C_Service()
{
	static unsigned char State = 0;
	//BYTE B1,B2;
	
	State++;
	switch(State) {
	case 1 : // write to pca1
		BIT_COPY(pca1.out1.b1, !ButLeds.b4);
		BIT_COPY(pca1.out1.b3, !ButLeds.b3);
		BIT_COPY(pca1.out1.b5, !ButLeds.b2);
		BIT_COPY(pca1.out1.b7, !ButLeds.b1);
		
		BIT_COPY(pca1.out2.b1, !ButLeds.b0);
		BIT_COPY(pca1.out2.b2, !PotLedsL.b1);
		BIT_COPY(pca1.out2.b3, !PotLedsH.b1);
		BIT_COPY(pca1.out2.b4, !ButLeds.b7); // PA8_OUT
		BIT_COPY(pca1.out2.b6, !ButLeds.b5);
		BIT_COPY(pca1.out2.b7, !ButLeds.b6);

		PCA9655_Write(&pca1, 3);
		break;
	case 2 : // read from pca1
		PCA9655_Read(&pca1, 3);
		
		BIT_COPY(Boutons.b4, pca1.in1.b0);
		BIT_COPY(Boutons.b3, pca1.in1.b2);
		BIT_COPY(Boutons.b2, pca1.in1.b4);
		BIT_COPY(Boutons.b1, pca1.in1.b6);
				
		BIT_COPY(Boutons.b0, pca1.in2.b0);
		BIT_COPY(Boutons.b6, pca1.in2.b4); // PA8_IN
		BIT_COPY(Boutons.b5, pca1.in2.b5);
		
		break;
	case 3 : // write to pca2 :
		BIT_COPY(pca2.out1.b0, !PotLedsL.b0);
		BIT_COPY(pca2.out1.b1, !PotLedsH.b0);
		BIT_COPY(pca2.out1.b2, !PotLedsL.b2);
		BIT_COPY(pca2.out1.b3, !PotLedsH.b2);
		BIT_COPY(pca2.out1.b4, !PotLedsH.b3);
		BIT_COPY(pca2.out1.b5, !PotLedsL.b3);
		BIT_COPY(pca2.out1.b6, !PotLedsH.b4);
		BIT_COPY(pca2.out1.b7, !PotLedsL.b4);
		
		pca2.out2._byte = PBout._byte;
		
		PCA9655_Write(&pca2, 3);
		break;
	case 4 :// read from pca2 :
		PCA9655_Read(&pca2, 3);
		PBin._byte = pca2.in2._byte;
		break;
	case 5 : // write to pca3 :
		PCA9655_Write(&pca3, 2);
		break;
	case 6 :// read from pca3 :
		PCA9655_Read(&pca3, 1);
		break;
	case 7 : // write to pca4 :
		PCA9655_Write(&pca4, 2);
		break;
	case 8 :// read from pca4 :
		PCA9655_Read(&pca4, 1);
		break;
	case 9 : // write to pca5 :
		PCA9655_Write(&pca5, 2);
		break;
	case 10 :// read from pca5 :
		PCA9655_Read(&pca5, 1);
		break;

	default :
		State = 0;
		PotLeds_Service();
	}
}

/*-----------*/
#define SendIfChanged(new, old, id) do { if(old != new) { buf[len++] = id; buf[len++] = old = new;} } while(0)
unsigned char lastIncs;
void SendBoutons(void)
{
	static byte oldboutons, oldpca3, oldpca4, oldpca5, oldPAboutons, oldLastIncs;
	static byte buf[15];
	byte len=0;
	
	buf[len++] = 'B';
	
	
	if(PortAMode == 0) {
		BIT_COPY(PortABoutons.b0, DigitalRead(PA5));
		BIT_COPY(PortABoutons.b1, DigitalRead(PA6));
		BIT_COPY(PortABoutons.b2, DigitalRead(PA7));
		BIT_COPY(PortABoutons.b3, Boutons.b6); Boutons.b6 = 1; // copy PA8...
		SendIfChanged(PortABoutons._byte, oldPAboutons, 2);
		SendIfChanged(IncA, oldIncA, 3);
		SendIfChanged(IncB, oldIncB, 4);
		SendIfChanged(lastIncs, oldLastIncs, 5);
	}

	SendIfChanged(Boutons._byte, oldboutons, 1);
	
	SendIfChanged(pca3.in1._byte, oldpca3, 10);
	SendIfChanged(pca4.in1._byte, oldpca4, 11);
	SendIfChanged(pca5.in1._byte, oldpca5, 12);
	
	if(len > 1) FrTXPacket(buf,len+1);
}

/*-----------*/
/*typedef union _BYTE2
{
    struct {
		unsigned _byte:8;    // by calling something._byte, you get the whole bitfield as 8-bit number
	};
    struct
    {
        unsigned b0:1;
        unsigned b1:1;
        unsigned b2:1;
        unsigned b3:1;
        unsigned b4:1;
        unsigned b5:1;
        unsigned b6:1;
        unsigned b7:1;
    };
} BYTE2;*/

BYTE oldIncs;
void IncrService()
{
	
	BYTE incs;
	BIT_COPY(incs.b0, DigitalRead(INCA1));
	BIT_COPY(incs.b1, DigitalRead(INCA2));
	BIT_COPY(incs.b2, DigitalRead(INCB1));
	BIT_COPY(incs.b3, DigitalRead(INCB2));
	
	if((incs.b0 != oldIncs.b0)&&(incs.b1 == oldIncs.b1)) {
		if(incs.b0 ^ incs.b1) IncA++; 
		else IncA--;
	}
	else if((incs.b1 != oldIncs.b1)&&(incs.b0 == oldIncs.b0)) {
		if(!(incs.b0 ^ incs.b1)) IncA++; 
		else IncA--;
	}

	if((incs.b2 != oldIncs.b2)&&(incs.b3 == oldIncs.b3)) {
		if(incs.b2 ^ incs.b3) IncB++; 
		else IncB--;
	}
	else if((incs.b2 == oldIncs.b2)&&(incs.b3 != oldIncs.b3)) {
		if(!incs.b2 ^ incs.b3) IncB++;
		else IncB--;
	}

	lastIncs = oldIncs._byte = incs._byte;
}

/*--------------------*/
void UpdatePortAMode()
{
	if(PortAMode == 0) {
		SetPinDigiIn(PA1); SetPinAnsel(PA1,0);
		SetPinDigiIn(PA2); 
		SetPinDigiIn(PA3); 
		SetPinDigiIn(PA4); 
		SetPinDigiIn(PA5); 
		SetPinDigiIn(PA6); SetPinAnsel(PA6,0);
		SetPinDigiIn(PA7); SetPinAnsel(PA7,0);
		//SetPinDigiIn(PA8) : PA8 is on Boutons.b6
	}
	else if(PortAMode == 1) {
		LCD_Init();
	}
		
}

void UpdateK1Mode()
{
	if(K1Mode == 1) ANALOG_SELECT(5,K1);
	else Analog_Deselect(5);
}
/*--------------------*/

void main(void)
{
	unsigned long t3;
	
	CoreInit();
	FraiseInit();
	//LCD_Init();
	
	i2cm_init(I2C_MASTER, I2C_SLEW_ON, FOSC/400000/4-1) ;		//400kHz : clock = FOSC/(4 * (SSPADD + 1)) : SSPADD=FOSC/clock/4-1
		
	Analog_Init();
	
	ANALOG_SELECT(0,POT1);
	ANALOG_SELECT(1,POT2);
	ANALOG_SELECT(2,POT3);
	ANALOG_SELECT(3,POT4);
	ANALOG_SELECT(4,POT5);
	
	
	//Switch_Init();
	
	PCA9655_Config(&pca1,PCA1ADD,0b01010101,0b00110001);
	PCA9655_Config(&pca2,PCA2ADD,0,255);
	PCA9655_Config(&pca3,PCA3ADD,255,0);
	PCA9655_Config(&pca4,PCA4ADD,255,0);
	PCA9655_Config(&pca5,PCA5ADD,255,0);
	
	t=PERIOD;
	t3=GetTime();
	EEreadMain();
	UpdatePortAMode();
	UpdateK1Mode();
	
	ButLeds._byte = 0;
	
#define SERVICE(service) do { FraiseService(); service(); IncrService(); } while (0)
	while(1){
		if(PortAMode == 1) SERVICE(LCD_Service);
		SERVICE(Analog_Service);
		//SERVICE(Switch_Service);
		if(PortAMode == 1) SERVICE(LCD_Service);
		SERVICE(I2C_Service);
/*
		FraiseService();
		FraiseService();
		LCD_Service();
		
		FraiseService();
		FraiseService();
		Analog_Service();
		
		FraiseService();
		FraiseService();
		LCD_Service();
		
		FraiseService();
		FraiseService();
		Switch_Service();
		
		FraiseService();
		FraiseService();
		LCD_Service();
		
		FraiseService();
		FraiseService();
		I2C_Service();*/
		
		if(Elapsed(t3)>Micros(5000UL)) {
			t3=GetTime();

			SendBoutons();			
				
			//if(Switch_Send()==0) 
				Analog_Send(AMODE_SCALE | AMODE_NUM | AMODE_CROSS);
			if(!t--){
				t=PERIOD;
				t2++;
				//if(t2==0) Analog_printmin();
				//else if(t2==1) Analog_printmax();
				//else 
				printf("C t2 %d %d\n", t2,(byte)DigitalRead(PA1)/*Analog_GetDist(2)>>6*/);//%d %d\n",t2,PORTB,PORTE);
			}
		}
	}

}

void CharBroadcast()
{
	/*unsigned char c;
	
	printf("Brdcst: ");
	for(c=0;c<FrRXout_len;c++) printf("%c",FrRXgetchar());
	putchar('\n');*/
}

void Broadcast()
{
	/*unsigned char c;
	
	printf("brdcst: ");
	for(c=0;c<FrRXout_len;c++) printf("%d ",FrRXgetchar());
	putchar('\n');*/
}

void CharInput()
{
	unsigned char c,c2, l;
	
	l = FrRXlen();
	c=FrRXgetchar();
	//printf("Cchar in:%c %c\n",c,c2);
	if(c=='L'){	
		//c=FrRXgetchar()-'0';
		/*DigitalWrite(Led1,(c&1)==0); 
		DigitalWrite(Led2,(c&2)==0); 
		DigitalWrite(Led3,(c&4)==0); */
		/*if(c=='0') 
			{LED=0;}
		else LED=1;*/
		//printf("C L");
		if(PortAMode==1) for(c=1;c<l;c++) {
			c2=FrRXgetchar();
			LCD_putchar(c2);
			//printf(" %d",c2);
		}
		//putchar('\n');
	}
	else if(c=='E') {
		printf("C");
		for(c=1;c<l;c++) printf("%c",FrRXgetchar());
		putchar('\n');
	}
		else if(c=='S') { //analog scaling
		c=FrRXgetchar()-'0';
		Analog_Scaling(c!=0);
		if(c==0) EEwriteMain();
	}
}

void Input()
{
	unsigned char c;//,c2;
	BYTE c2;
	//unsigned int i;
	int v;

	c=FrRXgetchar();

	switch(c) {
		PARAM_CHAR(1,t2); break;
		PARAM_CHAR(2,PERIOD); break;
		case 3:
			//MOTB_EN=0;
			//MOTB_EN=1;
			break;
		PARAM_CHAR(4, ButLeds._byte); break;
		//PARAM_CHAR(5,Leds[1]._byte); break;
		PARAM_CHAR(6, c2._byte); 
			DigitalWrite(PA1,c2.b0);
			DigitalWrite(PA2,c2.b1);
			DigitalWrite(PA3,c2.b2);
			DigitalWrite(PA4,c2.b3);
			DigitalWrite(PA5,c2.b4);
			DigitalWrite(PA6,c2.b5);
			DigitalWrite(PA7,c2.b6);
			break;

		PARAM_CHAR(10, c2._byte); pca3.out2._byte = 255 - c2._byte; break;
		PARAM_CHAR(11, c2._byte); pca4.out2._byte = 255 - c2._byte; break;
		PARAM_CHAR(12, c2._byte); pca5.out2._byte = 255 - c2._byte; break;

		PARAM_CHAR(20, PortAMode);
			UpdatePortAMode();
			EEwriteMain();
			break;
		PARAM_CHAR(21, K1Mode);
			UpdateK1Mode();
			EEwriteMain();
			break;
		PARAM_INT(100,v); Analog_Set(0,v); break;
		PARAM_INT(101,v); Analog_Set(1,v); break;
		PARAM_INT(102,v); Analog_Set(2,v); break;
		PARAM_INT(103,v); Analog_Set(3,v); break;
		PARAM_INT(104,v); Analog_Set(4,v); break;
		PARAM_INT(105,v); Analog_Set(5,v); break;
	}
}

void EEdeclareMain()
{
	Analog_declareEE();
	EEdeclareChar(&PortAMode);
	EEdeclareChar(&K1Mode);
}
