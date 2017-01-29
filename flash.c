
#include <xc.h>
#include "flash.h"
#include "i2c.h"

#define _str(x)  #x
#define str(x)  _str(x)

//   -- not used -- and not tested
//unsigned int flash_memory_read (unsigned int address) {
//	NVMADRL=((address)&0xff);
//	NVMADRH=((address)>>8);	
//  NVMCON1 = 0x80;
//	NOP();
//  NOP();
//	return (unsigned int) NVMDATA;
//  unsigned int nvmdat = NVMDATL;
//  return (NVMDATH << 8 | nvmdat);
//}	

void flash_memory_erase (unsigned int address) {
  NVMADRL = ((address)&0xff);
  NVMADRH = ((address)>>8);
  NVMCON1 = 0x94; 	// access FLASH memory, wren=1, FREE specifies erase 
  NVMCON2 = 0x55;
  NVMCON2 = 0xaa;
  NVMCON1bits.WR = 1;       // Start the write
  WREN = 0;					// disallow program/erase		
}

void flash_memory_write (unsigned int address, unsigned char *data )
{
		char idx;
		
	  NVMCON1 = 0x24;             // specify write, LWLO -> don't do it now
		NVMADRL =((address)&0xff);	// load address
		NVMADRH =((address)>>8);		// load address
		for (idx=0;idx<WRITE_FLASH_BLOCKSIZE;idx+=2)
		{
      if(idx == LAST_WORD_MASK) LWLO = 0;   // do actual flash this time
			NVMDATH = data[idx];
			NVMDATL = data[idx+1];
			WREN = 1;					        // allow program/erase
      NVMCON2 = 0x55;
      NVMCON2 = 0xaa;
      NVMCON1bits.WR = 1;       // Start the write
			NVMADRL++;
      if(NVMADRL == 0) NVMADRH++;
		}	
		WREN = 0;					// disallow program/erase
}
