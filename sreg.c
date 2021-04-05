
/*															
 *---------------------------------------------------------------------------
 * sreg.c - S Register Processor											-
 *	All S register commands are handled here								-
 *---------------------------------------------------------------------------
 * Original Version                                                         -
 *  Nov 7 2000	- mwj                                                       -
 *                                                                          -
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2000, iReady Corporation, All Rights Reserved.				_
 *                                                                         	_
 *---------------------------------------------------------------------------
 * Changelog
 *
 *---------------------------------------------------------------------------
*/

#include "sreg.h"

/*
U8
*Get_Byte(U8 *ptr, U8 *val)
{
	//
	// Dump Spaces
	//
	while(*ptr==' ')
			ptr++ ;

	smbuff[0]=(*ptr++ | 0x20);
	smbuff[1]=(*ptr++ | 0x20);
	smbuff[2]=0;
	
	if('='==smbuff[1])
	{
		ptr--;
	}

	// Convert
	*val=X2s(&smbuff[0]); 
	return(ptr);
}
*/


//
//		0-->N				Config Register Memory Map in Hex
//
//		e					EEPROM
//
//		x					raw S7600 read/write
//
//		b					S7600 interface baud rate
///		0-1					Socket Status
//      2-3					Socket Source Port
//      4					Our IP address
//		5					DNS server pri
//		6					DNS server sec
//      7					Modes
//
U8
sRegister(U8 *ptr)
{
U8		msg=0xff; 
U8		soc=1,s=1;
U16		t=0;
U8		*tptr=(U8 *)&mm;
//U16     t;
IPAddr 	ip;
	//
	// Dump Spaces
	//
	while(*ptr==' ')
			ptr++ ;
	//
	// Decode 
	//
	ptr=Get_Byte(ptr,&s);

#ifdef DEBUG_LV4	
	DEBUG2("sreg=");
	printShort(s);
	DEBUG2(CRLF);
#endif
	//
	// Dump Spaces
	//
	while(*ptr==' ')
			ptr++ ;	

	if('='==*ptr++)
		t=1;
	//
	// Operate on sreg
	//
	switch(s)
	{
		case 1:
			// Return socket 1 status      
			soc=2;
		case 0:
			// Return socket 0 status   
			//
			// See if this socket is up,  Returns socket state and source port
			//		
			t=tcp_status(soc);
			printByte((U8)t);
			CONSOLE_putch(' ');
			t=tcp_sport(soc);
			printWord(t);
			break;	
		case 2:
			//
			// Return UDP status,   Returns Len of data ready, peer ip, peer source port
			//
			if(0==udp_ptr)
				printShort(0);
			else
				printShort(1);
			CONSOLE_putch(' ');
			printShort(udp_len);
			CONSOLE_putch(' ');
			printIP(&udp_ip);
			CONSOLE_putch(' ');
			printShort(udp_sp);
			break;
		case 3:
			//
			// Print SW version
			//
			msg=MSG_VERSION;
			break;
		case 4:
			//
			// Print boot loader Version
			//
#ifdef __AVR__	
#ifndef DEMO
			t=boot_version();
#else
			t=1;
#endif
#else
			t=1;
#endif
			printWord(t);
			//msg=MSG_NULL;
			break;
		case 5:
			//
			// Our MAC address
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the MAC address    
				// Dump first 3 bytes that are non setable
				ptr=Get_Byte(ptr,&soc);	
				ptr=Get_Byte(ptr,&soc);	
				ptr=Get_Byte(ptr,&soc);
				ptr=Get_Byte(ptr,&mm.our_mac_addr[3]);	
				ptr=Get_Byte(ptr,&mm.our_mac_addr[4]);	
				ptr=Get_Byte(ptr,&mm.our_mac_addr[5]);
#ifdef __AVR__				
				eeprom_write_byte(1,mm.our_mac_addr[3]);
				eeprom_write_byte(2,mm.our_mac_addr[4]);
				eeprom_write_byte(3,mm.our_mac_addr[5]);
#endif			
			}	
			//nicGetMacAddress(&macaddr1);
			printMAC(mm.our_mac_addr);
			break;
		case 6:
			//
			// Our IP address
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the IP address    
				if(ptr!=(U8 *)s2ip(ptr, &ip ))
				{         
					mm.our_ipaddr.ipb1=ip.ipb1;
					mm.our_ipaddr.ipb2=ip.ipb2;
					mm.our_ipaddr.ipb3=ip.ipb3;
					mm.our_ipaddr.ipb4=ip.ipb4;
				}			
			}
			printIP(&mm.our_ipaddr);
			break;
		case 7:
			//
			// Our Netmask 		mm.mm.netmask
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the IP address    
				if(ptr!=(U8 *)s2ip(ptr, &ip ))
				{         
					mm.netmask.ipb1=ip.ipb1;
					mm.netmask.ipb2=ip.ipb2;
					mm.netmask.ipb3=ip.ipb3;
					mm.netmask.ipb4=ip.ipb4;
				}			
			}	
			printIP(&mm.netmask);
			break;
		case 8:
			//
			// Our Gateway IP 		mm.gatewayip
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the IP address    
				if(ptr!=(U8 *)s2ip(ptr, &ip ))
				{         
					mm.gatewayip.ipb1=ip.ipb1;
					mm.gatewayip.ipb2=ip.ipb2;
					mm.gatewayip.ipb3=ip.ipb3;
					mm.gatewayip.ipb4=ip.ipb4;
				}			
			}	
			printIP(&mm.gatewayip);
			break;
		case 9:
			//
			// Primary DNS
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the IP address    
				if(ptr!=(U8 *)s2ip(ptr, &ip ))
				{         
					mm.pri_dns_addr.ipb1=ip.ipb1;
					mm.pri_dns_addr.ipb2=ip.ipb2;
					mm.pri_dns_addr.ipb3=ip.ipb3;
					mm.pri_dns_addr.ipb4=ip.ipb4;
				}			
			}	
			printIP(&mm.pri_dns_addr);
			break;
		case 0xa:
			//
			// Secondary DNS
			//                    
			// Check Read or Write
			//                    
			if(1==t)
			{
				// Convert the IP address    
				if(ptr!=(U8 *)s2ip(ptr, &ip ))
				{         
					mm.sec_dns_addr.ipb1=ip.ipb1;
					mm.sec_dns_addr.ipb2=ip.ipb2;
					mm.sec_dns_addr.ipb3=ip.ipb3;
					mm.sec_dns_addr.ipb4=ip.ipb4;
				}			
			}
			printIP(&mm.sec_dns_addr);
			break;
		case 0xb:
			// Console Baud Rate
			if(1==t)
			{
				if(tptr!=(U8 *)s2p(ptr,&t))
				{
					//printShort(t);
					//DEBUG1(CRLF);
					mm.ConsoleBaud=(U8)t;
#ifndef WINDOWS
			//		MODEM_init(t);							/// bug!
#endif
				}
				else
					msg=MSG_ERROR;
			}
			if(msg!=MSG_ERROR)
			{
				printShort(mm.ConsoleBaud);
			}
			break;
