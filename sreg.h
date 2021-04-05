#ifndef	__SREG_H__
#define __SREG_H__   
/*															
 *---------------------------------------------------------------------------
 * sreg.h - S Register Processor header file								-
 *---------------------------------------------------------------------------
 * Original Version                                                         -
 *  Nov 7 2000	- mwj                                                       -
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2000, iReady Corporation, All Rights Reserved.				_
 *                                                                         	_
 *---------------------------------------------------------------------------
*/    

#include "mTypes.h"

// Dependancies
#include "econfig.h"
#include "memory.h"
#include "mip.h"
#include "udpcmd.h"
#include "sreg.h"
#include "econfig.h"
#include "msg.h"
#include "dhcp.h"
#include "ports.h"
// Quasi - to be removed
#include "globals.h"

// Debug Dependancies
#include "console.h"
#include "debug.h"


U8 sRegister(U8 *);  
  

#endif /* SREG_H */  

