#ifndef __RESOLVE_H__
#define __RESOLVE_H__
/*															www.nchip.com
 *---------------------------------------------------------------------------
 * Resolve.h - Include file for resolve.c, a very simple, 					_
 *	very optimized DNS resolver.											_
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 23, 2000									-        
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2000, nChip, All Rights Reserved.							_
 *                                                                         	_
 *---------------------------------------------------------------------------
*/

//
// iTypes.h
//               
#include "mTypes.h"
 
// Dependancies
#include "time.h"	
#include "mudp.h"
#include "resolve.h"   
#include "memory.h"

// Quasi - to be removed
#include "globals.h"

// Debug dependancies
#include "console.h"
#include "conio.h"
#include "debug.h"



//
// Tunable parametser
//                   
#define		MAX_DNS_RETRY			3	// Number of time to retry sending DNS request
#define		DNS_REPLY_WAIT_TIME		4	// How long in seconds to wait for server response
										// Valid range (2-250)
//
// Local Prototypes
//
U16			gobble_name(U8 *data, U16 index);
//
// API - External Prototypes
//
extern		void		Set_DNS_Server(IPAddr *);         
extern		void 		Get_DNS_Server(IPAddr *serverip);
extern 		RET_CODE	resolve_name(IPAddr *, U8 *);

#endif
