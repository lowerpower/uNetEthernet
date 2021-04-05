#ifndef	__MSG_H__
#define __MSG_H__   
/*															
 *---------------------------------------------------------------------------
 * msg.h - Message Result File												-
 *---------------------------------------------------------------------------
 * Original Version                                                         -
 *  Nov 7 2000	- mwj                                                       -
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2000, iReady Corporation, All Rights Reserved.				_
 *                                                                         	_
 *---------------------------------------------------------------------------
*/    

/*					*/ 
/* include files 	*/
/*					*/ 
#include "mTypes.h"
#include "econfig.h"

// Dependancies

// Quasi - to be removed
#include "globals.h"

// Debug dependancies
#include "console.h"
#include "conio.h"
#include "debug.h"
    


#define	MSG_OK				0
#define MSG_CONNECT			1
#define	MSG_ERROR			4
//#define MSG_NOCARRIER		5
#define MSG_FAIL			6
//#define MSG_PPPFAIL			128						//80
//#define MSG_PPPAUTHFAIL		129
//#define MSG_PPPDOWN			130						//81
#define MSG_LINKDOWN		131
#define MSG_DHCPFAIL		132
#define MSG_TCPFAIL			140						//82
#define MSG_SOCDOWN			141						//84
#define MSG_DNSFAIL			150						//87  

#define MSG_NODATA			133                    	//125
#define MSG_EEERROR			134
#define MSG_VERSION			200
#define MSG_EMPTY			0xfd
#define MSG_NULL			0xfe
#define MSG_NONE			0xff


void	displayMessage(U8);     

#endif /* msg_H */     



