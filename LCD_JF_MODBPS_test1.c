

#pragma config LVP = OFF
#pragma config WDT = OFF
#pragma config OSC = HS   
#pragma config PWRT = OFF   // Power on delay 

#include <p18F4480.h>
//#include <can2510.h>
#include <spi.h>
#include <stdlib.h>
#include "ECANPoll.h"
#include <usart.h>
#include <stdio.h>
#include <i2c.h>
#include <string.h>
#include <delays.h>
#include "functions_Serial_JF.h"
#include "LCD_JF_BPS.def"


#define RSTART SSPCON2bits.RSEN   //repeated startbit
#define START SSPCON2bits.SEN		//start bit
#define STOP SSPCON2bits.PEN 		//stop bit
#define ACK SSPCON2bits.ACKSTAT	//!ACK BIT  
#define SSPIF PIR1bits.SSPIF  
 
void i2c_busy(unsigned char); 

void wait_sspif(void);

void startSerialPort(void);

void starti2cPort(void);

void clearDisplay(void);

void writeLineToDisplay(unsigned char *line, unsigned int linenum);

void monitorStartup(void);

void startCAN(void);



void memcpy_reduced(void *output, void *input);

void checkMessages(unsigned int* voltage, unsigned char* temp, long* current);

void checkValues(unsigned int* voltage, unsigned int*VoltageHigh, unsigned int* VoltageLow, unsigned int* VoltageAverage);
/*
unsigned int findHighVoltage(unsigned int *VoltageArray, unsigned int *ArrayLength);
unsigned int findLowVoltage(unsigned int *VoltageArray, unsigned int *ArrayLength);
unsigned int findAverageVoltage(unsigned int *VoltageArray, unsigned int *ArrayLength);
void requestVoltages(unsigned int *VoltageArray, unsigned int *ArrayLength);//*/

	#define INTMAX	65535
	#define	COUNT	32
	//variable used to test the display with no other boards connected
	int testing = 1; // 1 means testing
	unsigned char line[21] = "                    \0";
    unsigned char  cool[20] =   "johnBroadbent       ";
	unsigned char  notcool[20] ="garyBroadbent       ";
	
	 

void main (void)
{
	unsigned int VoltageHigh;
	unsigned int VoltageLow;
	unsigned int VoltageAvg;
	unsigned int voltageArray[COUNT];
	unsigned char tempArray[COUNT]; 
	unsigned char TempHigh;
	long current = 0;                    
//	unsigned char line[21] = "                    \0";	//21 allows for use of termination bit when printing to serial port with no interference when printing to i2c port
	//openSerialPort();
	
    startCAN();

	TRISB |= 0b00001000;//set the CAN input (RB3) to input
	TRISC &= 0b11111011;//set the CAN output (RB2) to output
	//ECANInitialize();																							    // defined ECANPoll.c
  	CIOCONbits.ENDRHI =1;
	starti2cPort();
	Delay10KTCYx(0);
	sprintf(line, "   Welcome to the   ");
	writeLineToDisplay(line, 0);
	sprintf(line, "    UK Solar Car    ");
	writeLineToDisplay(line, 1);
	sprintf(line, "  GATO DEL SOL IV!  ");
	writeLineToDisplay(line, 2);
	sprintf(line, "                    ");
	writeLineToDisplay(line, 3);
	Delay10KTCYx(0); //delays 213.3 ms
	Delay10KTCYx(0); //delays 213.3 ms
	Delay10KTCYx(0); //delays 213.3 ms
	Delay10KTCYx(0); //delays 213.3 ms
	//Delay10KTCYx(0); //delays 213.3 ms	

	//while(1)
	//;
	//*
	//sprintf(line, "    Starting Car    ");
	//writeLineToDisplay(line, 0);
	//printf("%s\n\r", line);
	//monitorStartup();//*/
	clearDisplay();
	while(1)
	{//*
		clearDisplay();
		checkMessages(voltageArray, tempArray, &current);
		//printf("Voltage[0] = %d\r\nVoltage[1] = %d\r\n", voltageArray[0], voltageArray[1]);
		checkValues(voltageArray, &VoltageHigh, &VoltageLow, &VoltageAvg);
		
		sprintf(line, "Batt High: %01d.%02d     ", (int)(VoltageHigh/100), (int)(VoltageHigh%100));
		//sprintf(line, "Batt High: %07d       ",VoltageHigh);
		writeLineToDisplay(line, 0);
		//printf("%s\n\r", line);
		
		sprintf(line, "Batt Avg: %01d.%02d      ", (int)(VoltageAvg/100), (int)(VoltageAvg%100));
		writeLineToDisplay(line, 1);
		//printf("%s\n\r", line);
		
		sprintf(line, "Batt Low: %01d.%02d      ", (int)(VoltageLow/100), (int)(VoltageLow%100));
		writeLineToDisplay(line, 2);
		//printf("%s\n\r", line);
		
		if(current>=0) //if it is positive
			sprintf(line, "Batt Current:  %02ld.%02ld", (long)(current/100), (long)(current%100));
		else //if it is negative put the negative sign in front
			sprintf(line, "Batt Current: -%02d.%02d", (long)(-current/100), (long)(-current%100));

	//	writeLineToDisplay(line, 3);
		Delay10KTCYx(0);
	}	
	return;
}	


