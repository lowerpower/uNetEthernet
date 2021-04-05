/*! \file dhcp.c \brief DHCP client. */
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * dhcp.c - Mycal DHCP client												-
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

// DHCP init
// DHCP process

// DHCP release


#include "dhcp.h"

// global variables
IPADDR	DhcpServerIP;		///< IP address of the DHCP server that offered lease
U32		DhcpTransactID;		///< Unique transaction ID that identifies DHCP request/replies
U32		DhcpLeaseTime;		///< Number of seconds left in DHCP lease
U16		dhcp_timer;
U8		dhcpflags;			///< dhcp flags, represents dhcp state


//---------------------------------------------------------------------------------------------------
// dhcp_callback(IP *header, U8 * data, U16 len) - this is called by mTCPIP when a UDP packet
//	has been received on a DHCP bound socket, the main statemachine for DHCP request, rebind  and
//	renew is here.
//---------------------------------------------------------------------------------------------------
/*! 
  \brief DHCP packet callback
  \param header - IP header of incoming packet.
  \param *data	- pointer to incoming packet.
  \param len	- length of incoming packet.
  \return nothing.
*/
void 
dhcp_callback(IP *header,U8 *data, U16 len)
{
IPADDR	sid,rid;
S16		index;
U8		t,i;
U32		val;
DHCP	*buffer;
U8		*optptr,msgtype;


	buffer=(DHCP*)((UDP*)(data))->data;//+sizeof(UDP);

	DEBUG2("dhcp callback"CRLF);

	// check that this is a reply and that the transaction ID is mine.
	if((buffer->opcode != BOOTP_OP_BOOTREPLY) || (buffer->transaction_ID != DhcpTransactID))
		return;

	// Extract the first option
	dhcpGetOption(buffer->options, DHCP_OPT_DHCPMSGTYPE, 1, &msgtype);

	// Is it a DHCP offer message?
	if(msgtype == DHCP_MSG_DHCPOFFER)
	{
		dhcpflags=1;
	}
	// else check if it is an DHCP ACK
	else if(msgtype == DHCP_MSG_DHCPACK)
	{
		if(2==dhcpflags)
		{
			dhcpflags=3;
			return;
		}
		else
		{
			// 2nd offer ack, lets store the configuration parameters
			dhcpflags=2;
			mm.our_ipaddr.ip32=buffer->your_ip.ip32;
			optptr = dhcpGetOption(buffer->options, DHCP_OPT_NETMASK, 4, &mm.netmask);
			optptr = dhcpGetOption(buffer->options, DHCP_OPT_ROUTERS, 4, &mm.gatewayip);
			optptr = dhcpGetOption(buffer->options, DHCP_OPT_LEASETIME, 4, &DhcpLeaseTime);
			optptr = dhcpGetOption(buffer->options, DHCP_OPT_DNSSERVERS, 8, &mm.pri_dns_addr);
			ipConfig();
		}
	}
	else
	{
		//DEBUG1("dhcp msg= %d\n",msgtype);
		return;
	}
	
		DEBUG2("dhcp offer"CRLF);
		// get DHCP server ID
		dhcpGetOption(buffer->options, DHCP_OPT_SERVERID, 4, &sid);

		// build DHCP request (on top of this reply)
		buffer->opcode = BOOTP_OP_BOOTREQUEST;		// request type
		// set operation
		val = DHCP_MSG_DHCPREQUEST;
		optptr = dhcpSetOption(buffer->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);
		// set the server ID
		optptr = dhcpSetOption(optptr, DHCP_OPT_SERVERID, 4, &sid);
		// request the IP previously offered
		optptr = dhcpSetOption(optptr, DHCP_OPT_REQUESTEDIP, 4, &buffer->your_ip);
		// request additional information
		((U8*)&val)[0] = DHCP_OPT_NETMASK;
		((U8*)&val)[1] = DHCP_OPT_ROUTERS;
		((U8*)&val)[2] = DHCP_OPT_DNSSERVERS;
		((U8*)&val)[3] = DHCP_OPT_DOMAINNAME;
		optptr = dhcpSetOption(optptr, DHCP_OPT_PARAMREQLIST, 4, &val);

		DEBUG2("sending request"CRLF);
		// send DHCP request
		if(1==dhcpflags)
			DhcpServerIP.ip32 = 0xffffffff;
		else
			DhcpServerIP.ip32 = sid.ip32;

		// ++ experimental
		buffer->gateway_ip.ip32=0;

		// Send the DHCP packet (need to send broadcast)
		udp_tx(DhcpServerIP, DHCP_SERVER_PORT, DHCP_CLIENT_PORT, buffer, sizeof(DHCP)+3+6+6+6+1);
}


