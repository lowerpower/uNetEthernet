/*																www.nchip.com
 *---------------------------------------------------------------------------
 * emain.c - Main file for the Ethernet IR command interpreter software		-
 *---------------------------------------------------------------------------
 * Version                                                                  -                                            
 *		0.1 Original Version Jan 21, 2005    								-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2005, nChip, All Rights Reserved.							_
 *															               	_
 *---------------------------------------------------------------------------
*/
//
// Include file for the project
//
#include "econfig.h"
#include "emain.h"


#ifdef WINDOWS
#include <signal.h>
#include <time.h>
#else

#ifdef DEBUG_LV1
FILE uart_str = FDEV_SETUP_STREAM(CONSOLE_putch, NULL, _FDEV_SETUP_WRITE);
#endif

#endif



//#include "ports.h"
//#include "globals.h"
//#include "debug.h"

//
// Boot Loader Call
//
#ifdef MEGA128
short (*boot_version)(void)=(void *)0xfE03;
int (*crc_boot)(void)=(void *)0xfE06;
int (*bootloader)(U16 baud)=(void *)0xfE00;
#endif

#ifdef MEGA64
short (*boot_version)(void)=(void *)0x7003;
int (*crc_boot)(void)=(void *)0x7006;
int (*bootloader)(U16 baud)=(void *)0x7000;
int (*absolute)=(void *)0x800100;
#endif

//
// 
//
U8	smbuff1[25];
U8	smbuff2[25];
U8	smbuff[16];


boot_addr()
{
U16 c;
		CONSOLE_puts("	-Bootloader address ");
		c=(U16)*boot_version;
		printWord(c);
		CONSOLE_puts(CRLF);
}



U8 *
bytecopy(U8 *dest,U8 *source, U16 length)
{
	U16 i=0;

	if((dest) && (source))
	{
		while(i<length)
		{
			dest[i]=source[i];
			i++;
		}
	}
	else
		return(dest);

	return(&dest[i]);
}

void
setup_hw(void)
{
#ifdef __AVR__
	DDRA=PORTADIR;						// outp(PORTADIR,DDRA);
	DDRB=PORTBDIR;						// outp(PORTBDIR,DDRB);
	DDRC=PORTCDIR;						// outp(PORTCDIR,DDRC);
	DDRD=PORTDDIR;						// outp(PORTDDIR,DDRD); 
	DDRE=PORTEDIR;						// outp(PORTEDIR,DDRE);
	DDRG=PORTGDIR;						// outp(PORTGDIR,DDRG);

	// weak pullups on some inputs
	//outp(~PORTADIR,PORTA);  not used
	PORTB=~PORTBDIR;					// outp(~PORTBDIR,PORTB);
	PORTC=~PORTCDIR;
	PORTD=~PORTDDIR;
	PORTE=~PORTEDIR;
	PORTG=~PORTGDIR;
	//outp(~PORTCDIR,PORTC);
	//outp(~PORTDDIR,PORTD);
	//outp(~PORTEDIR,PORTE);
	//outp(~PORTGDIR,PORTG);
	
#endif
}


//
// Poll_serial() - This should be in a different file, this should be platform specific
//
// Get a char from the modem port and process it.  This function is re-entrent and
// is internally protected by a semiphor.
//
U8	poll_semi=0;
void
poll_serial()
{
	U16			plen,i;
	MEMBLOCK	*memblock;
	U8			*packet;	
	U8			mac[6];

	if(poll_semi)
		return;

	poll_semi=1;

	nicGetMacAddress(mac);

	packet=rx_pkt;
	//for(i=0;i<128;i++)packet[i]=0xfe;
	
	// On receive we use our own buffer and process packets until there are no more.
	plen=1;
	while(plen)
	{
		plen= nicPoll(MAX_RX_PACKET, packet);
		if(plen)
		{
			// Filter for our MAC or broadcast
			if( ((0xff==packet[0]) && (0xff==packet[1]) && (0xff==packet[2]) &&
				(0xff==packet[3]) && (0xff==packet[4]) && (0xff==packet[5])) ||
				(0==memcmp(packet,mac,6) ))
			{
				// Demux
				// Parse packet, either ARP or IP
				if(0x08==packet[12])
				{
					if(0x00==packet[13])
					{
						// IP
						DEBUG2("IP Packet"CRLF);
						ip_upcall(&packet[14], plen-14);
					}
					else if(0x06==packet[13])
					{
						// ARP
						DEBUG2("ARP Packet");
						arpHandler(packet, plen);
					}
				}
			}
		}
	}
	poll_semi=0;
}

#ifdef WIRELESS
wireless()
{
	// setup SPI I/O pins
	sbi(PORTB, 1);	// set SCK hi
	sbi(DDRB, 1);	// set SCK as output
	cbi(DDRB, 3);	// set MISO as input
	sbi(DDRB, 2);	// set MOSI as output
	sbi(DDRB, 0);	// SS must be output for Master mode to work
	// initialize SPI interface
	// master mode
	sbi(SPCR, MSTR);

spi_readFromSlaveReg(HOST_S2M_MAIL_BOX_REG2, &parser_stat );

#if 0
	// select clock phase positive-going in middle of data
	cbi(SPCR, CPOL);
	// Data order MSB first
	cbi(SPCR,DORD);
	// switch to f/4 2X = f/2 bitrate
	cbi(SPCR, SPR0);
	cbi(SPCR, SPR1);
	sbi(SPSR, SPI2X);
	// enable SPI
	sbi(SPCR, SPE);
#endif
}

#endif


