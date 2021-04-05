#ifndef DHCP_H
#define DHCP_H
/*! \file dhcp.h \brief DHCP Protocol Library. */
//@{
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * mip.h - Mycal DHCP client header file									-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 1998									-        
 *
 * Exports:
 *	
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
#include "micmp.h"
#include "memory.h"
#include "mip.h"
#include "mudp.h"
#include "mtcp.h"
#include "net_packets.h"
#include "nic.h"

// Quasi - to be removed
#include "globals.h"

// Debug Dependancies
#include "console.h"
#include "debug.h"
#include "conio.h"


#define	DHCP_CLIENT_PORT		68	///< UDP port clients will receive DHCP replies
#define	DHCP_SERVER_PORT		67	///< UDP port where DHCP requests should be sent

#define	DHCP_TIMEOUT			5	///< DHCP timeout in seconds

#define DHCP_MSG_DHCPDISCOVER	1	///< DISCOVER is broadcast by client to solicit OFFER from any/all DHCP servers.
#define DHCP_MSG_DHCPOFFER		2	///< OFFER(s) are made to client by server to offer IP address and config info.
#define DHCP_MSG_DHCPREQUEST	3	///< REQUEST is made my client in response to best/favorite OFFER message.
#define DHCP_MSG_DHCPDECLINE	4	///< DECLINE may be sent by client to server to indicate IP already in use.
#define DHCP_MSG_DHCPACK		5	///< ACK is sent to client by server in confirmation of REQUEST, contains config and IP.
#define DHCP_MSG_DHCPNAK		6	///< NAK is sent to client by server to indicate problem with REQUEST.
#define DHCP_MSG_DHCPRELEASE	7	///< RELEASE is sent by client to server to relinquish DHCP lease on IP address, etc.
#define DHCP_MSG_DHCPINFORM		8	///< INFORM is sent by client to server to request config info, IP address configured locally.


// Bootp Header (see stevens ch16 pg 216) (DHCP is transported by BOOTP/UDP/IP)
typedef struct DHCP_
{      
	U8			opcode;					///< Message op-code / message type
	U8			hw_type;				///< Hardware address type   (Ethernet=1)
	U8			hw_address_len;			///< Hardware address length (Ethernet=6 byte MAC addr)
	U8			hop_count;				///< hop count (client set to zero)
	U32			transaction_ID;			///< Transaction ID (randomly chosen by client, must remain same)
	U16			num_seconds;			///< Seconds elapsed since DHCP negotiation began (filled by client)
	U16			flags;					///< Flags
	IPADDR		client_ip;				///< Client IP address (filled only if already bound, renewing, or rebinding)
	IPADDR		your_ip;				///< Client IP address (reply from server)
	IPADDR		server_ip;				///< Server IP address
	IPADDR		gateway_ip;				///< Gateway IP address
	U8			client_hw_address[16];	///< Client Hardware Address
	U8			server_hostname[64];	///< Server Host Name
	U8			boot_filename[128];		///< Boot file name (null-term string)
	U32			cookie;					///< DHCP cookie
	U8			options[];
}DHCP;       

typedef struct	DHCP_ELEMENT_
{
	U8				code;
	U8				len;
	U8				data[];
}DHCP_ELEMENT;


extern U32		DhcpLeaseTime;		///< Number of seconds left in DHCP lease

#define BOOTP_HEADER_LEN		236	///< length of BOOTP header not including options

#define BOOTP_OP_BOOTREQUEST	1	///< BOOTP Request operation (message from client to server)
#define BOOTP_OP_BOOTREPLY		2	///< BOOTP Reply operation (message from server to client)

#define BOOTP_HTYPE_ETHERNET	1
#define BOOTP_HLEN_ETHERNET		6


#define DHCP_HEADER_LEN			240	///< length of DHCP header not including options

