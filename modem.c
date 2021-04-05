/*																www.nchip.com
 *---------------------------------------------------------------------------
 * modem.c - Modem and modem port functions, windows console version		-
 *---------------------------------------------------------------------------
 * Version                                                                  -                                            
 *		0.1 Original Version AUG 21, 2002									-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2002, nChip, All Rights Reserved.							_
 *															               	_
 *---------------------------------------------------------------------------
*/


#include "modem.h"


static U8		pushch;

#ifdef WINDOWS
#include <windows.h>
#include "wserial.h"
static HANDLE	SerHandle;
//
// Windows Init
//
S8 MODEM_init(U8 port, U16 rate)
{
#ifdef DEBUG_LV1
	printf("Initialize Modem Serial Port com %d speed %d\n",port,rate);
#endif

	pushch=0;
	if(SerHandle!=0)
		CloseSerial(SerHandle);

	if(-1==(SerHandle=OpenSerial(port,rate)))
	{
		printf("\nCould not open Serial Port\n");
		return(RET_CODE_FAIL);
	}
	return(RET_CODE_OK);
}

//
// Close for windows
//
void	MODEM_close()
{
	if(SerHandle!=0)
	{
		CloseSerial(SerHandle);
		SerHandle=0;
	}
}

#else

//
// AVR Mega Init
//
S8 MODEM_init(U16 speed)
{
U16 baud_rate;

	if(speed==0)
		baud_rate=MODEM_BAUD_STATIC;
	else
		baud_rate=speed;

	pushch=0;
	UART1_Init(baud_rate);
	return(RET_CODE_OK);
}
#endif


#ifdef __AVR__
//
// UART1 Rx FIFO and ready
//                        
#define			UART1_RX_BUFFER_SIZE	32
#define			UART1_RX_FLOW_SIZE		30
volatile U8		UART1_RxBuffer[UART1_RX_BUFFER_SIZE];
volatile U8		UART1_RxSize;
volatile U8		UART1_RxHead;
volatile U8		UART1_RxTail;
volatile U8		UART1_Ready;

#define kbhit() ((UART1_RxSize))

/* UART1 Receive Complete Interrupt Function */
SIGNAL(SIG_UART1_RECV)      
{    
U8	c;             

	//RXI_ON();
    /* Get incomming char */
    c = inp(UDR1);
    
	/* check for room on the fifo, if so store the char, else we toss it. */
	if(UART1_RxSize<UART1_RX_BUFFER_SIZE)
	{
		UART1_RxBuffer[UART1_RxTail++]=c;
		if(UART1_RxTail>=UART1_RX_BUFFER_SIZE)
			UART1_RxTail=0;
		UART1_RxSize++;
		if(UART1_RxSize>=UART1_RX_FLOW_SIZE)
		{
			// Turn Clear to Request to send off    
		//	RTS1_OFF();
		}
	}
}

/* UART1 Receive Complete Interrupt Function */
SIGNAL(SIG_UART1_TRANS)      
{    
    UART1_Ready = ((U8)0x1);
}


void 
UART1_Init(U16 Baud_Rate )
{
	//
	// Init Rx FIFO
	//    
	UART1_RxSize=UART1_RxHead=UART1_RxTail=0;
    UART1_Ready = ((U8)0x1);
//    pUART0_Buffer = ((U8*)0x0);
//    UART0_PgmMem = ((U8)0x0);
//    UART0_CRLF = ((U8)0x0);

    /* enable RxD/TxD and interrupts */
    outp( (BV(RXCIE)|BV(TXCIE)|BV(RXEN)|BV(TXEN)), UCSR1B );

    /* set baud rate */
    outp( Baud_Rate, UBRR1L ); 
	outp( Baud_Rate >> 8, UBRR1H);
#ifdef DEBUG_LV3   
	CONSOLE_puts(" Modem baud rate ");
	printShort(Baud_Rate);
	CONSOLE_puts(CRLF);
#endif
    /* Set Request to send on */ 
    /* Data Set Ready on */
	//RTS1_ON();
	//DTR1_ON();
}

static char 
getch( void ) 
{   
U8	c;
#ifdef DEBUG_LV5    
	CONSOLE_puts("UART1_RxSize ");
	printShort(UART1_RxSize);
	CONSOLE_puts(CRLF);
#endif
	    /* wait for UART to indicate that a character has been received */
    	while( !UART1_RxSize);
    
	    /* get byte from fifo */
		c=UART1_RxBuffer[UART1_RxHead++];
		if(UART1_RxHead >= UART1_RX_BUFFER_SIZE)
			UART1_RxHead=0;
	
		/* Decerment number of bytes in fifo, if we are under flow size make sure flow control is off   */
		/* this is a critical section, we don't want off flow control after the interrupt has turned    */
		/* it on after we've updated the UART_RxSize, so protect this section 							*/

		cli(); 
		UART1_RxSize--;
		if(UART1_RxSize < UART1_RX_FLOW_SIZE)
		{
			// Turn Request to Send on.
	       // RTS1_ON();				
		}    
	    // End critical section
		sei();
	//
	// Return char
	return c;
}