U16
nic_intialize()
{
	//
	// Reset ETH controller for good mesure
	//
	ERST_ON();
	TIMER_Wait_Tick(200);
	ERST_OFF();
	TIMER_Wait_Tick(50);
	//
	// Initialize NIC
	//
	DEBUG1("Nic Init"CRLF);
#ifdef __AVR__
	WDTCR=0x0d;							// 0x    0000 1101			enbable watch dog timeour .5 seconds
#endif	
	while(-1==nicInit())
	{
		CONSOLE_puts("NIC initialization failure.");
		TIMER_Wait_Tick(100);
	}

	DEBUG1("Nic Init out"CRLF);
#ifdef __AVR__
	WDTCR=0x1d;							// Clear watch dog timer!!!!!!!!!!!!!!!!!!!!!!!!
	WDTCR=0x05;
	// Set MAC
	nicSetMacAddress(mm.our_mac_addr);



#ifdef DEBUG_LV5 
	while(1)
	{
		enc28j60RegDump();//stdout = &uart_str;
		CONSOLE_puts(CRLF);
		TIMER_Wait_Tick(5);
	}
	enc28j60RegDump();//stdout = &uart_str;
#endif

#endif
	// Init ARP
	arpInit();
	// Init IP
	ipConfig();

	TIMER_Wait_Tick(200);

}






#ifdef WINDOWS

void
eexit(void)
{
	printf("**********");
	//MODEM_close();
	exit(1);
}

void
main(argc,argv)
int 	argc;
char 	**argv;
{
int		port=1;
int		speed=19200;

	signal(SIGINT,eexit);
/*
	if (argc==3)
	{   
		// Get com port
		port=atoi(argv[1]);
		// Get com speed
		speed=atoi(argv[2]);
    }
	else
	{
		printf("Warning: must specify IP address Gateway and netmask to not use defaults.\n");
		printf("	Correct format is unete <ipaddress> <gatewayip> <netmask>\n");
		printf("	Example: ppp 192.168.1.10 192.168.1.1 255.255.255.0\n\n");
	}
*/

#else

int
main()
{
int	port=1;
int c;
U16	plen;
IPADDR	ipaddr;
U8	buf[13];

//eeprom_write_byte (0,0);
#endif


	// Setup the hardware pins
	setup_hw();
	// Initialize the Timer
	TIMER_Init();
	// start reset seq of eth controller
	ERST_OFF();
	// Enable the interrupts
	sei();


	IND2_OFF();
	IND3_OFF();
	TIMER_Wait_Tick(20);
	//
	// Intialize the config to defaults (change this to 1 !!!+++)
	global_init(1);
	IND2_OFF();
	TIMER_Wait_Tick(20);
	// If set to autobaud, autobaud the Console Serial Port and save it to mm
#ifdef __AVR__	

	if(mm.SerialConfig & AUTOBAUD)
	{
		c=autobaud_setup();
		mm.ConsoleBaud=c;
	}
	else
	{
		// Load default baud/ no autobaud
		c= mm.ConsoleBaud;
	}

	// Initialize the Console Port 
	CONSOLE_init(c);
	IND2_OFF();
	IND3_OFF();
	RI0_OFF();

#endif

#ifdef __AVR__
#ifdef DEBUG_LV3	
	CONSOLE_puts("autobaud = ");
	printShort(c);
	DEBUG1(CRLF);
#endif

	//
	// Print the hello message
	//
	if(mm.IMMConfig & BOOT_BANNER)
	{
		TIMER_Wait_Tick(20);
		CONSOLE_puts("uNetEthernet, Embedded Internet TCP/IP stack over Ethernet, Beta - Version "EM_VERSION CRLF);
		CONSOLE_puts("	-Beta Version build on " __DATE__ " at " __TIME__ CRLF);
	}
	TIMER_Wait_Tick(20);
#endif

#ifdef __AVR__	
	//
	// This is where the AVR bootloader checker is, we should also put the CRC routing
	// in the bootloader since it will never change.
	//
	if(mm.IMMConfig & BOOT_BANNER)
	{
		CONSOLE_puts("	-Bootloader Version ");
		c=boot_version();
		printWord(c);
		CONSOLE_puts(CRLF);
	}
#endif

	// Initialize memory
	DEBUG1("Initialize Memory Pool"CRLF);
	MEM_init();
	DEBUG1("out init"CRLF);
// Initialize modem
//	DEBUG1("Initialize camera serial port"CRLF);
//	MODEM_init(MODEM_BAUD_STATIC);								// camera talks at 14.4K to start
//	DEBUG1("out init"CRLF);

	
//	if(MODEM_Init_Camera())
//		DEBUG1("Camera Initialized"CRLF);
//	else
//		DEBUG1("Camera Failed Initialization"CRLF);
	
	// 
	// Get a picture
	//
//	MODEM_Camera_SendCmd(05,0,0,0,0);			// Snapshot compressed
//	MODEM_Camera_SendCmd(04,1,0,0,0);			// Get picture
	
#ifdef WIRELESS
	// wireless init?
	DEBUG1("enter wireless test"CRLF);
	wireless();
	DEBUG1("exit wireless test"CRLF);
	while(1);
#endif	
	
	nic_intialize();

//	if(mm.network_config & DHCP_ENABLE)
//	{
		// Try to aquire a DHCP address
//		if(0==dhcp_init())
//			CONSOLE_puts("DHCP_OK"CRLF);
//		else
//			CONSOLE_puts("DHCP_FAIL"CRLF);
//	}
//	else
//		CONSOLE_puts("NO DHCP"CRLF);


	// command processor loop, runs forever
	irCommands();

}


