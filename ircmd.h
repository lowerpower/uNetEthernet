#ifndef __IRCMD_H__
#define __IRCMD_H__

//---------------------------------------------------------------------------
// ircmd.h - IR Command header												-
//---------------------------------------------------------------------------
// Version                                                                  -
//		0.1 Original Version March 21, 2000									-
//---------------------------------------------------------------------------
// (c)2000,2001 iReady, All Rights Reserved										-
//---------------------------------------------------------------------------
//
// ircmd_state values		masks	=
//							0x01	passthrough
//
//
#include "mTypes.h"
#include "econfig.h"
#include <ctype.h>
#include <stdlib.h>
//#include "imm.h"
//#include "ppp.h"
//#include "time.h"
//#include "resolve.h"  
//#include "sreg.h"
//#include "ppp.h"       

#include "msg.h"    
#include "ports.h"   
//#include "mudp.h"
//#include "ircmd.h"   
//#include "udpcmd.h"
#include "console.h"
//#include "modem.h"
#include "globals.h"
#include "dhcp.h"
#include "debug.h"
//#include "memory.h"

#define IR_MODEM_COMMAND		1
#define	IR_MODEM_DATA			3
#define IR_INTERNET_COMMAND		4
#define IR_STREAM_SOCKET		6
#define IR_MODEM_ONLY			9

//
// Interface Mode Config bits 
//
#define ECHO_CMD  			0x1						// echo chars if set
#define ECHO_STREAM			0x2						// echo chars in stream mode if set
#define RESULT				0x4						// Result codes verbose or numeric
#define INBAND_ESCAPE		0x8        
#define DATA_MODE_ESCAPE    0x10
#define BOOT_BANNER			0x20
#define PPP_DEBUG			0x40					// Set to 1 for PPP debug
//#define DEBUG_SEND			0x80
#define RESULT_OFF			0x80

#define	MAX_SND_SIZE		128						// ++ this has to be tested

//
// Export some variables
//   
extern  U8 			ircmd_state;   
extern	U8			TCPStreamTickTime;
extern	U8			IMMConfig;

extern	U8			snd_buff[];
extern  U16			snd_count;
extern  U16			TCPSendTime;

/*
extern  U16			udp_sp;
extern  IPAddr		udp_ip;
extern  U16			udp_len;
extern  MEMBLOCK	*udp_ptr;
*/

void irCommands(void);
U8	Modem_Signals(void);
U8	Get_IP(U8 *, IPAddr *);



#endif /* __IRCMD_H__ */

