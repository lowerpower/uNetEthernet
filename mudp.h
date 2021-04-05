/*! \file mudp.h \brief UDP Protocol Include File. */
#ifndef __MUDP_H__
#define __MUDP_H__
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * mudp.h - Mycal UDP Layer header file										-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 1998									-        
 *
 * Exports:
 *	void	
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 1998, Mycal Labs www.mycal.com								-
 *                                                                         	-
 *---------------------------------------------------------------------------
 * Notes:
 *
*/

#include "mTypes.h"

// Dependancies
#include "memory.h"
#include "mip.h"

// Quasi - to be removed
#include "globals.h"

// Debug Dependancies
#include "console.h"
#include "debug.h"

    
#define MAX_UDP_SOCKETS		2



//
// Define UDP packet structure
//
typedef struct	udp_h
{  
	U16	source_port;
	U16	dest_port;
	U16	length;
	U16	checksum;
	U8	data[];
}UDP;    

//
// Define UDP socket callback structure
//
typedef struct	udp_socket_h
{  
	U16		port;
	void	(*socket_callback)(struct ip_h *header,U8 *data, U16 len);
	//void	(*socket_callback)(U8 *data, U16 len);
}UDPSOC;   


//
// Exported Functions
//
void	udp_upcall(struct ip_h *, struct udp_h *, U16 );

//
// Application Calls (API);
//
void	udp_init(void);
//U8		udp_listen(U16 port, void (*callback)(struct ip_h *header,U8 *data, U16 len));
U8		udp_bind(U16 port, void (*callback)(struct ip_h *header,U8 *data, U16 len));
void	udp_close(U8 socket);
S16		udp_tx(IPAddr dest_ip, U16 dest_port, U16 src_port, U8 *data, U16 len);
U16		udp_port(U8 socket);

#endif