//write the given line on the given line number (starts at 0)
void writeLineToDisplay(unsigned char *line, unsigned int linenum)
{
unsigned int j;
	SSPADD|=0x56;			// send address
	i2c_busy(0x50);

	SSPBUF = 0xFE;
	wait_sspif();
	SSPBUF = 0x45;
	wait_sspif();
	if (linenum == 0)
		SSPBUF = 0x00;
	else if (linenum == 1)
		SSPBUF = 40; //0x40;
	else if (linenum == 2)
		SSPBUF = 20;//0x14;
	else if (linenum == 3)
		SSPBUF = 60; //0x54;
	else return;
	wait_sspif();
	
	for( j=0;j<20;j++)
		{
			//printf("\rstep 5\n");
				//might have to convert character to is acii code
			SSPBUF = (unsigned char)line[j];  //displays ascii code
			wait_sspif();	
		}
		
	StopI2C();

	return;
}

void startSerialPort(void)
{
	//setup serial port
	OpenUSART( USART_TX_INT_OFF &
	USART_RX_INT_OFF &
	USART_ASYNCH_MODE &
	USART_EIGHT_BIT &
	USART_CONT_RX &
	USART_BRGH_HIGH, 71);  //61 calculated for 19200, oscillator 20 Mhz

	printf("\rSerial Port Online\n");
	
	
	
	return;

}

void starti2cPort(void)
{
	TRISC = 0xFF;
	TRISC &= 0b11111011;//set the CAN output (RB2) to output
	SSPCON1 = 0b00111000;
	SSPADD|=0x56;			// set speed

	i2c_busy(0x50);



	SSPBUF = 0xFE;
	wait_sspif();
	SSPBUF = 0x51;		//clears display
	wait_sspif();
	
	StopI2C();	

	return;
	
}
	
void clearDisplay(void)
{
	SSPADD|=0x56;			// send address

	i2c_busy(0x50);

	SSPBUF = 0xFE;
	wait_sspif();
	SSPBUF = 0x51;		//clears display
	wait_sspif();

	StopI2C();
}
	
void i2c_busy(unsigned char x)
{
	// JF	actually starts the i2c interface
	START = 1;				// start bit
	while(START);				// wait until start bit finished

	while (1) {
		SSPIF = 0;
		SSPBUF = x;			// send write command
		wait_sspif();		// wait until command finished

		if (ACK) {		// check ACK, if 1, ACK not received
			RSTART = 1;		// then sent start bit again
			while (RSTART);	// wait until start bit finished
		}
		else
			return;
	}
}

void wait_sspif(void)
{
	unsigned char loop_hi = 9;
	unsigned char loop_lo = 0xC4;
	SSPIF = 0;
	while (!SSPIF && loop_hi) {
		loop_lo--;
		if(!loop_lo)
			loop_hi--;
	}
	SSPIF = 0;
}

//watch the BPS startup signals
void monitorStartup(void)
{

	//function variables
	unsigned char line[21] = "                    ";
	line[21] = '\0'; //terminate the string

	
	sprintf(line[0], "  Waiting on BPS 1  ");
	writeLineToDisplay(line[0], 0);
	printf("\r%s\n", line[0]);
	//wait for master to run checks
	
		clearDisplay();

		sprintf(line, "Congratulations     ");
		writeLineToDisplay(line, 0);
		printf("\r%s\n", line);
		
		sprintf(line, "you have started    ");
		writeLineToDisplay(line, 1);
		printf("\r%s\n", line);
		
		sprintf(line, "the UK Solar Car.   ");
		writeLineToDisplay(line, 2);
		printf("\r%s\n", line);
		
		sprintf(line, "  GATO DEL SOL IV   ");
		writeLineToDisplay(line, 3);
		printf("\r%s\n", line);
	
//	Delay10KTCYx(250);
}

