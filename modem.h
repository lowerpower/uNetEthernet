#ifndef __MODEM_H__
#define __MODEM_H__

/*																www.nchip.com
 *---------------------------------------------------------------------------
 * modem.h - Modem and modem port functions, windows console version		-
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
/*					*/ 
/* include files 	*/
/*					*/ 
#include "mTypes.h"
#include "econfig.h"

// Dependancies

// Debug dependancies
#include "console.h"
#include "conio.h"
#include "debug.h"

//
// private
//
void	UART1_Init(U16 Baud_Rate );

//
// Public
//
void MODEM_putch(U8 c);
S16  MODEM_getch(void);
void MODEM_pushch(U8 c);
char MODEM_kbhit(void);
void MODEM_puts(char *buf);
U8	 MODEM_reads(char *buf, U16 len);

U8	 MODEM_Init_Camera(void);
U8	 MODEM_Camera_SendCmd(U8 cmd, U8 d1 , U8 d2, U8 d3, U8 d4);

#ifdef WINDOWS
void	MODEM_close(void);
S8		MODEM_init(U8 port,U16 speed);
#else
S8		MODEM_init(U16 speed);
#endif




#endif