//****************************************************************************
// dhcp_init() -
//******************************************************************************
/*! 
  \brief Initialize the DHCP clinet
  \return 0 for fail or 1 for DHCP ok.
*/
S8
dhcp_init(void)
{
	IPADDR		destip;
	U8			macaddr[6],val;
	DHCP		*buffer;
	S8			s,ret=0;
	U16			size,stime;

	DEBUG2("dhcp init"CRLF);

	// get interface mac address
	nicGetMacAddress(macaddr);
	// set transaction ID based on mac address
	DhcpTransactID = *((U32*)&macaddr[0]);
	// reset lease time
	DhcpLeaseTime = 0;
	//
	// We use the RX buffer for DHCP, we should have exclusive access if running in polling mode
	//
	buffer=rx_pkt;
	//
	// Bind the UDP port for the DHCP reply
	//
	s=udp_bind(DHCP_CLIENT_PORT, (void *)&dhcp_callback);
	if(0==s)
	{
		DEBUG1("Failed to get socket"CRLF);
		return(ret);
	}
	//memset(buffer,sizeof(DHCP)+4,1);
	memset(buffer,0,sizeof(DHCP)+4);	
	//
	// Call DHCP send
	//
	// build BOOTP/DHCP header
	buffer->opcode			= BOOTP_OP_BOOTREQUEST;		// request type
	buffer->hw_type			= BOOTP_HTYPE_ETHERNET;
	buffer->hw_address_len	= BOOTP_HLEN_ETHERNET;
	buffer->hop_count		= 0;						// client set to zero
	buffer->num_seconds		= 0;
	buffer->flags			= 0x00;						// Set broadcaset flag was swap(1);
	//
	buffer->client_ip.ip32	= 0;						// filled only if already bound, renewing, or rebinding
	buffer->your_ip.ip32	= 0;						// from server only
	buffer->server_ip.ip32	= 0;						// from server only
	buffer->gateway_ip.ip32 = 0;						// from server only
	nicGetMacAddress(buffer->client_hw_address);		// Get HW mac address
	buffer->transaction_ID	= DhcpTransactID;			// Transaction ID derived from MAC
	buffer->cookie			= 0x63538263;				// DHCP magic cookie
	
	// set operation
	//val = DHCP_MSG_DHCPDISCOVER;
	val = DHCP_MSG_DHCPDISCOVER;
	dhcpSetOption(buffer->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);
  
	destip.ip32=0xffffffff;
	size=sizeof(DHCP)+4;
	// Send the DHCP packet (need to send broadcast)
	udp_tx(destip, DHCP_SERVER_PORT, DHCP_CLIENT_PORT, buffer, size);
	//udp_tx(destip, DHCP_SERVER_PORT, DHCP_CLIENT_PORT, buffer, size);

	//
	// wait for response, up to 10 seconds (make configurable)
	// All of the rest of the statemachine is handles in the DHCP callback routine.
	//
	stime=TIMER_Get_Second();  	
	dhcpflags=0;
	while(3!=dhcpflags)
    {   
		//poll_serial();
	   	if(DHCP_TIMEOUT <  (U8)(TIMER_Get_Second()-stime)) 
    	{                 
			ret=RET_CODE_TIMEOUT;
#ifdef DEBUG_LV1
			if(mm.IMMConfig & DEBUG_MODE)
			{
				CONSOLE_puts("DHCP timeout"CRLF);
			}
#endif	
			break;
    	}
			//printf("Timer = %x \n",TIMER_Get_Second());
			poll_serial();
	} 
		
	if(3==dhcpflags)
	{
		ret=1;
		//CONSOLE_puts("Crap"CRLF);
		DEBUG2("DHCP OK!"CRLF);
	}

	// Done, 
	// release bound socket
	udp_close(s);

	return(ret);
}



