#ifndef __UDPCMD_H__
#define __UDPCMD_H__
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * udpcmd.h - UDP Command processor 										-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version October 6, 2003, mwj							-        
 *
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 2003, nchip, www.nchip.com 								-
 *                                                                         	-
 *---------------------------------------------------------------------------
 * Notes:
 *
*/

#include "mTypes.h"

// Dependancies
#include "time.h"
#include "memory.h"
#include "mip.h"
#include "mudp.h"
#include "msg.h"
#include "ircmd.h"

// Quasi - to be removed
#include "globals.h"

// Debug Dependancies
#include "console.h"
#include "debug.h"


extern  U16			udp_sp;
extern  IPAddr		udp_ip;
extern  U16			udp_len;
extern  MEMBLOCK	*udp_ptr;


U8	udpcmd(U8 *ptr);
 

#endif