#define DHCP_UDP_SERVER_PORT	67	///< UDP port where DHCP requests should be sent
#define DHCP_UDP_CLIENT_PORT	68	///< UDP port clients will receive DHCP replies


#define DHCP_OPT_PAD			0	///< token padding value (make be skipped)
#define DHCP_OPT_NETMASK		1	///< subnet mask client should use (4 byte mask)
#define DHCP_OPT_ROUTERS		3	///< routers client should use (IP addr list)
#define DHCP_OPT_TIMESERVERS	4	///< time servers client should use (IP addr list)
#define DHCP_OPT_NAMESERVERS	5	///< name servers client should use (IP addr list)
#define DHCP_OPT_DNSSERVERS		6	///< DNS servers client should use (IP addr list)
#define DHCP_OPT_HOSTNAME		12	///< host name client should use (string)
#define DHCP_OPT_DOMAINNAME		15	///< domain name client should use (string)
#define DHCP_OPT_REQUESTEDIP	50	///< IP address requested by client (IP address)
#define DHCP_OPT_LEASETIME		51	///< DHCP Lease Time (uint32 seconds)
#define DHCP_OPT_DHCPMSGTYPE	53	///< DHCP message type (1 byte)
#define DHCP_OPT_SERVERID		54	///< Server Identifier (IP address)
#define DHCP_OPT_PARAMREQLIST	55	///< Paramerter Request List (n OPT codes)
#define DHCP_OPT_RENEWALTIME	58	///< DHCP Lease Renewal Time (uint32 seconds)
#define DHCP_OPT_REBINDTIME		59	///< DHCP Lease Rebinding Time (uint32 seconds)
#define DHCP_OPT_END			255 ///< token end value (marks end of options list)

/*!	Initialize DHCP system.
	Prepares DHCP for use and initializes lease time to zero. */
S8 dhcpInit(void);

/*!	Processes incoming DHCP packets from UDP port 68.
	This function is to be called by the stack when a DHCP packet
	arrives over the network.  The DHCP packet will be parsed, handled,
	and a response will be generated and sent if needed.  When the DHCP
	process completes, the IP addressing will be automatically updated. */
void dhcpIn(unsigned int len, struct netDhcpHeader* packet);

/*!	Request DHCP assigned network parameters.
	This function begins the DHCP process.  The remainder of operations
	are handled in dhcpIn(). */
void dhcpRequest(void);

/*!	Release DHCP lease and assigned network parameters.
	This function releases the DHCP assigned address and allows the
	DHCP server to reallocate it. */
void dhcpRelease(void);

/*! Periodic DHCP maintenance.
	This function is to be called once per second and will 
	expire the DHCP lease. */
void dhcpTimer(void);

/*!	Get a DHCP option from the option list.
	\param options is a pointer to the options field of a DHCP packet.
	\param optcode is the desired option number to retrieve.
	\param optlen is the maximum data length that should be retrieved (less data will be retrieved if option is shorter).
	\param optvalptr is a pointer to where the option value will be stored.
	\return actual length of the option data, as stored in the options list. */
//uint8_t dhcpGetOption(uint8_t* options, uint8_t optcode, uint8_t optlen, void* optvalptr);
U8* dhcpGetOption(U8* options, U8 optcode, U8 optlen, void* optvalptr);

/*!	Set a DHCP option in the option list.
	\param options is a pointer to the options field of a DHCP packet.
	\param optcode is the option number to write.
	\param optlen is the data length of the option value.
	\param optvalptr is a pointer to the option data to be read.
	\return pointer to write location of the next option. */
//uint8_t* dhcpSetOption(uint8_t* options, uint8_t optcode, uint8_t optlen, void* optvalptr);
U8* dhcpSetOption(U8* options, U8 optcode, U8 optlen, U8* optvalptr);

/*! Print diagnotic information about BOOTP/DHCP packet.
*/
//void dhcpPrintHeader(struct netDhcpHeader* packet);

#endif
//@}

