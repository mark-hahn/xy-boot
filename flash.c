
#include <xc.h>
#include "flash.h"
#include "main.h"
#include "i2c.h"

#define _str(x)  #x
#define str(x)  _str(x)

char haveApp() {
  NVMADRH = NEW_RESET_VECTOR >> 8;
  NVMADRL = 0;  // (NEW_RESET_VECTOR & 0xff) gives constant overflow ???
  NVMCON1 = 1;  // set RD bit and clear other write stuff
  return ((NVMDATH << 8 | NVMDATL) != 0x3FFF);
}
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

void flash_memory_erase (unsigned int wordAddress) {
  NVMADRL = ((wordAddress)&0xff);
  NVMADRH = ((wordAddress)>>8);
  NVMCON1 = 0x94; 	// access FLASH memory, wren=1, FREE specifies erase 
  NVMCON2 = 0x55;
  NVMCON2 = 0xaa;
  NVMCON1bits.WR = 1;       // Start the write
  WREN = 0;					// disallow program/erase		
}

void flash_memory_write (unsigned int wordAddress, unsigned char *data ) {
  char wordIdx;
  NVMCON1 = 0x24; // specify write, LWLO -> don't do it now
  for (wordIdx=0; wordIdx < WRITE_FLASH_BLOCKSIZE; wordIdx++)  {
    NVMADRL =((wordAddress)&0xff);	// load address low byte
    NVMADRH =((wordAddress)>>8);		// load word address high byte
    if(wordIdx == WRITE_FLASH_BLOCKSIZE-1) LWLO = 0; // do actual flash this time
    NVMDATH = data[wordIdx*2];  // big-endian
    NVMDATL = data[wordIdx*2+1];
    WREN = 1;					        // allow program/erase
    NVMCON2 = 0x55;
    NVMCON2 = 0xaa;
    NVMCON1bits.WR = 1;       // Start the write
    wordAddress++;
  }	
  WREN = 0;					// disallow program/erase
}
