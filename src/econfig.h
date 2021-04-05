#ifndef __ECONFIG_H__
#define __ECONFIG_H__
/*! \file econfig.c \brief uNetE Project Config file. */
//---------------------------------------------------------------------------
// econfig.h - ethernet config header file 									-
//---------------------------------------------------------------------------
// This file contains all the includes used for the project.  It also		- 
//	includes the default equate values for the project.						-
//																			-
//	Great care should be taken to properly configure these values.			-
//																			-
//---------------------------------------------------------------------------
// Version                                                                  -
//		0.1 Original Version July 16, 2002									-
//																			-
// (c)2001 Mycal Labs, All Rights Reserved									-
//---------------------------------------------------------------------------

// Includes for project, first must by the type declares 
#include "mTypes.h"
//
// Add includes for the project here
//
#include "memory.h"
#include "arp.h"
#include "mip.h"
#include "mtcp.h"
#include "mudp.h"
#include "resolve.h"
#include "conio.h"
#include "console.h"
#include "time.h"
#include "ports.h"
#include "globals.h"
#include "emain.h"

#ifndef WIN32
#include "enc28j60.h"
#endif

#include "debug.h"


// Version Number here
#define EM_VERSION				"0.6c"

// Demo Version define
//#define	DEMO					1

//#define MEGA128					1
//#define MEGA64					1
//#define MEGA162					1

// Let the compiler determine which AVR chip:
#if defined (__AVR_ATmega64__)
#define MEGA64						1
#elif defined (__AVR_ATmega128__)
#define MEGA128						1
#define	DEMO					1
#elif defined (__AVR_ATmega162__)
#define MEGA162						1
#define DEMO						1
#endif


//
// Clocks and baud rate config
//
#define CPU_CLOCK 				7372800     /* 7.3728Mhz -or- 7372800 */
#define	CONSOLE_BAUD_RATE		19200 		/* terminal baud rate 9600bps if no autobaud*/
#define	CPU_CLOCK_DIV100			CPU_CLOCK/100
#define CONSOLE_BAUD_RATE_DIV100	CONSOLE_BAUD_RATE/100
#define CONSOLE_BAUD_STATIC		(CPU_CLOCK_DIV100/(CONSOLE_BAUD_RATE_DIV100*16)-1)

#define	MODEM_BAUD_RATE			115200 //14400
#define MODEM_BAUD_RATE_DIV100	MODEM_BAUD_RATE/100
#define MODEM_BAUD_STATIC		(CPU_CLOCK_DIV100/(MODEM_BAUD_RATE_DIV100*16)-1)
	
//
// IR command processor defaults
//
#define ESCAPE_TIMEOUT			2


//
// TCP Configuration defaluts
//
#define	TCP_CONNECT_TIMEOUT		5
//#define	TCP_TIMETOUT			4

//
// DNS default config
//
#define	DNS_REPLY_WAIT_TIME		4


//
// Platform specific stuff
//

#ifdef __AVR__

#ifdef MEGA128						
//#include <iom128.h>
#endif
#ifdef MEGA64						
//#include <iom64.h>
#endif

#include <ctype.h>
#include <stdint.h>
#include <stdio.h>
#include <avr/interrupt.h>
//#include <signal.h>
//#include <avr/signal.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>

#define FLASHMEM __attribute__ ((progmem))

//#define CPU_CLOCK 				7372800     /* 7.3728Mhz -or- 7372800 */
//#define USE_SERIAL_FLOW_CNTROL	1
//#define USE_AUTOBAUD			0
//#define USE_DCD					0
//#define USE_DTR					0
//#define	USE_TTLESCAPE			0
//#define USE_SREG_BAUD			1

//#define printf				CONSOLE_puts
#endif


#ifdef WIN32
#define WINDOWS			1

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <winbase.h>
//#include <conio.h>
#include <time.h>
#include <stdio.h> 
#include <sys/timeb.h>


#endif


void poll_serial(void);


#endif 

