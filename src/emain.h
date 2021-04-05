#ifndef __EMAIN_H__
#define __EMAIN_H__
//																www.mycal.com
//---------------------------------------------------------------------------
// emain.h - emain header file      											-
//---------------------------------------------------------------------------
// Version                                                                  -
//		0.1 Original Version June 3, 2000									-
//																			-
// (c)2000 Mycal Labs, All Rights Reserved									-
//---------------------------------------------------------------------------

#include "mTypes.h"

#include "ports.h"
#include "debug.h"


// misc short buffer, should be in globals
extern U8	smbuff[16];
extern int	*absolute;



short (*boot_version)(void);
int (*crc_boot)(void);
int (*bootloader)(U16 baud);


#endif 
