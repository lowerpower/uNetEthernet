/*! \file arp.h \brief ARP Protocol Library. */
//*****************************************************************************
//
// File Name	: 'arp.h'
// Title		: ARP Protocol Library
// Author		: Pascal Stang
// Created		: 9/7/2004
// Revised		: 7/3/2005
// Version		: 0.1
// Target MCU	: Atmel AVR series
// Editor Tabs	: 4
//
///	\ingroup network
///	\defgroup arp ARP Protocol Library (arp.c)
///	\code #include "net/arp.h" \endcode
///	\par Description
///		To send anything over ethernet (or most any other physical network)
///		a packet must be addressed to a physical network node address, often
///		called a MAC/hardware/ethernet address.  This MAC address identifies
///		a specific interface (like the ethernet card in your computer) on the
///		network.
///			ARP (Address Resolution Protocol) assists in mapping IP addresses to 
///		the MAC addresses required to actually get data to its destination.
///		In other words, an IP address is not enough to send information over
///		ethernet.  You need the MAC address of the network interface/card that
///		"owns" that IP address.  ARP maintains a table mapping IP addresses to
///		MAC addresses.  This table can be filled by both listening to
///		traffic on the network, as well as making specific ARP requests if
///		an IP<->MAC mapping is not in the table.
///		
///	\note This code is currently below version 1.0, and therefore is considered
///	to be lacking in some functionality or documentation, or may not be fully
///	tested.  Nonetheless, you can expect most functions to work.
///
//	This code is distributed under the GNU Public License
//		which can be found at http://www.gnu.org/licenses/gpl.txt
//*****************************************************************************
//@{

#ifndef ARP_H
#define ARP_H

// Must call mTypes first
#include "mTypes.h"
//
// Include other needed includes
//
#include "net_packets.h"
#include "nic.h"

// Debug
#include "debug.h"
#include "console.h"
#include "conio.h"
#include <string.h>

#define ARP_TYPE				0x0608			// in network order
#define	ARP_REQUEST				0x0100			// 0x0001 in network order
#define ARP_REPLY				0x0200			// 0x0002 in network order
#define ARP_EXPIRE_TIME			1000			// 1000 ticks 
#define ARPHRD_ETHER			0x0100			// hardware type ethernet 10Mb
#define ARP_SIZE				0x3e			// arp packet size    
    			       //			            

//
// ARP cache size configuration
//
#ifndef ARP_CACHE_SIZE
#define ARP_CACHE_SIZE	8
#endif

//
// ARP cache entry time to live.
//
#ifndef ARP_CACHE_TTL
#define ARP_CACHE_TTL	200
#endif

//
// ARP cache entry.
//
typedef struct ARPENTRY_
{                        
	IPAddr				ipaddr;
	U8					macaddress[6];
	U16					time;
}ARPENTRY; 


/*!	Initialize ARP system.
	Clears ARP table and prepares it for use. This is typically done
	once at program initialization. */
void arpInit(void);

/*!	Set IP and Ethernet hardware/MAC address.
	This must be done before valid replies can be generated for ARP
	requests. Typically done once at program initialization. */
void arpSetAddress(IPADDR ipaddr, U8 *mac_addr);

/*!	Processes incoming ARP packets.
	This function is to be called when an ARP type packet has arrived
	over the network.  If the packet type is an ARP request for us,
	an ARP reply will be generated and sent. */
void arpHandler(U8 *pkt, U16 len);

/*!	Add an ARP entry to the cache, will replace oldest entry if cache is full.
*/
void arpAdd(IPADDR sip, U8 *mac);

/*! Send an ARP Request.
*/
//void arpRequest(U8* packet, IPADDR destIp);
void arpRequest(IPADDR *destIp);

/*! Periodic ARP cache maintenance.
	This function is to be called once per second and will slowly 
	expire old ARP cache entries. */
void	arpProcess(void);


/*! Check if this IP address is present in the ARP cache. Returns pointer to 
	MAC address if found or zero if IP address is not found.
	Internal function.
. */
U8 *arpMatchIp(IPADDR *ip);


//! Print diagnotic information about ARP packet.
//void arpPrintHeader(struct netArpHeader* packet);
//! Print diagnotic information about ARP cache.
//void arpPrintTable(void);

#endif
//@}