/*
void dhcpRequest(void)
{
	struct netDhcpHeader* packet;
	uint32_t val;
	
	packet = (struct netDhcpHeader*)&netstackGetBuffer()[ETH_HEADER_LEN+IP_HEADER_LEN+UDP_HEADER_LEN];

	// build BOOTP/DHCP header
	packet->bootp.op = BOOTP_OP_BOOTREQUEST;		// request type
	packet->bootp.htype = BOOTP_HTYPE_ETHERNET;
	packet->bootp.hlen = BOOTP_HLEN_ETHERNET;
	packet->bootp.ciaddr = htonl(ipGetConfig()->ip);
	packet->bootp.yiaddr = HTONL(0l);
	packet->bootp.siaddr = HTONL(0l);
	packet->bootp.giaddr = HTONL(0l);
	nicGetMacAddress(&packet->bootp.chaddr[0]);	// fill client hardware address
	packet->bootp.xid = DhcpTransactID;
	packet->bootp.flags = HTONS(1);
	
	// build DHCP request
	// begin with magic cookie
	packet->cookie = 0x63538263;
	// set operation
	val = DHCP_MSG_DHCPDISCOVER;
	dhcpSetOption(packet->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);

	#ifdef DHCP_DEBUG
	rprintfProgStrM("DHCP: Sending Query\r\n");
	//dhcpPrintHeader(packet);
	#endif

	// send request
	udpSend(0xFFFFFFFF, DHCP_UDP_SERVER_PORT, DHCP_HEADER_LEN+3+1, (uint8_t*)packet);
}

void dhcpRelease(void)
{
	struct netDhcpHeader* packet;
	uint32_t val;
	uint8_t* optptr;
	
	packet = (struct netDhcpHeader*)&netstackGetBuffer()[ETH_HEADER_LEN+IP_HEADER_LEN+UDP_HEADER_LEN];

	// build BOOTP/DHCP header
	packet->bootp.op = BOOTP_OP_BOOTREQUEST;		// request type
	packet->bootp.htype = BOOTP_HTYPE_ETHERNET;
	packet->bootp.hlen = BOOTP_HLEN_ETHERNET;
	packet->bootp.ciaddr = htonl(ipGetConfig()->ip);
	packet->bootp.yiaddr = HTONL(0l);
	packet->bootp.siaddr = HTONL(0l);
	packet->bootp.giaddr = HTONL(0l);
	nicGetMacAddress(&packet->bootp.chaddr[0]);	// fill client hardware address
	packet->bootp.xid = DhcpTransactID;			// set trans ID (use part of MAC address)
	packet->bootp.flags = HTONS(1);
	
	// build DHCP request
	// begin with magic cookie
	packet->cookie = 0x63538263;
	// set operation
	val = DHCP_MSG_DHCPRELEASE;
	optptr = dhcpSetOption(packet->options, DHCP_OPT_DHCPMSGTYPE, 1, &val);
	// set the server ID
	val = htonl(DhcpServerIP);
	optptr = dhcpSetOption(optptr, DHCP_OPT_SERVERID, 4, &val);
	// request the IP previously offered
	optptr = dhcpSetOption(optptr, DHCP_OPT_REQUESTEDIP, 4, &packet->bootp.ciaddr);

	// send release
	udpSend(DhcpServerIP, DHCP_UDP_SERVER_PORT, DHCP_HEADER_LEN+3+6+6+1, (uint8_t*)packet);
	
	// deconfigure ip addressing
	ipSetConfig(0,0,0);
	DhcpLeaseTime = 0;
}
*/


//****************************************************************************
// dhcp_process()
//******************************************************************************
/*! 
  \brief DHCP process, should be called peridocically, at leaset once a second.  If flag set to 1 re-init will take place.
  \return nothing.
*/
U8 dhcp_process(U8 flag)
{
	int ret=0;

	// only check if the second ticks
	if(dhcp_timer!=TIMER_Get_Second())
	{
		// update timer. 
		dhcp_timer=TIMER_Get_Second();
		// Update the lease time
		if(DhcpLeaseTime)
			DhcpLeaseTime--;
		//
		// If lease time has expired, renew the DHCP address
		//
		//dhcp_release();	
		if(0==DhcpLeaseTime)
		{
			if(flag)
			{
				ret=dhcp_init();
				if(0==ret)
					DhcpLeaseTime=10;			// retry in 10 seconds
			}
		}
	}
	return(ret);
}


//
//
//
U8* dhcpGetOption(U8* options, U8 optcode, U8 optlen, void* optvalptr)
{
	U8 i;

	// parse for desired option
	for (;;)
	{
		// skip pad characters
		if(*options == DHCP_OPT_PAD)
			options++;
		// break if end reached
		else if(*options == DHCP_OPT_END)
			break;
		// check for desired option
		else if(*options == optcode)
		{
			// found desired option
			// limit size to actual option length
			if(optlen> *(options+1))
				optlen= *(options+1);
			//if(*(options+1) < optlen)
			//	optlen = *(options+1);
			
			// copy contents of option
			for(i=0; i<optlen; i++)
				*(((U8*)optvalptr)+i) = *(options+i+2);
			// return length of option
			//return *(options+1); +++
			return (options+1);
		}
		else
		{
			// skip to next option
			options++;
			options+=*options;
			options++;
		}
	}
	// failed to find desired option
	return 0;
}


//
// Set a DHCP option in the BootP packet
// 
U8* dhcpSetOption(U8* options, U8 optcode, U8 optlen, U8* optvalptr)
{
	// use current options address as write point

	// set optcode
	*options++ = optcode;
	// set optlen
	*options++ = optlen;
	// copy in argument/data
	while(optlen--)
	{
		*options++ = *optvalptr++;
	}
	// write end marker
	*options = DHCP_OPT_END;

	// return address of end marker, to be used as a future write point
	return options;
}

