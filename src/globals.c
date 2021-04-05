/*! \file globals.c
    \brief globals definition and initialization memory map

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

/*					*/ 
/* include files 	*/
/*					*/ 
#include	"econfig.h"
//#include	"globals.h"
//#include	"console.h"
//#include	"debug.h"


U8	rx_pkt[MAX_RX_PACKET+4];

#ifdef WIN32

U8	pcap_iface=0;

int
readln_from_a_file(FILE *fp, char *line, int size)
{
	char *p;

    do
      p = fgets( line, size, fp );
    while( ( p != NULL ) && ( *line == '#') );

    if( p == NULL )
		return( 0 );

    if (strchr(line, '\n'))
          *strchr(line, '\n') = '\0';
    if (strchr(line, '\r'))
          *strchr(line, '\r') = '\0';
    return( 1 );
}
#endif

/*
// Serial config byte equates
#define MODEM_RTS 			0x1						// Set if using modem RTS
#define MODEM_CTS			0x2						// Set if using modem port CTS
#define CONSOLE_RTS			0x4						// Set if using console RTS
#define CONSOLE_CTS			0x8						// Set if using console CTS
#define RI_PASSTHROUGH	    0x10					// Pass through RI indicator
#define CD_PASSTHROUGH		0x20					// Pass through CD indicator 
*/

#if 0
typedef struct	MEMMAP_
{  
	U8			IMMConfig;				// IMM configuration Byte |DS|DPPP|x|DE|IE|R|ES|EC|
	U8			escapeChar;				// this is the char used for IMM escape to cmd mode
	U8			escape_timeout;			// this is the guard time between data and escap chars
	U8			TCPStreamTickTime;		// TCP send timer
	U8			UDPStreamTickTime;		// UDP send timer
	// Serial Ports
	U8			SerialConfig;			// Serial Config Byte |CAB|CD|CD|x|CCTS|CRTS|MCTS|MRTS|
	U8			ModemBaud;				// ModemBaudRate
	U8			ConsoleBaud;			// ConsoleBaudRate
	U8			DialTimeout				// Dialup Timeout. 
	// PPP configuration Vars -do we need retry counts
	U8			ppp_connect_timeout;	// PPP Connect timeout in seconds
	U32			ppp_ACCM;				// ACCM for link 
	// UDP config
	U8			udp_flags;				// |UDPDAVCHAR|DAVDSR|DAVDAV|DIRECT|LEN|IP|PORT
	// TCP configuration Vars
	U8			tcp_connect_timeout;	// In Seconds
	U16			tcp_retransmit_timeout;	// In 10's of MS
	// IP configuration vars
	U8			ip_ttl;
	U8			ip_tos;
	// IP address info
	IPAddr		our_ipaddr;
	IPAddr		peer_ip_addr;
	IPAddr		pri_dns_addr;
	IPAddr		sec_dns_addr;
	//
	// PPP username paramters
	//
	U8			user_name[40];
	U8			user_pass[40];
	//
	// Read Only, or status only regesters
	//
	U8			ircmd_state;			// command processor state (warning about writing)
	U8			ppp_flags;
	U8			ppp_flags2;
	U8			lcp_state;
	U8			pap_state;
	U8			ipcp_state;
	U16			ppp_tx_mru;
}MEMMAP; 
#endif


MEMMAP	mm;

//
// IMM Command Processor configuration Vars
//
/*
#pragma pack( 1)
U8			IMMConfig;				// IMM configuration Byte |DS|DPPP|x|DE|IE|R|ES|EC|
U8			escapeChar;				// this is the char used for IMM escape to cmd mode
U8			escapeTimeout;			// inband escape guard time in 10's MS
U8			TCPStreamTickTime;		// TCP send timer
U8			UDPStreamTickTime;		// UDP send timer

// Serial Ports
U8			SerialConfig;			// Serial Config Byte |CAB|CD|CD|x|CCTS|CRTS|MCTS|MRTS|
U8			ModemBaud;				// ModemBaudRate
U8			ConsoleBaud;			// ConsoleBaudRate

// PPP configuration Vars -do we need retry counts
U8			ppp_connect_timeout;	// PPP Connect timeout in seconds
U32			rx_accm;				// ACCM for link 
// TCP configuration Vars
U8			tcp_connect_timeout;	// In Seconds
U16			tcp_retransmit_timeout;	// In 10's of MS

// IP configuration vars
U8			ip_ttl;
U8			ip_tos;

// IP address info
IPAddr		our_ipaddr;
IPAddr		peer_ip_addr;
IPAddr		pri_dns_addr;
IPAddr		sec_dns_addr;

//
// Read Only, or status only regesters
//
U8			ircmd_state;			// command processor state (warning about writing)
U8			ppp_flags;
U8			lcp_state;
U8			pap_state;
U8			ipcp_state;
U16			ppp_tx_mru;
U32			tx_accm
*/