/* putch(char c) - putchar for UART0 */
static char 
putch( char c )
/* blocks */
{
    /* wait for UART to become available and we are not throttled by flow control */

//	TXI_ON();

	// User flow contol? +++FIX RTS should be CTS+++
//	if(mm.SerialConfig&MODEM_RTS)
//		while((!UART1_Ready) || !CTS1()) {wait();}		/* RTS not working on hyperterm? */
//	else
		while( !UART1_Ready ) wait();


    UART1_Ready = 0;
	
    /* Send character */
    outp( c, UDR1 );

    return c;
}

#endif








void
MODEM_putch(U8 c)
{
	//printf("%2x ",c);
#ifdef WINDOWS
	SendByte(SerHandle,c);
#else
	putch(c);
#endif
}	






S16
MODEM_getch()
{
S16	ret;

	//
	// If a char has been pushed back return it, else return one from the serial port
	//
	if(pushch)
	{
		ret=pushch &0xff;
		pushch=0;
	}
	else
	{
#ifdef WINDOWS
		ret=GetByte(SerHandle);
#else
		if(kbhit())
		{
			ret=getch();
		}
		else
		{
			ret=-1;
		}
#endif
	}
	return(ret);
}

void
MODEM_pushch(U8 c)
{
	// push the char back into the serial buffer, flag the buffer is full even if the
	// char is null.  We only handle one pushed back char (all we need to)
	pushch=c | 0x100;
}

char
MODEM_kbhit()
{
	if((pushch) || kbhit())
		return(1);
	return(0);
//	else
//	{
//		return(kbhit());
//	}
}

void
MODEM_puts(char *buf)
{
	if(buf!=0)
		while(*buf)
			MODEM_putch(*buf++);
}

//
// Tries to read a string of len (len) from UART1, returns 0 for timeout
//	and nothing read, < len for timeout and something read, or len if everything read
//
U8 
MODEM_reads(char *buf, U16 len)
{	
	U8  start_tick;
	U8	ret=0;

	// Set timeout
	start_tick=TIMER_Get_Tick();
	while(((U8)(TIMER_Get_Tick()-start_tick))<3)
	{
		if(MODEM_kbhit())
		{
			buf[ret++]=MODEM_getch();
			start_tick=TIMER_Get_Tick();
			if(ret==len)
				break;
		}
	}
	DEBUG1("reads len ");
	printShort((U16)ret);
	DEBUG1(CRLF);
	return ret;
}



U8
MODEM_Camera_SendCmd(U8 cmd, U8 d1 , U8 d2, U8 d3, U8 d4)
{
U8	i;
U8	buf[13];

	MODEM_putch(0xaa);
	MODEM_putch(cmd);
	MODEM_putch(d1);
	MODEM_putch(d2);
	MODEM_putch(d3);
	MODEM_putch(d4);
	
	if(6==MODEM_reads(buf, 6))
	{
		for(i=0;i<6;i++)
			printByte(buf[i]);
		CONSOLE_puts(CRLF);
	}

}


U8
MODEM_Init_Camera(void)
{
	U8 count=0;
	U8	buf[13];

	while(count++<254)
	{
			// Send Synch
		DEBUG2("Sync"CRLF);
			MODEM_putch(0xaa);
			MODEM_putch(0x0d);
			MODEM_putch(0x00);
			MODEM_putch(0x00);
			MODEM_putch(0x00);
			MODEM_putch(0x00);
		// Wait for response
		TIMER_Wait_Tick(5);
		// Check for response
		if(12==MODEM_reads(buf, 12))
		{
			// we have ack #
			if((buf[0]==0x0aa) && (buf[1]==0x0e) && (buf[2]==0x0d))
			{
				// send ack reply
				MODEM_putch(0xaa);
				MODEM_putch(0x0e);
				MODEM_putch(0x0d);
				MODEM_putch(buf[3]);
				MODEM_putch(0x00);
				MODEM_putch(0x00);
				break;
			}
		}	
	}
	if(count>253)
		return 0;
	else
		return 1;
}

U8
MODEM_Get_Pic()
{
}




#if 0
//
// Poll_serial() - This should be in a different file, this should be platform specific
//
// Get a char from the modem port and process it.
//
void
poll_serial()
{
S16 ci;
U8 status;

#ifdef __AVR__
	while(kbhit())
	{
			ci=MODEM_getch();
#else
	while(1)
	{
			ci=MODEM_getch();
			if(ci==-1)
				break;
#endif
			//printByte((U8)ci);
			//CONSOLE_puts(" - ");
			status=ahdlc_rx((U8) ci);

			///
			// if status == 1 then ppp down, feed through to console
			//
			if(1==  status)
				CONSOLE_putch((U8)ci);
			//
			// Else if status is 2 we cannot process the char, push it back on the serial buffer
			// so we can process it next time
			//
			else if (2==status)
			{
				MODEM_pushch((U8)ci);
				DEBUG2("pushback \n");
			}
	}
}
#endif

  /*
void
poll_serial()
{
int ci;

	if((ci=GetByte(ser))!=-1)
	{
			//printf("-%x",ci);
			ahdlc_rx((U8) ci);
	}
}
*/


