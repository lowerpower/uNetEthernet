/*! \file arp.c \brief ARP Protocol. */
//*****************************************************************************
//
// File Name	: 'arp.c'
// Title		: ARP Protocol Library
// Author		: Mycal Labs
// Created		: 9/7/2004
// Revised		: 7/3/2005
// Version		: 0.1
//
//*****************************************************************************

#include "arp.h"



// global variables

ARPENTRY	MyAddr;						///< Local IP to MAC address translation
ARPENTRY	ArpCache[ARP_CACHE_SIZE];	///< ARP cache of IP to MAC assocations

#if 0
// Dump a packet
void dump(U8 *pkt, U16 len)
{
	U16 i,j;


	CONSOLE_puts(CRLF"Addr 0  1  2  3  4  5  6  7    8  9  A  B  C  D  E  F"CRLF);
	CONSOLE_puts("----------------------------------------------------------"CRLF);

	for(i=0;i<len;i=i+16)
	{
		printWord(pkt+i);
		CONSOLE_puts(":");
		// Print Hex
		for(j=0;j<16;j++)
		{
			printByte(pkt[i+j]);
			if(7!=j)
				CONSOLE_puts(" ");
			else
				CONSOLE_puts(" - ");
		}	
		// Print ASCII
		CONSOLE_puts("   ");
		// Print Hex
		/*for(j=0;j<16;j++)
		{
			if((pkt[i+j]>31) && (pkt[i+j]<127))
				printByte(pkt[i]);
			else
				printByte(".");
		}
		*/
		CONSOLE_puts(CRLF);
	}
}

#endif

void arpInit(void)
{
	U8 i;
	// Init ARP cache
	for(i=0; i<ARP_CACHE_SIZE; i++)
	{
		ArpCache[i].ipaddr.ip32 = 0;
		ArpCache[i].time = 0;
	}
}


// Combind with above!!
void arpSetAddress(IPADDR ipaddr, U8 *mac_addr)
{
	// Initalize Local IP to MAC translation
	bytecopy(MyAddr.macaddress,mac_addr,6);
	MyAddr.ipaddr.ip32=ipaddr.ip32;
}


// Handel incoming ARP packets
void
arpHandler(U8 *pkt, U16 len)
{
	ARP				*arp;

	arp=(ARP *) pkt;

	// Check if this is an ARP request.
	if(ARP_REQUEST==arp->op)
	{
		// ARP Request

		//DEBUG2("ARP-REQUST My IP ");
		//printIP(&MyAddr.ipaddr);
		//DEBUG2(" requested ip is ");
		//printIP(&arp->d_ip_add);
		//DEBUG2(CRLF);
//		dump(arp,64);

		// Is ARP request for our IP?
		if(arp->d_ip_add.ip32 == MyAddr.ipaddr.ip32)
		{

			DEBUG2(CRLF"add-req - ");

			// Add requesting IP to ARP cache?  Fix this call, corrupts mac-ip addr
			//arpAdd(arp->s_ip_add, arp->s_hw_add);

			//  Then respond by fixing up packet
			bytecopy(arp->dest_hw_add,arp->src_hw_add,6);
			bytecopy(arp->d_hw_add,arp->src_hw_add,6);
			arp->d_ip_add.ip32 = arp->s_ip_add.ip32;
			arp->s_ip_add.ip32 = MyAddr.ipaddr.ip32;
			bytecopy(arp->src_hw_add,MyAddr.macaddress,6);
			bytecopy(arp->s_hw_add,MyAddr.macaddress,6);

			arp->op=ARP_REPLY;
		//	DEBUG2(" Nic Send len");
		//	printShort(len);
			DEBUG2(CRLF);
			nicSend(len-4, pkt);
		}
		else
		{
			// Not for us
		}

	} 
	else if (ARP_REPLY==arp->op)
	{
		// Did we request this?
		// Yes lets add it to the cache.
		arpAdd(arp->s_ip_add, arp->s_hw_add);

	}
}

/*! 
  \brief Adds an ARP address to the ARP cache.
  \param dest_ip - IP address to send arp request to
  \return nothing.
*/
void
arpAdd(IPADDR sip, U8 *mac)
{
	U8		i;
	U8		old;

	// This is an ARP reply, add into cache.
	old=0;
	for(i=0; i<ARP_CACHE_SIZE; i++)
	{
		// Check to see if address matches cache.
		if(sip.ip32 == ArpCache[i].ipaddr.ip32)
		{
			DEBUG2("refresh entry"CRLF);
			// We have a match, update time
			ArpCache[i].time = ARP_CACHE_TTL;
			break;
		}
		// else lets keep track of the oldest one.
		if (ArpCache[i].time < ArpCache[old].time)
			old=i;
	}
	// If not found replace oldest
	if(i == ARP_CACHE_SIZE)
	{
		DEBUG2("Overwrite oldest"CRLF);
		// Replace oldest
		ArpCache[old].ipaddr.ip32 = sip.ip32;
		bytecopy(ArpCache[old].macaddress, mac, 6);
		ArpCache[old].time = ARP_CACHE_TTL;
	}
}

