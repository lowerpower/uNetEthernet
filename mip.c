/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * mip.c - Mycal IP Layer													-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 1998									-        
 *
 * Exports:
 *	void	ip_upcall(U8 * buffer, U16 len)
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 1998, Mycal Labs www.mycal.com								-
 *                                                                         	-
 *---------------------------------------------------------------------------
 * Notes:
 *
*/

#include "mip.h"

#define NULL 0

void	(*ICMP_Callback)(struct ip_h *header,ICMP *data, U16 len);
U16				ipid;

//U8				headerbuffer[62];

IPADDR	myip;
IPADDR	netmask;
//U8	macaddr1[6];


//
// Experimental
//
U8		*arppend;
U16		arppendtime;


//****************************************************************************
// ipConfig() - some of these setting might be redundant
// 4 bytes myip
// 4 bytes mask
// 4 bytes gateway
// 12 bytes total storage req.
//******************************************************************************
/*! 
  \brief Configures the IP layer.
  \param myip - IP address to send to.
  \param *ip - pointer to incoming packet.
  \param len - length of incoming packet.
  \return nothing.
*/
void ipConfig(void)
{

	// set ARP association
//	nicGetMacAddress(&macaddr1);

#ifdef DEBUG_LV1
	DEBUG1(CRLF"Using MAC ");
	printMAC(mm.our_mac_addr);
	DEBUG1(CRLF"IP      :");
	printIP(&mm.our_ipaddr);
	DEBUG1(CRLF"Netmask :");
	printIP(&mm.netmask);
	DEBUG1(CRLF"Gateway :");
	printIP(&mm.gatewayip);
	DEBUG1(CRLF"Pri DNS :");
	printIP(&mm.pri_dns_addr);
	DEBUG1(CRLF"Sec DNS :");
	printIP(&mm.sec_dns_addr);
	DEBUG1(CRLF);
#endif

	arpSetAddress(mm.our_ipaddr, mm.our_mac_addr);
}



//****************************************************************************
// ip_upcall()
//******************************************************************************
/*! 
  \brief Incoming IP packet handler
  \param dest_ip - IP address to send to.
  \param *ip - pointer to incoming packet.
  \param len - length of incoming packet.
  \return nothing.
*/
void
ip_upcall(IP *ip, U16 len)
{

	// we could check flags and frags here.

	// Check Checksum, checksum should be zero for the header.
	if(Checksum_16(0, (U16 *)ip, 20))
	{
		// bad IP checksum, toss
		return;
	}

	// Check our IP address
	if(0!=mm.our_ipaddr.ip32)
	{
		if((ip->dest_ip.ip32!=0xffffffff) && (ip->dest_ip.ip32!=mm.our_ipaddr.ip32))
		{
			DEBUG2("bad dest ip\n");
			return;
		}
	}
	// Check frag flag, if so drop 
	if(swap(ip->flags) & IP_F_MF)
	{ 
		DEBUG2("Frag pkt\n");
		return;
	}

	// Lets make sure packet is in ARP table
	arpAdd(ip->source_ip, ((U8*)ip)-sizeof(MAC)+6);

	// Demux based on protocol.
	switch(ip->protocol)
	{
	case PROT_ICMP:
		DEBUG3("ICMP pkt\n"CRLF);
		icmp_upcall(ip,(ICMP *)ip->transport , swap(ip->length));
		break;
	case PROT_UDP:
		DEBUG3("UDP pkt\n"CRLF);
		udp_upcall(ip, (UDP *)ip->transport, swap(ip->length));
		break;
	case PROT_TCP:
		DEBUG4("TCP pkt"CRLF);
		tcp_upcall(ip, (TCP *)ip->transport, swap(ip->length));
		break;
	default:
		// Silently Discard all other protocols, maybe a counter here in the future.
		break;
	}
}