#ifdef __AVR__	
		case 0xc:
			// 
			// PortF GPIO Direction Pin
			//
			if(1==t)
			{
				Get_Byte(ptr,&soc);
				DDRF=soc;				//outp(soc,DDRF);
			}
			soc=DDRF;					//soc=inp(DDRF);
			printByte(soc);
			break;
		case 0xd:
			// 
			// PortF GPIO pin  PINF? +++
			//
			if(1==t)
			{
				Get_Byte(ptr,&soc);
				PORTF=soc;				//outp(soc,PORTF);
			}
			PORTF=soc;					//soc=inp(PORTF);
			printByte(soc);
			break;
#endif
		case 0xe:
			//
			// DHCP lease time
			//
			printDWord(DhcpLeaseTime);
			break;
		default:
			if(s<(sizeof(MEMMAP)+0x20+1) && (s>=0x20))
			{
				//
				// now see if it is set or display
				//
				if(1==t)
				{
					// Write to S-Register
					Get_Byte(ptr,&soc);
					tptr[s-0x20]=soc;
				}
				// Display register value always
				printByte(tptr[s-0x20]);
				msg=MSG_EMPTY;
			}
			else
			{
			//	DEBUG1("SREG is out of range"CRLF);
				msg=MSG_ERROR;
				break; 
			}
	}
	return(msg);
}


