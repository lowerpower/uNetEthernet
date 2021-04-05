
/*															
 *---------------------------------------------------------------------------
 * msg.c - result message code   											-
 *---------------------------------------------------------------------------
 * Original Version                                                         -
 *  Nov 7 2000	- mwj                                                       -
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2000, iReady Corporation, All Rights Reserved.				_
 *                                                                         	_
 *---------------------------------------------------------------------------
*/

#include "msg.h"

//
// Strings are stored in FLASH memeory on AVR devices
//
#ifdef __AVR__
#define FLASHMSG(msg)	FLASHMEM = msg
#else
#define FLASHMSG(msg)	= msg
#endif


//static char	strCRLF[] FLASHMEM					= "\r\n"; 
static char strOK[]								FLASHMSG("OK");       
static char strFAIL[]							FLASHMSG("FAIL");
static char strCONNECT[] 						FLASHMSG("CONNECT");
static char strERROR[]			 				FLASHMSG("ERROR");  

//static char strPPPFail[]						FLASHMSG("FAIL PPP");
//static char strPPPAUTHFail[]					FLASHMSG("AUTH FAIL PPP");
//static char strPPPDown[]						FLASHMSG("PPP DOWN");
    
//static char strNoCarrier[]						FLASHMSG("NO CARRIER"); 
static char strLINKDown[]						FLASHMSG("FAIL LINK"); 
static char strDHCPFail[]						FLASHMSG("FAIL DHCP"); 

static char strTCPFail[]						FLASHMSG("FAIL TCP");  
static char strDNSFail[]						FLASHMSG("FAIL DNS");

static char strSOCDown[]						FLASHMSG("SOCKET CLOSED");
static char strSOCReset[]						FLASHMSG("SOCKET RESET");    
static char strNoData[]							FLASHMSG("NO DATA");

static char strEERROR[]			 				FLASHMSG("ERROR - DEFAULTS LOADED");  

static char strVersion[]						FLASHMSG(EM_VERSION);




//#define RESULT 1
//char	IMMConfig = 1;

//
// Display the result message, either string or numeric result.
//
void
displayMessage(U8 msg)
{
	if(msg==MSG_NULL)
		return;

	//
	// Do nothing if result is turned off.
	//
	if(RESULT_OFF & mm.IMMConfig)
		return;
	// sysdata
	//if(msg!=MSG_EMPTY)
	//	CONSOLE_puts(CRLF);
	//
	// Result if on, send numeric or text.
	//
	if(RESULT & mm.IMMConfig)
	{   
#ifdef DEBUG_LV3
		DEBUG3("msg ");
		printShort(msg);
		DEBUG3(CRLF);
#endif
		//
		// Return result string
		//
	    switch(msg)
    	{
    	case MSG_OK:   
			CONSOLE_cputs(strOK);
    		break;             
	    case MSG_CONNECT:   
    		CONSOLE_cputs(strCONNECT);
    		break;	
	    case MSG_ERROR:          
			CONSOLE_cputs(strERROR);    
    		break;
	    case MSG_LINKDOWN:          
			CONSOLE_cputs(strLINKDown);    
    		break;
	    case MSG_DHCPFAIL:          
			CONSOLE_cputs(strDHCPFail);    
    		break;

#ifdef PPP
		case MSG_NOCARRIER:
			CONSOLE_cputs(strNoCarrier);
			break;    		  
		case MSG_PPPFAIL:				// was 80  
			CONSOLE_cputs(strPPPFail);
			break;    	
		case MSG_PPPAUTHFAIL:			// was 80  
			CONSOLE_cputs(strPPPAUTHFail);
			break; 			
		case MSG_PPPDOWN:				// was 81  
			CONSOLE_cputs(strPPPDown);
			break;	  
#endif
		case MSG_TCPFAIL:				// was 82  
			CONSOLE_cputs(strTCPFail);
			break;	
		case MSG_SOCDOWN:				// was 84
			CONSOLE_cputs(strSOCDown);
    		break;	
    	case MSG_DNSFAIL:				// was 87
    		CONSOLE_cputs(strDNSFail);
    		break;              
		case MSG_FAIL:                       //88
			CONSOLE_cputs(strFAIL);    
			break;
		case MSG_NODATA:				// was 125
			CONSOLE_cputs(strNoData);		
			break;		
		case MSG_EEERROR:
			CONSOLE_cputs(strEERROR);
			break;
		case MSG_VERSION:
			CONSOLE_cputs(strVersion);
			break;
		case MSG_EMPTY:
	    default:
    		break;
		}
	}
	else
	{
		//
		// Return Numeric String
		//
		if(msg!=MSG_EMPTY)
		{
			b2d(msg, smbuff);
			CONSOLE_puts(smbuff);
		}
	}	
	
	//CONSOLE_putch(0x0d);
	//CONSOLE_putch(0x0a);
	CONSOLE_puts(CRLF);
	//cputs(strCRLF);
	
}












