
/*  
Name		WLN_Power                            
Arguments	powerState		:   Power state values ON, OFF, SLEEP, DOZE, RESET, ACT                      
Return		TRUE or FALSE
Description	This does different tasks based on the passed parameter ON, OFF, SLEEP, 
			 DOZE, RESET, ACT. This function is called by application directly. WLN_Power
			 handles power management and on/off reset.
*/
uint8_t WLN_Power(uint8_t powerState)
{
	uint32_t u8Result;
	
	IODIR1 |= 1<<23; // as output 
	switch(powerState)
	{
  	case ON:
   		//Switch ON
		IOCLR1 |= 1<<23; //active low
		//allows hardware to settle after power up
		SleepMs(10); 
		//load cfg parser and MAC firmware
		u8Result=PhgHhalDoBootLoad();
	
		if(u8Result == TRUE)
		{
			u8Result = WLN_InitializeMac();
		}
		break;
	case OFF:
		RDEBUGMSG("Power Off -OK\n\r");
		//Switch OFF
		IOSET1 |= 1<<23; 
		u8Result=TRUE;
		break;
	case SLEEP:
		u8Result=WLN_Sleep();
		break;
	case ACT:
		u8Result=WLN_Activate();
		break;
	case DOZE:
		u8Result=WLN_Doze();
		break;
	case RESET:
		u8Result=TRUE;
		break;
	default:
		u8Result=FALSE;
	}
 	return u8Result;
 }



///////////////////////////////////////////////
/*  
Name		PhgHhalDownloadMAC                            
Arguments	None                                                 
Return		BLFU_STATUS_SUCCESS or BLFU_STATUS_FAILURE 
Description	The PhgHhalDownloadMAC does a boot load of files mainly the MAC file 
			or production firmware					 
            This gets called inside the PhgHhalDoBootLoad.                                     
*/
uint32_t PhgHhalDownloadMAC(void)
{
	uint32_t u32Status;
	uint16_t u16RespVal;
	uint32_t u32RetryCount;

	uint32_t u32Iterations;
	uint32_t u32Count, u32TotalLenRemaining, u32CurDMALen;
	uint8_t * pu8DmaPtr;	
	uint16_t  u16Val;

	u32Status = BLFU_STATUS_SUCCESS;
	u16RespVal = 0;
	u32RetryCount =0;
 	
	u32Iterations =  (redmond_mac_len / MAX_M2S_DMA_SIZE) + 1;
	u32Count = 0; 
	u32TotalLenRemaining = redmond_mac_len; 
	u32CurDMALen = 0;
	pu8DmaPtr = (uint8_t *)redmond_mac;
	u16Val = 0;
	
	// write the number of iterations required for download based on 
	//above calculations to M2S SR3.
	spi_writeToSlaveReg(HOST_M2S_SCRATCH_REG3,u32Iterations);

	// wait for the response from cfgp downloaded previously for 
 	//starting img file transfer 
 	u16RespVal = 0;
	while ( (u16RespVal != 0x26) && (u32RetryCount)) // check the boot code for SPI2 for the value 
	{
		  //In the bootrom, after booting DMA completion, the ISR sets 0x24 
		  //to the HOST_S2M_MAIL_BOX_REG2 
		spi_readFromSlaveReg(HOST_S2M_MAIL_BOX_REG2,&u16RespVal);
		u32RetryCount--;
	}
	if(u32RetryCount == 0)
	{
		SleepMs(SLEEP_TIME_AFTER_MAX_RETRY);
		spi_readFromSlaveReg( HOST_S2M_MAIL_BOX_REG2,&u16RespVal);
	}
	if(u16RespVal != 0x26)
	{
		return BLFU_STATUS_FAILURE;
	}

	// loop till the DMA of complete code is done       
	while ( u32Count < u32Iterations )
  	{
 		u32Count++;
 		if ( u32TotalLenRemaining > MAX_M2S_DMA_SIZE )
 		{
			u32CurDMALen = MAX_M2S_DMA_SIZE; 
			u32TotalLenRemaining -= MAX_M2S_DMA_SIZE;
		}
 192:drivers/wlndriver/wln_embedded_bootload.c **** 		else
 193:drivers/wlndriver/wln_embedded_bootload.c **** 		{
 194:drivers/wlndriver/wln_embedded_bootload.c **** 			u32CurDMALen = u32TotalLenRemaining;
 195:drivers/wlndriver/wln_embedded_bootload.c **** 		}
 196:drivers/wlndriver/wln_embedded_bootload.c **** 	
 197:drivers/wlndriver/wln_embedded_bootload.c **** 		// send packet length to SPI interface		
 198:drivers/wlndriver/wln_embedded_bootload.c **** 		spi_sendM2SDMAPkg(u32CurDMALen);
 199:drivers/wlndriver/wln_embedded_bootload.c **** 
 200:drivers/wlndriver/wln_embedded_bootload.c **** 		// send packet buffer to SPI interface
 201:drivers/wlndriver/wln_embedded_bootload.c **** 		spi_write_buffer((uint8_t *)pu8DmaPtr, u32CurDMALen);
 202:drivers/wlndriver/wln_embedded_bootload.c **** 		pu8DmaPtr+=u32CurDMALen;
 203:drivers/wlndriver/wln_embedded_bootload.c **** 
 204:drivers/wlndriver/wln_embedded_bootload.c **** 	
 205:drivers/wlndriver/wln_embedded_bootload.c **** 		// Now wait for the tgt to ACK this DMA 
 206:drivers/wlndriver/wln_embedded_bootload.c **** 		u32RetryCount = HOST_LONG_RETRY_COUNT;
 207:drivers/wlndriver/wln_embedded_bootload.c **** 		do
 208:drivers/wlndriver/wln_embedded_bootload.c **** 		{
 209:drivers/wlndriver/wln_embedded_bootload.c **** 			spi_readFromSlaveReg(HOST_S2M_SCRATCH_REG3,
 210:drivers/wlndriver/wln_embedded_bootload.c **** 									 &u16Val);
 211:drivers/wlndriver/wln_embedded_bootload.c **** 		}while ( (u16Val != u32Count) && (u32RetryCount--) );
 212:drivers/wlndriver/wln_embedded_bootload.c **** 
 213:drivers/wlndriver/wln_embedded_bootload.c **** 		if(u32RetryCount == 0x00)
 214:drivers/wlndriver/wln_embedded_bootload.c **** 		{
 215:drivers/wlndriver/wln_embedded_bootload.c **** 			// do a last try
 216:drivers/wlndriver/wln_embedded_bootload.c **** 			SleepMs(SLEEP_TIME_AFTER_MAX_RETRY);
 217:drivers/wlndriver/wln_embedded_bootload.c **** 			spi_readFromSlaveReg(	 HOST_S2M_SCRATCH_REG3,
 218:drivers/wlndriver/wln_embedded_bootload.c **** 									 &u16Val);
 219:drivers/wlndriver/wln_embedded_bootload.c **** 		}
 220:drivers/wlndriver/wln_embedded_bootload.c **** 	
 221:drivers/wlndriver/wln_embedded_bootload.c **** 		if(u16Val != u32Count)
 222:drivers/wlndriver/wln_embedded_bootload.c **** 		{
 223:drivers/wlndriver/wln_embedded_bootload.c **** 			// return failure
 224:drivers/wlndriver/wln_embedded_bootload.c **** 			return BLFU_STATUS_FAILURE;


