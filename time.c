
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * timeer.c - Time/timer related fucntions. (intel port versions)			-
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
*/
/*					*/ 
/* include files 	*/
/*	
*/
#include "mTypes.h"
#include "econfig.h"
//#include "ircmd.h"
 //#include "ports.h"
//#include "time.h"
//#include "console.h"
//#include "globals.h"
//#include "debug.h"

//
// Timer start for timer, if we need more we can index and allocate.
//
static	U16		time_start;

#ifdef WINDOWS
//
// Windows get seconds
//
U16 get_seconds()
{
time_t			t;

	time(&t);
	return((U16)t);
}
#else
//
// AVR timer stuff
//
//#define TICKS_PER_SECOND    70
/* TIMER global variables */
volatile U8	 TIMER_Counter;
volatile U16 TIMER_Tick;
volatile U8  TIMER_Second;
volatile U8	 TIMER_wdt;

/* AVR Timer 0 overflow Signal */
SIGNAL(SIG_OVERFLOW0)
{   
	/* incerment timer */
	TIMER_Counter++;
	TIMER_Tick++;

	/* seconds need to be updated? */
	if(TIMER_Counter>TICKS_PER_SECOND)
	{
		TIMER_Second++;
		TIMER_Counter=0;
		if(TIMER_wdt)
		{
			TIMER_wdt--;
			if(0==TIMER_wdt)
				asm volatile ("jmp 0x0000");
		}
	}
	
	/* Reset Timer */
	TCNT0= 190;							//outp((U8)(190/*-TICKS_PER_SECOND*/)/*TIMER_SELECT*/,TCNT0);
	//outp((U8)TIMER_SELECT,TCNT0);

	/* turn off indicators */
	TXI0_OFF();
	RXI0_OFF();

}

#endif






void
timer_set(void)
{
	//printf("set timer\n");
	time_start=TIMER_Get_Second();
	if(0==time_start)
		time_start++;
}

U8
timer_timeout(U16 timeout_val)
{
	if(0==time_start)
	{
		//DEBUG1("timerturned off\n");
		return 1;
	}
	if(timeout_val < (TIMER_Get_Second()-time_start))
	{
		//printf("timeout %d  --  %d \n",timeout_val, TIMER_Get_Second()-time_start);
		if(mm.IMMConfig & DEBUG_MODE)
		{
			DEBUG1("timer timeout"CRLF);
		}
		return 1;
	}
	else
	{
		//printf("notimeout %d  --  %d \n",timeout_val, TIMER_Get_Second()-time_start);
		return 0;
	}
}

U8
timer_expire(void)
{
	///DEBUG1("turn off\n");
	time_start=0;
	return(0);
}


//#define TICKS_PER_SECOND    100
//U16 TIMER_Counter;
//U8  TIMER_Tick;
//U8  TIMER_Second;

//
// This function initilizes timer 0 on the Mega128/64 (and possibly other avr chips)
//
//
void 
TIMER_Init(void)
{
#ifdef __AVR__
	    /* enable TCNT0 overflow */
    TIMSK=1<<TOIE0;							//outp((1<<TOIE0),TIMSK);

    /* count with cpu clock/1024 */
    TCCR0=0x07;								//outp(0x07,TCCR0);

    /* reset TCNT0 */
    TCNT0=(255-TICKS_PER_SECOND);				//outp((U8)(255-TICKS_PER_SECOND)/*TIMER_SELECT*/,TCNT0);
#endif
}


//void
//TIMER_WDT(U8 Timeout)
//{
//	TIMER_wdt=Timeout;
//}


U16
TIMER_Get_Tick()
{
#ifdef WINDOWS
	struct _timeb timebuffer;
	time_t			t;
	U16		ticks;
  
	_ftime( &timebuffer );
	time(&t);

	// Need to explain this calculation, this can be done better (right now it is (seconds * 64) + (millisec / 4))
//	ticks=0;
	ticks=(U16)(t<<6);
	ticks=ticks + ((timebuffer.millitm  & 0xff)>>2);
//	ticks=timebuffer.millitm;

	//printf("%d\n",ticks);

	return(ticks);
#else
	// AVR
	return(TIMER_Tick);
#endif   
}

U16
TIMER_Get_Second()
{
#ifdef WINDOWS
	return(get_seconds());
#else
	// AVR
	return(TIMER_Second);
#endif
} 


// How long does this wait?
void
TIMER_Wait_Tick(U16 wait_ticks)
{     
U16	tick_start;

	tick_start=TIMER_Get_Tick(); 
    while(1)	
    {   
		//poll_serial();
	   	if(abs(TIMER_Get_Tick()-tick_start) > wait_ticks )	
			break;
	}
}      


void
delay_us(U16 us)
{
	U16 i,j;

	for(i=1;i<us;i++)
		for(j=1;j<2;j++);
}
