#ifndef __CONSOLE_H__
#define __CONSOLE_H__
/*																www.nchip.com
 *---------------------------------------------------------------------------
 * console.h - Console, fucntions											-
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

#include "mTypes.h"

void CONSOLE_init(U16 baud_rate);
void CONSOLE_putch(char c);
char CONSOLE_getch(void);
char CONSOLE_kbhit(void);
void CONSOLE_puts(char *buf);
void CONSOLE_cputs(char *buf);

#endif
