

#include <xc.h>
#include "main.h"
#include "i2c.h"
#include "flash.h"
 
// note that there is NO error checking to make code small

void i2cInit() {
  SSP1CLKPPS = 0x13; // RC3 => SCL in
  SSP1DATPPS = 0x14; // RC4 => SDA in
  RC3PPS     = 0x15; // SCL => RC3 out
  RC4PPS     = 0x16; // SDA => RC4 out
  
  SSP1MSK = 0b11111100;       // matches addresses 10 and 11
  SSP1ADD = 0b00010100;       
  
//  SSP1CON1bits.SSPM  =  6;   // I2C Slave, 7-bit address
//  SSP1CON1bits.CKP   =  1;   // Zero holds clock low (clock stretch)
  SSP1CON1 = 0b00010110;       // SSPEN not set until end
  
//  SSP1CON2bits.SEN   =  1;   // Stretch Enable
  SSP1CON2 = 0b00000001;

  SSP1CON3 = 0;

  SSP1IF = 0; // clear I2C int flag, is used without interrupts
  SSP1IE = 0;   /* Clear I2C Interrupt Enable, is used without interrupts. */
  BCL1IE = 0;   /* Make sure Bus Collision Interrupt Enable is off. */
  BCL1IF = 0;   /* Clear I2C Bus Collision Interrupt Flag. */
  
  SSP1CON1bits.SSPEN =  1;   // Serial Port Enable bit
}

char i2cAddr; // two addresses (10 and 11) remove need for cmd byte
char packetByteIdx; // 0-based idx for data bytes
unsigned int wordAddr = NEW_RESET_VECTOR;  // not wise to count on this default
char buf[WRITE_FLASH_BLOCKSIZE*2]; // erase and data commands always 32 words

void doWriteAction(char dataByte) {
  if(i2cAddr == addrI2cWriteAddr) {  // set word address command
    if(packetByteIdx == 0) wordAddr = dataByte;  // address is big-endian
    else {
      wordAddr = wordAddr << 8 | dataByte;
      if(wordAddr == 0xffff) RESET();  // magic word address resets mcu
    }
  }
  else // dataI2cWriteAddr      fill buffer with data for later flashing
    buf[packetByteIdx] = dataByte;
}

void doReadAction() {
  if(i2cAddr == eraseI2cReadAddr)
    flash_memory_erase (wordAddr); // wordAddr not incremented

  else  // flashI2cReadAddr
    flash_memory_write (wordAddr, buf);
    wordAddr += WRITE_FLASH_BLOCKSIZE;
}

void chkI2c() {
  if (BCL1IF)           /* Check if Bus Collision have been detected. */
    BCL1IF = 0;         /* Clear the flag. */

  if(!SSP1IF) return;  // int flag used without interrupts
  SSP1IF = 0;  

  SSP1CON1bits.WCOL = 0;
  
  if (!SSP1STATbits.D_nA) { // Address char
    packetByteIdx = 0;
    i2cAddr = SSP1BUF >> 1;
    
    if(SSP1STATbits.R_nW) { // read type packet
      doReadAction();       // do actual erase or flash
      SSP1BUF = 0;          // single dummy byte
    }
  }
  else { // Data char, always an i2c write
    doWriteAction(SSP1BUF);  
    packetByteIdx++;
  }
  SSP1CON1bits.CKP = 1; // Release clock to stop stretching
}
