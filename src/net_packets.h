/*! \file net_packets.h \brief Basic Internet Packet Structures. */
//*****************************************************************************
//
// File Name	: 'net_packets.h'
// Title		: Internet Protocol Packets
// Author		: Mike Johnson
// Created		: 9/7/2004
// Version		: 0.1
//
///	\ingroup network
///	\defgroup IP Protocol Library
///	\code #include "net/arp.h" \endcode
///	\par Description
///		Basic Internet Protocol Packet Structures.
///		
//
//*****************************************************************************
//@{

#ifndef NET_PACKETS_H
#define NET_PACKETS_H

#include "mTypes.h"


#define ETHERTYPE_IP			0x0008			// 0x0800 in network order
#define	ETHERTYPE_ARP           0x0608          // 0x0806 in network order
#define	ETHERTYPE_REVARP		0x3508			// 0x0835 in network order    
#define ETHERTYPE_IPTRAILERS	0x0001          // 0x0100 in network order

/*
#define		swap(x)		(U16)((x>>8) | (x<<8))      
#define		swapl(x) ( (((x) & 0xff000000) >> 24) | (((x) & 0x00ff0000) >> 8) | (((x) & 0x0000ff00) << 8) | (((x) & 0x000000ff) << 24)) 
#define		htons(x)	swap(x)
#define		ntohs(x)	swap(x)
#define		htonl(x)	swapl(x)
#define		ntohl(x)	swapl(x)
*/



typedef struct	arp_h
{               
	U8				dest_hw_add[6];						//0
	U8				src_hw_add[6];
	U16				ether_type;							//12
	U16				hard_type;							//14
	U16				prot_type;							//16
	U8				hard_size;							//18
	U8				prot_size;							//19
	U16				op;									//20
	U8				s_hw_add[6];						//22
	IPAddr			s_ip_add;							//28
	U8				d_hw_add[6];						//32
	IPAddr			d_ip_add;							//37
}ARP; 

typedef struct mac_h
{ 
	U8				dest_hw_add[6];
	U8				src_hw_add[6];
	U16				ether_type;
	U8				ip[];
}MAC;

/*
//
// Define the ICMP packet type
//
typedef struct	icmp_h
{
	U8	type;               	// the above defined types
	U8	code;
	U16	checksum;  
	U16	id;
	U16	seq; 
	U8	data[];
}ICMP;

*/

#endif

