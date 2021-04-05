#ifndef __MEMORY_H__
#define __MEMORY_H__

/*																www.nchip.com
 *---------------------------------------------------------------------------
 * memory.h - block memory allocation stuff									-
 *---------------------------------------------------------------------------
 * Version                                                                  -                                            
 *		0.1 Original Version JUN 15, 2003									-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				_
 * Copyright (C) 2003, nChip, All Rights Reserved.							_
 *															               	_
 *---------------------------------------------------------------------------
*/
#include "mTypes.h"

// Dependancies

// Quasi - to be removed
#include "globals.h"

// Debug dependancies
#include "console.h"
#include "conio.h"
#include "debug.h"

#define	MEMSIG 0xBEEF

#define NUM_MEM_BLOCKS	4
// MAC heder + IP header+TCP header + TCP data  14 + 20+ 20 +128  roundup
#define BLOCK_DATA_SIZE	190	


typedef struct memblock_h
{
	U16					sig;
	struct memblock_h	*next;
	U16					ack;					// Not used
	U16					timer;
	U16					len;
	U8					data[BLOCK_DATA_SIZE];
}MEMBLOCK;



// Module Prototypes
void		MEM_init(void);
MEMBLOCK	*MEM_GetBlock(void);
void		MEM_FreeBlock(MEMBLOCK *block);


#endif
