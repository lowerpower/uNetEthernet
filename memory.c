
/*																www.mycal.com			
 *---------------------------------------------------------------------------
 * memory.c - Memory Management												-
 *																			-
 *---------------------------------------------------------------------------
 * Version                                                                  -
 *		0.1 Original Version June 3, 2000									-        
 *
 *---------------------------------------------------------------------------    
 *                                                             				-
 * Copyright (C) 2000, Mycal Labs www.mycal.com								-
 *                                                                         	-
 *---------------------------------------------------------------------------
*/
/*					*/ 
/* include files 	*/
/*	
*/

#include "memory.h"


#define	MEMSIG 0xBEEF

MEMBLOCK	*free_list;					/*!< pointer to the memory block free list */
MEMBLOCK	memory[NUM_MEM_BLOCKS];		/*!< Memory block array */
static	U8			used_count;			/*!< Memory blocks currently in use */


/*! \fn void MEM_init(void)
    \brief Initializes the memory block allocator
*/
void
MEM_init()
{
int	i;

#ifdef DEBUG_LV2
	if(mm.IMMConfig & PPP_DEBUG)
	{
		DEBUG1("Createing ");
		printShort(NUM_MEM_BLOCKS);
		DEBUG1(" Memory blocks of ");
		printShort(BLOCK_DATA_SIZE);
		DEBUG1(" Size"CRLF);
		DEBUG1("]"CRLF);
	}
#endif
	// hook up memory blocks
	for(i=0;i<NUM_MEM_BLOCKS;i++)
	{
		memory[i].next=&memory[i+1];
		memory[i].sig=MEMSIG;
		// tmp clean
		memset(memory[i].data,0,BLOCK_DATA_SIZE);
	
	}
	// Null last one
	memory[i-1].next=0;
	memory[i-1].sig=MEMSIG;
	// Hook up head of free list pointer
	free_list=&memory[0];
		//
	used_count=0;
}

/*! \fn MEMBLOCK * MEM_GetBlock(void)
    \brief Returns a memory block of the freelist or 0 if none left

*/
MEMBLOCK *
MEM_GetBlock()
{
MEMBLOCK	*ret;

	// Check free list
	ret=free_list;
	//DEBUG1(CRLF"ret=");
	//printShort(ret);
	//DEBUG1(CRLF);
	if(0!=ret)
	{
		free_list=free_list->next;
		used_count++;
		
		if(used_count>0)
		{
#if 0
			DEBUG1("GUsed Count ");
			printByte(used_count);
			DEBUG1("blk -");
			printWord(ret);
			DEBUG1(CRLF);
			if(ret->sig!=MEMSIG)
				DEBUG1("ALLOC CORRUPT BLOCK"CRLF);
#endif	
		}
	}
	// return
	return(ret);
}

/*! \fn void MEM_FreeBlock(MEMBLOCK *block)
    \brief Put a block back on the free list

    \param block pointer to memory block to free

*/
void
MEM_FreeBlock(MEMBLOCK *block)
{
	//
	// Verify block is ok
	//
	//	DEBUG1("free --");
	//	printWord(block);
	//	DEBUG1("attach -");
	//	printWord(free_list);
	//	DEBUG1(CRLF);

	if( ( block<memory) || (block > memory+(NUM_MEM_BLOCKS*sizeof(MEMBLOCK))) )
	{
		DEBUG1("BADFREEBLOCK"CRLF);
		return;
	}

	if(block->sig!=MEMSIG)
	{
		DEBUG1("CORRUPT BLOCK"CRLF);
		return;
	}

  //
	// Add back onto the head of the free list
	//
	if(used_count)
	{
		block->next=free_list;
		free_list=block;
		used_count--;

	//	DEBUG1("Used Count ");
	//	printByte(used_count);
	//	DEBUG1(CRLF);
	}
	else
		DEBUG1("bad free"CRLF);

}





