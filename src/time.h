#ifndef __TIME_H__
#define __TIME_H__
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * time.h - Time/timer header file											-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 2000									-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 2000, Mycal Labs www.mycal.com								-
 *                                                                         	-
 *---------------------------------------------------------------------------
 *
 */

#include "mTypes.h"


#ifdef __AVR__

/* TIMER rate calculation */
#define TIMER_INTERVAL         10000        /* 10ms in microseconds */
#define TIMER_SCALAR           1024         
#define TIMER_SELECT           (256-((TIMER_INTERVAL*(CPU_CLOCK/TIMER_SCALAR))/1000000))
/* Be careful that this is a valid value!!! */


#define TICKS_PER_SECOND    72 /* 10ms Ticks */
#define SECONDS_PER_MINUTE  60
#define MINUTES_PER_HOUR    60
#define HOURS_PER_DAY       24

#endif




U16 get_seconds(void);

void	timer_set(void);
U8		timer_timeout(U16);
U8		timer_expire(void);


//
// Standard timer API
//
void	TIMER_Init(void);
U16		TIMER_Get_Tick(void);
U16		TIMER_Get_Second(void);
void	TIMER_Wait_Tick(U16 wait_ticks);


void	delay_us(U16 us);

#endif
