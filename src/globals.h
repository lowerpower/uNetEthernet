#ifndef __GLOBAL_H__
#define __GLOBAL_H__
/*! \file globals.h
    \brief globals header file, global vars for memory map

*/
/*!																www.mycal.com			
 *---------------------------------------------------------------------------
 *! \file globals.c
 *  \brief Global Config Vars for the IMM project, configurable memory map
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 2003									-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 2003, Mycal Labs www.mycal.com								-
 *                                                                         	-
 *---------------------------------------------------------------------------
*/
//#include	"mTypes.h"
//#include	"econfig.h"
//#include	"mip.h"
#define PPP_DEBUG			0x40

// SerialConfig byte equates
#define MODEM_RTS 			0x1						// Set if using modem RTS
#define MODEM_CTS			0x2						// Set if using modem port CTS
#define CONSOLE_RTS			0x4						// Set if using console RTS
#define CONSOLE_CTS			0x8						// Set if using console CTS
#define RI_PASSTHROUGH	    0x10					// Pass through RI indicator
#define CD_PASSTHROUGH		0x20					// Pass through CD indicator 
#define AUTOBAUD			0x80					// Autobaud Console Port

// Network Config bits
#define	DHCP_ENABLE			0x01
//
// Interface Mode Config bits 
//
#define ECHO_CMD  			0x1						// echo chars if set
#define ECHO_STREAM			0x2						// echo chars in stream mode if set
#define RESULT				0x4						// Result codes verbose or numeric
#define INBAND_ESCAPE		0x8        
//#define DATA_MODE_ESCAPE    0x10
#define BOOT_BANNER			0x20
#define DEBUG_MODE			0x40					// Set to 1 for debug
//#define DEBUG_SEND			0x80
#define RESULT_OFF			0x80

#define	MAX_SND_SIZE		128						// ++ this has to be tested

#define	MAX_RX_PACKET		700
extern	U8	rx_pkt[];

#ifdef WINDOWS
extern	iface;
#endif

//(IPADDR myip, IPADDR *netmask, IPADDR *gatewayp

#ifdef WINDOWS
#pragma pack(1)
#endif
typedef struct	MEMMAP_
{  
	U8			IMMConfig;				// 0x20 IMM configuration Byte |DS|DPPP|BB|DE|IE|R|ECHO_STREAM|ECHO_CMD|
	U8			escapeChar;				// 0x21 this is the char used for IMM escape to cmd mode
	U8			escape_timeout;			// 0x22 Guard Time
	U8			TCPStreamTickTime;		// 0x23 TCP send timer
	U8			UDPStreamTickTime;		// 0x24 UDP send timer
	U8			dns_timeout;			// 0x25 DNS retry time
	// Serial Ports
	U8			SerialConfig;			// 0x26 Serial Config Byte |CAB|CD|CD|x|CCTS|CRTS|MCTS|MRTS|
	U8			ConsoleBaud;			// 0x27 ConsoleBaudRate
	// UDP config
	U8			udp_flags;				// 0x28
	// TCP configuration Vars
	U8			tcp_connect_timeout;	// 0x29 In Seconds
	U16			tcp_retransmit_timeout;	// 0x2a In 10's of MS
	// IP configuration vars
	U8			ip_ttl;					// 0x2c IP time to live
	U8			ip_tos;					// 0x2d IP type of service
	// Network config
	U8			network_config;			// 0x2e netowrk config |x|x|x|x|x|x|x|DHCP|
	U8			our_mac_addr[6];		// 0x2f
	// IP address info
	IPAddr		our_ipaddr;				// 0x35
	//
	// Changing info depending on product.
	//
	IPAddr		netmask;				// 0x39
	IPAddr		gatewayip;				// 0x3d
	IPAddr		pri_dns_addr;			// 0x41
	IPAddr		sec_dns_addr;			// 0x45
	//
	// Read Only, or status only regesters
	//
	U8			ircmd_state;			// 0x49 command processor state (warning about writing)

#ifdef __AVR__	
}MEMMAP __attribute__ ((packed));
#else
}MEMMAP;
#endif

extern	MEMMAP mm;


U8	global_init(U8 cmd);

#endif