//****************************************************************************
// ip_send() - IP send routine, for now assume data pkt has room for header
// also that *data points to IP header.
//****************************************************************************
/*! 
  \brief Sends and IP packet to the desired IP address,
  \param dest_ip - IP address to send to.
  \param protocol- Protocol of the transport layer (IE UDP/TCP/ICMP)
  \param *data - pointer to the data to send.
  \param len - Length of the data to send.
  \return -1 if send failed, 0 if OK.
*/
//ip_send(ip,data,(sizeof(IP)+sizeof(UDP)),len);	// IP_Send(header,data,len);
//void
//ip_send(IP *dest_ip, U8 protocol, IP *data, U16 len)

S8
ip_send(IP *ip, U8 *data, U16 iplen, U16 datalen)
{
	S8	ret=0,t=0;
	MAC	*mac;
	U8	*next_hop;
	//U16	tlen;
	U8	broadcast[6];


	// Get MAC header pointer into header
	mac=(MAC*) ((U8*)(ip)-sizeof(MAC));
	//tlen=datalen+iplen;				// +++
	//
	// Set IP packet type
	//
	mac->ether_type=ETHERTYPE_IP;
	// 
	// Build the IP header
	//
	ip->version=IP_VERSION;
	ip->tos=mm.ip_tos;
	ip->length=swap((datalen+iplen));
	ip->id=ipid++;
	ip->ttl=mm.ip_ttl;
	ip->flags=0;

	//ip->protocol=protool;
	ip->source_ip.ip32=mm.our_ipaddr.ip32;
	//ip->dest_ip.ip32=dest_ip.ip32;
	ip->checksum=0;
	ip->checksum=Checksum_16(0, (U16 *)ip, 20);
	//
	// Simple subnet routing for now, just check the mask for local route or to the gateway, if
	// we add more than one interface or need more than one gateway we need a router here.
	//
	while(1)
	{
		if(0xffffffff==ip->dest_ip.ip32)
		{
			// Broadcast
			next_hop=broadcast;
			broadcast[0]=0xff;
			broadcast[1]=0xff;
			broadcast[2]=0xff;
			broadcast[3]=0xff;
			broadcast[4]=0xff;
			broadcast[5]=0xff;
		}
		// Local or Remote Route?
		else if((ip->dest_ip.ip32 & mm.netmask.ip32) == (mm.our_ipaddr.ip32 & mm.netmask.ip32))
		{
			// Local Route
			DEBUG2("local route ");
			next_hop=arpMatchIp(&ip->dest_ip);
		}
		else
		{
			// GW route
			DEBUG2("gateway route ");
			next_hop=arpMatchIp(&mm.gatewayip);
			t=1;
		}	
		
		// Sucessfull lookup?
		if(0==next_hop)
		{
			//DEBUG1("cache miss ");
			// No send ARP request, either for gateway or local IP, wait 200ms and try again (but only once)
			if(1==t)
				arpRequest(&mm.gatewayip);
			else
				arpRequest(&ip->dest_ip);

			TIMER_Wait_Tick(30);
			poll_serial();
		}
		else
		{
			// Copy MAC address over to packet and then send packet out interface
			bytecopy(mac->dest_hw_add,next_hop,6);
			bytecopy(mac->src_hw_add,mm.our_mac_addr,6);		// set our src addr
			nicSendMulti(mac, iplen+sizeof(MAC), data, datalen);
			//void nicSendMulti(U8 *, U16, U8 *, U16);
			ret=0;
			break;
			//	dump(mac,64);
		}
		if(-1==ret)
			break;
		ret--;
	}

	return(ret);
}


//																	www.mycal.net
//*******************************************************************************
// Checksum_16(seed, buffer, len)
//	Calculates checksums for IP headers and TCP, UDP and ICMP packets.
//
//	Set seed to zero for new checksum calculation, or from previous result
//	to continue checking checksum.
//  
//*******************************************************************************
U16
Checksum_16(U16 start,U16 *data, U16 len)
{
U16				sum=~start;

	// Main Checksum Loop
	while(len > 1)
	{
		len-=2;
		sum += *data;
		if(sum < *data)
			sum++;		// Add in carry
 		data++;
	}
	// Check for odd byte, if there handle it.
	if(len == 1) 
	{
		len=*data & 0xff;
		sum+=len;
		if(sum<len)
			sum++;
    }
  	// Return the one's complement
	return (~sum);
}

