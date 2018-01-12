/******************************
 *     Author: Omar Gamal     *
 *         Ahmed Galal        *
 *       Mohamed Khalaf       *
 *                            *
 *   c.omargamal@gmail.com    *
 *                            *
 *    Hardware: Intel 8051    *
 *                            *
 *         12/4/2017          *
 *  Elevator simulation 8051  *
 ******************************/

#include <C8051F020.h>

//number of the current floor initially 0
//used in many things including 7-segment
//max floors = 4 --> 0, 1, 2, 3
int currentFloor = 0;
int maxFloors = 4;

//iteration counter
int i, k, t, f, s;

int Steps = 0;
int steps_left;


//flag bit for each floor to set if that floor calls up or down
//lighting a LED if calling
char buttons[2] = {0x00, 0x00};

//things we will do each time
//config the watchdog, crossbar and ports
void init_ports()
{
	//disabling watchdog timer
	WDTCN = 0xDE;
	WDTCN = 0xAD;
	
	//crossbar configurations
	XBR0 = 0x00;
	XBR1 = 0x00;
	XBR2 = 0x40;
	
	//configuring P0.0 -> P0.2 push/pull -> output pins for 7 segment
	P0MDOUT = 0xFF;
	P0 = 0x00;

	//configuring P1.0 -> P1.3 push pull to work with the stepper motor.
	P1MDOUT = 0x0F;
	P1 = 0x00;
	
	//configuring P2.0 -> P2.5 open-drain -> input pins for push buttons
	P2MDOUT = 0x00;
	P2 = 0x00;

	//P5 config as P5.0-3 input, P5.4-7 output. P5.0 as door sensor, leds for flags
	P74OUT = 0x08;
	P5 = 0x0F;
}


//configurations of the clock
void init_clk()
{
	//default internal clock, at 2MHz
	OSCICN = 0x14;
}



void delayStepper()
{
	for(t=0; t<1; t++)
	{
		for(f=0; f<150; f++)
		{
		}
	}
}


void delay()
{
	for(s=0; s<6; s++)
	{
		for(t=0; t<250; t++)
		{
			for(f=0; f<250; f++)
			{
			}
		}
	}
}


void display()
{
	switch(currentFloor)
	{
		case 0:
			P0 = 0x04;
			break;
		
		case 1:
			P0 = 0x08;
			break;
		
		case 2:
			P0 = 0x0C;
			break;
		
		case 3:
			P0 = 0x10;
			break;
		
		default:
			P0 = 0x00;
			break;
	}
}



//setting the flags if there is a call, each element holds flag for a floor
void readCalls()
{
	buttons[0] |= (P2 & 0x01);		//0 or 1
	if(buttons[0] == 1)
	{
		P5 |= 0x10;		//light led 5.7 to indicate a call
	}
	
	buttons[1] |= (P2 & 0x02);		//0 or 4
	if(buttons[1] == 2)
	{
		P5 |= 0x20;		//light led 5.6 to indicate a call
	}
}


//move a complete turn
void moveUp()
{
	steps_left = 4095;

	while(steps_left > 0)
	{
		readCalls();
  		switch(steps_left % 8)
    	{
      		case 0:
        		P1 = 0x08;
        		break;
      
      		case 1:
        		P1 = 0x0C;
        		break;
        
      		case 2:
        		P1 = 0x04;
        		break;
        
      		case 3:
        		P1 = 0x06;
        		break;
        
      		case 4:
        		P1 = 0x02;
        		break;
        
      		case 5:
        		P1 = 0x03;
        		break;
        
      		case 6:
        		P1 = 0x01;
        		break;
        
      		case 7:
        		P1 = 0x09;
        		break;
        
      		default:
        		P1 = 0x00;
        		break;
  		}
		delayStepper();
  		steps_left--;
    }
	
	currentFloor++;
	display();
	if (currentFloor == 1)
	{
		if (buttons[0] == 1)
		{
			delay();
			buttons[0] = 0;
			P5 &= 0xEF;
		}
	}

	if (currentFloor == 2)
	{
		if (buttons[1] == 2)
		{
			delay();
			buttons[1] = 0;
			P5 &= 0xDF;
		}
	}
}



void moveDown()
{
	steps_left = 0;

	while (steps_left < 4095)
	{
		display();
		readCalls();
    	switch(steps_left % 8)
    	{
      		case 0:
        		P1 = 0x08;
        		break;
      
      		case 1:
        		P1 = 0x0C;
        		break;
        
      		case 2:
        		P1 = 0x04;
        		break;
        
      		case 3:
        		P1 = 0x06;
        		break;
        
      		case 4:
        		P1 = 0x02;
        		break;
        
      		case 5:
        		P1 = 0x03;
        		break;
        
      		case 6:
        		P1 = 0x01;
        		break;
        
      		case 7:
        		P1 = 0x09;
        		break;
        
      		default:
        		P1 = 0x00;
        		break;
  		}
		delayStepper();
  		steps_left++;
	}

	currentFloor--;
	display();

	if (currentFloor == 1)
	{
		if (buttons[0] == 1)
		{
			delay();
			buttons[0] = 0;
			P5 &= 0xEF;
		}
	}

	if (currentFloor == 2)
	{
		if (buttons[1] == 2)
		{
			delay();
			buttons[1] = 0;
			P5 &= 0xDF;
		}
	}
}





//main function
void main()
{
	init_ports();
	init_clk();


	while(1)
	{
		display();
		readCalls();


		for (i = 0; i < (maxFloors - 1); ++i)
		{
			moveUp();

			while((P5 & 0x01) == 0)
			{
				delay();
			}
			readCalls();
		}
		
		delay();
		readCalls();
		delay();

		for (i = maxFloors-1; i > 0; --i)
		{
			moveDown();

			while((P5 & 0x01) == 0)
			{
				delay();
			}
			readCalls();
		}

		delay();
		readCalls();
		delay();
	}
}