void startCAN(void)
{
	//ensure proper TRIS settings
	TRISB |= 0b00001000;//set the CAN input (RB3) to input
	TRISC |= 0b00000000;//set the CAN output (RB2) to output
	TRISC &= 0b11111011;//set the CAN output (RB2) to output	
	//actually start the CAN module
	ECANInitialize();
}

void checkMessages(unsigned int* voltageArray, unsigned char* tempArray, long* current)
{
	unsigned long int i;
	unsigned int voltage;
	unsigned char temp;
	unsigned int received_CAN_ID;
	long * newcurrent;
	//can receiving variables
	char messageReceived = 0;
	char dataReceived[8]={0,0,0,0,0,0,0,0};	//maximum length that can be recieved
	char lengthReceived = 0, flagsReceived = 0;
	unsigned long addressReceived = 0;
	messageReceived = 0;
	//LATCbits.LATC3 = ~LATCbits.LATC3;
	messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);

	//while(!messageReceived)
	//{
	//	writeLineToDisplay(notcool, 1);
	//}
	while(messageReceived == 1)
	{
    	//writeLineToDisplay(cool, 2);
		//Delay10KTCYx(0);

		messageReceived = 0;
		//if it has the reading bit set
		//printf("CAN_ID = 0x%.3lx\r\n", addressReceived);
		
		if(((MASK_BPS_READING | MASK_BPS_SLAVE) | addressReceived) == (MASK_BPS_READING | MASK_BPS_SLAVE))
		{
			//just behave as if it is a slave and print out the information received
			received_CAN_ID = (addressReceived & MASK_BPS_SLAVE) >> 2;
			//memcpy(&voltage, dataReceived, 2);	//requires string.h
			memcpy_reduced(&voltage, dataReceived);
			temp = dataReceived[2];
			//printf("V[%.2d]=%u\n\r", received_CAN_ID, voltage);
			//printf("T[%.2d]=%.2d\n\r", received_CAN_ID, temp);
			voltageArray[received_CAN_ID] = voltage;
			tempArray[received_CAN_ID] = temp;
		}
		else if(((MASK_BPS_MASTER | MASK_BPS_READING) == addressReceived)) //if current reading
		{
			if(lengthReceived == 4)
			{
				//memcpy_reduced(&newcurrent, dataReceived);
				//printf("BC=%d\n\r", current);
				newcurrent = (void*)dataReceived;
				current = newcurrent;
			}
		}
			
		//check for any more messages
		for(i=0; i<1000; ++i);
		messageReceived = ECANReceiveMessage(&addressReceived, &dataReceived, &lengthReceived, &flagsReceived);
	}
	return;
}



void checkValues(unsigned int* voltage, unsigned int* VoltageHigh, unsigned int* VoltageLow, unsigned int* VoltageAverage)
{
	unsigned int i = 0;
	unsigned long int average = 0;
	*VoltageAverage = 0; *VoltageHigh = 0; *VoltageLow = INTMAX; //*TempHigh = 0;
	for(i=0; i<COUNT; ++i)
	{
		//printf("voltage[i] = %d\r\n", voltage[i]);
		if(voltage[i]>*VoltageHigh) *VoltageHigh = voltage[i];
		if(voltage[i]<*VoltageLow) *VoltageLow = voltage[i];
		//if(temp[i]>*TempHigh)	*TempHigh = temp[i];
		if(average != 0)
			average = (average + voltage[i]) >> 1;//bitshift right by 1 is same as divide by 2
		else average = voltage[i];
		//printf("H = %d\n\rL = %d\r\nA = %d\r\n", *VoltageHigh, *VoltageLow,average);
	}
	*VoltageAverage = average;
	
	//set to the proper units for display on the screen (3 digits ?.??)
	*VoltageHigh = (int)(*VoltageHigh/100);
	*VoltageLow = (int)(*VoltageLow/100);
	*VoltageAverage = (int)(*VoltageAverage/100);
	return;
}

//ripoff of memcpy so that I don't have to include the string library
void memcpy_reduced(void *output, void *input)
{
	*(char *)output = *(char *)input;
	*(char *)((char *)output+1) = *(char *)((char *)input+1);
	return;
}