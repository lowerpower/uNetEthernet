#ifndef __MICMP_H__
#define __MICMP_H__
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * micmp.h - Mycal ICMP Layer header file									-
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

#include "mTypes.h"

//
// ICMP types
//           
#define	ICMP_ECHO_REPLY			0
#define ICMP_DEST_UNREACHABLE   3
#define ICMP_SOURCE_QUENCH		4
#define	ICMP_REDIRECT			5
#define	ICMP_ECHO_REQUEST		8
#define ICMP_ROUTER_ADVERT		9
#define ICMP_ROUTER_SOLICIT		10
#define ICMP_TIME_EXCEEDED		11
#define ICMP_PARAMETER_PROBLEM	12
#define ICMP_TIMESTAMP_REQEST	13
#define ICMP_TIMESTAMP_REPLY	14
#define ICMP_MASK_REQUEST		17
#define ICMP_MASK_REPLY			18       


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





#endif