// send an arp request (uses lots of stack, can change later)
/*! 
  \brief Sends and ARP reqest packet out the interface.
  \param dest_ip - IP address to send arp request to
  \return nothing.
*/
void 
arpRequest(IPADDR *destIp)
{
	ARP	pkt;

	// build an arp request for the IP address requested.
	bytecopy(pkt.src_hw_add,MyAddr.macaddress,6);
	bytecopy(pkt.s_hw_add,MyAddr.macaddress,6);
	// broadcast
	pkt.dest_hw_add[0]=pkt.dest_hw_add[1]=pkt.dest_hw_add[2]=pkt.dest_hw_add[3]=pkt.dest_hw_add[4]=pkt.dest_hw_add[5]=0xff;

	pkt.ether_type		=ARP_TYPE;
	pkt.hard_type		=ARPHRD_ETHER;
	pkt.prot_type		=ETHERTYPE_IP;
	pkt.hard_size		=6;
	pkt.prot_size		=4;
	pkt.op				=ARP_REQUEST;
	pkt.s_ip_add.ip32	= MyAddr.ipaddr.ip32;
	pkt.d_ip_add.ip32	= destIp->ip32;

	nicSend(sizeof(ARP), &pkt);

}


//-----------------------------------------------------------------------------
// arp_process() - 
//
//	This should be called every 10 seconds or so.  
// 
//-----------------------------------------------------------------------------
void
arpProcess(void)
{
	U8 i;

	// decrement TTL for all non zero entries
	for(i=0; i<ARP_CACHE_SIZE; i++)
	{
		if(ArpCache[i].time)
			ArpCache[i].time--;
	}
}

/*! 
  \brief ARP matching function.
  \param dest_ip - pointer to the IP address to match in ARP table.
  \return Returns a pointer to the cache entry.
*/
U8 
*arpMatchIp(IPADDR *ip)
{
	U8	i;
	
	// Search through the arp cache and try to find a match
	for(i=0; i<ARP_CACHE_SIZE; i++)
	{
		if(ArpCache[i].ipaddr.ip32 == ip->ip32)
		{
			DEBUG2("found entry"CRLF);
			// Found, return pointer to MAC address
			return(ArpCache[i].macaddress);
		}
	}
	// Return 0 if not found
	return 0;
}



#ifdef ARP_DEBUG_PRINT
void arpPrintHeader(struct netArpHeader* packet)
{
	rprintfProgStrM("ARP Packet:\r\n");
	//debugPrintHexTable(60, (unsigned char*)&packet);
	// print operation type
	rprintfProgStrM("Operation   : ");
	if(packet->opcode == htons(ARP_OPCODE_REQUEST))
		rprintfProgStrM("REQUEST");
	else if(packet->opcode == htons(ARP_OPCODE_REPLY))
		rprintfProgStrM("REPLY");
	else
		rprintfProgStrM("UNKNOWN");
	rprintfCRLF();
	// print source hardware address
	rprintfProgStrM("SrcHwAddr   : ");	netPrintEthAddr(&packet->shwaddr);	rprintfCRLF();
	// print source protocol address
	rprintfProgStrM("SrcProtoAddr: ");	netPrintIPAddr(HTONL(packet->sipaddr));	rprintfCRLF();
	// print target hardware address
	rprintfProgStrM("DstHwAddr   : ");	netPrintEthAddr(&packet->dhwaddr);	rprintfCRLF();
	// print target protocol address
	rprintfProgStrM("DstProtoAddr: ");	netPrintIPAddr(HTONL(packet->dipaddr));	rprintfCRLF();
}


void arpPrintTable(void)
{
	uint8_t i;

	// print ARP table
	rprintfProgStrM("Time    Eth Address    IP Address\r\n");
	rprintfProgStrM("---------------------------------------\r\n");
	for(i=0; i<ARP_TABLE_SIZE; i++)
	{
		rprintfu08(ArpTable[i].time);
		rprintfProgStrM("   ");
		netPrintEthAddr(&ArpTable[i].ethaddr);
		rprintfProgStrM("  ");
		netPrintIPAddr(ArpTable[i].ipaddr);
		rprintfCRLF();
	}
}
#endif