/*! \fn U8 global_init(U8 cmd)
    \brief Initializes the global vars

    \param cmd	0=load default	1=load eeprom
*/
U8
global_init(U8 cmd)
{
U8	count,*ptr=(U8 *)&mm;

#ifdef WIN32
U8		flag=0;
#endif


#ifdef __AVR__
	if(1==cmd)
	{
		// Try to load eeprom, 0xa5 is the flag that means eeprom is good.
		// uint8_t 	eeprom_read_byte (const uint8_t *addr)
		if((cmd) && (0xa5==eeprom_read_byte(0)))
		{
			//DEBUG1("Load EEPROM"CRLF);
			// Load the config from the eeprom
			for(count=0;count<sizeof(MEMMAP)-1;count++)
				ptr[count]=eeprom_read_byte((U8 *)(count+0xa));
		}
		else
		{
			//DEBUG1("EEPROM not valid ");
			// eeprom is not valid, load defaults
			cmd=0;
		}
		// Load MAC address no matter what
		mm.our_mac_addr[0]				= 0;
		mm.our_mac_addr[1]				= 0;
		mm.our_mac_addr[2]				= 0x14;
		mm.our_mac_addr[3]				= eeprom_read_byte(1);
		mm.our_mac_addr[4]				= eeprom_read_byte(2);
		mm.our_mac_addr[5]				= eeprom_read_byte(3);
	}
	if(2==cmd)
	{
		// Validate the eeprom
		eeprom_write_byte (0,0xa5);
		//
		for(count=0;count<sizeof(MEMMAP)-1;count++)
			eeprom_write_byte((count+0xa),ptr[count]);

	}
#else
	if(1==cmd)
	{
#define MAX_LINE_SIZE	128


		FILE	*fp;
		U8		line[MAX_LINE_SIZE];
		U8		*subst;

DEBUG1("read from file"CRLF);
		mm.our_mac_addr[0]				= 0;
		mm.our_mac_addr[1]				= 0;
		mm.our_mac_addr[2]				= 0x14;

		// Read from file
		if(NULL == (fp = fopen("unetconfig.txt", "r")) )
			cmd=0;
		else
		{	
			while(readln_from_a_file(fp, line, MAX_LINE_SIZE))
			{
				subst=strtok(line," \n");
				if(0==strcmp(subst,"iface"))
				{
					subst=strtok(NULL," \n");
					pcap_iface=d2s(subst);
				}
				if(0==strcmp(subst,"dhcp"))
				{
					subst=strtok(NULL," \n");
					if(1==d2s( subst))
					{
						// should we memset the IP addresses?
						mm.network_config=DHCP_ENABLE;
						break;
					}
				}
				else if(0==strcmp(subst,"ip"))
				{
					// Parse IP address
					subst=strtok(NULL," \n");
					s2ip( subst, &mm.our_ipaddr );
				}
				else if(0==strcmp(subst,"mask"))
				{
					// Parse IP address
					subst=strtok(NULL," \n");
					s2ip( subst, &mm.netmask );
				}
				else if(0==strcmp(subst,"gateway"))
				{
					// Parse IP address
					subst=strtok(NULL," \n");
					s2ip( subst, &mm.gatewayip);
				}
				else if(0==strcmp(subst,"pridns"))
				{
					// Parse IP address
					subst=strtok(NULL," \n");
					s2ip( subst, &mm.pri_dns_addr);
				}
				else if(0==strcmp(subst,"secdns"))
				{
					// Parse IP address
					subst=strtok(NULL," \n");
					s2ip( subst, &mm.sec_dns_addr);
				}
			}
			cmd=0;
			flag=1;
		}
	}
#endif
	if(0==cmd)
	{
		// 
		// Set defaults 
		//
		//DEBUG1("Load Defaults"CRLF);
#ifdef __AVR__
		mm.IMMConfig					= BOOT_BANNER | ECHO_CMD | ECHO_STREAM | RESULT ;
#else
		mm.IMMConfig					= BOOT_BANNER | DEBUG_MODE | ECHO_CMD | ECHO_STREAM | RESULT ;		
#endif
		mm.escapeChar					= '+';
		mm.escape_timeout				= ESCAPE_TIMEOUT;
		mm.TCPStreamTickTime			= 200;		
		mm.UDPStreamTickTime			= 200;
		mm.SerialConfig					= 0;							// serial config
		mm.ConsoleBaud					= CONSOLE_BAUD_STATIC;			// ConsoleBaudRate
		
		mm.ip_ttl						= IP_TTL_DEFAULT; 
		mm.ip_tos						= IP_TOS_DEFAULT;
		
		mm.tcp_connect_timeout			= TCP_CONNECT_TIMEOUT;
		mm.tcp_retransmit_timeout		= TCP_TIMETOUT;
		mm.dns_timeout					= DNS_REPLY_WAIT_TIME;	

		//
		// Changing info depending on product.
		//
#ifdef WIN32
		if(0==flag)
		{
#endif
/*
		mm.pri_dns_addr.ip32			= 0x0aa02840;
		// Default ip address
		mm.our_ipaddr.ip32	=0x0a0b0a0a;		// IP is backwards  10.10.11.10 = 0a00A8C0
		//mm.our_ipaddr.ip32	=0x0a00a8C0;		// IP is backwards  192.168.0.10 = 0a00A8C0
		mm.netmask.ip32		=0x00ffffff;
		mm.gatewayip.ip32	=0x010b0a0a;
*/
		mm.network_config		= DHCP_ENABLE;

		mm.pri_dns_addr.ip32	= 0x0;
		// Default ip address
		mm.our_ipaddr.ip32		=	0;				// IP is backwards  10.10.11.10 = 0a00A8C0
		//mm.our_ipaddr.ip32	=0x0a00a8C0;		// IP is backwards  192.168.0.10 = 0a00A8C0
		mm.netmask.ip32			=0x0;
		mm.gatewayip.ip32		=0x0;	
		//mm.gatewayip.ip32	=0x0100a8c0;
#ifdef WIN32		
		}
#endif
	}
	return(cmd);
}