U16 Checksum(U16 start,U8 *addr, U16 count)
{
U32				sum=~start;

  // Main summing loop
  while(count > 1)
  {
    sum = sum + (*((U16 *) addr))++;
    count = count - 2;
  }

  // Add left-over byte, if any
  if (count > 0)
    sum = sum + *((U8 *) addr);

  // Fold 32-bit sum to 16 bits
  while (sum>>16)
    sum = (sum & 0xFFFF) + (sum >> 16);

  return(~sum);
}





void
ICMP_Echo_Callback(void (*callback)(struct ip_h *header,ICMP *data, U16 len))
{
	ICMP_Callback=callback;
}

void
ICMP_Echo_Terminate()
{
	ICMP_Callback=0;
}


U8
ICMP_Send_Ping(IPAddr *dest_ip, U16 len)
{
MEMBLOCK	*memblock;
IP			*ip;
ICMP		*icmp;
U16			i;

	if(len>64)
		len=64;

	memblock=MEM_GetBlock();
	if(memblock)
	{
		ip=(IP *)(memblock->data+sizeof(MAC));

		ip->protocol=PROT_ICMP;
		ip->source_ip.ip32=mm.our_ipaddr.ip32;
		ip->dest_ip.ip32=dest_ip->ip32;
		ip->checksum=0;

		// Build ECHO Request packet
		icmp=(ICMP *)ip->transport;
		icmp->type=ICMP_ECHO_REQUEST;
		icmp->code=0;
		icmp->checksum=0x0000;
		icmp->id=1;
		icmp->seq=1;

		for(i=0;i<len;i++)
			icmp->data[i]=(i&0xff);

		icmp->checksum=Checksum_16((U16)0, (U16 *)icmp, (U16)(len+sizeof(ICMP)) );
		
		ip_send(ip,(U8 *)0,(U16)(sizeof(IP)+sizeof(ICMP)+len),0);	// IP_Send(header,data,len);

		MEM_FreeBlock(memblock);
		return(1);
	}
	return(0);
}


//-----------------------------------------------------------------------------
// icmp_upcall()
//-----------------------------------------------------------------------------
void
icmp_upcall(IP *ip, ICMP *icmp, U16 len)
{
//U8		*m;

	//
	// Check the ICMP checksum (we can omit this if we want)
	//

	switch(icmp->type)
	{
    case	ICMP_ECHO_REPLY:
			// Allow callback here, call it if defined.
			if((void *)ICMP_Callback!=0)
			{
				ICMP_Callback(ip,icmp,len);
			}
			break;
    case	ICMP_ECHO_REQUEST:
		
			DEBUG2("Build reply"CRLF);
			//
			// Build reply inline
			//
			icmp->type=ICMP_ECHO_REPLY;
			icmp->checksum+=0x0008;				// Fixup checksum
			//
			// Get dest IP, source will be filled in later
			ip->dest_ip.ip32=ip->source_ip.ip32;
			//
			//
			//
			//ip_send(&ip->dest_ip, PROT_ICMP, ip, len);			
			//
			// Whole packet is in IP header spot, don't send data spot
			//
			ip_send(ip, NULL, len, 0);
			break;
#if 0
			// Future use
    case	ICMP_DEST_UNREACHABLE:
    case 	ICMP_SOURCE_QUENCH:
    case	ICMP_REDIRECT: 
    case	ICMP_ROUTER_ADVERT:
    case	ICMP_ROUTER_SOLICIT:
    case	ICMP_TIME_EXCEEDED:
    case	ICMP_PARAMETER_PROBLEM:
    case	ICMP_TIMESTAMP_REQEST:
    case	ICMP_TIMESTAMP_REPLY:
    case	ICMP_MASK_REQUEST:
    case	ICMP_MASK_REPLY:
		break;
#endif    
	default:
			//icmp_badtype++;
		break;
	}
}


