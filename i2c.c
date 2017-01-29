

#include <xc.h>
#include "i2c.h"
#include "flash.h"

// note that there is NO error checking to make code small

void i2cInit() {
  SSP1CLKPPS = 0x13; // RC3 => SCL in
  SSP1DATPPS = 0x14; // RC4 => SDA in
  RC3PPS     = 0x15; // SCL => RC3 out
  RC4PPS     = 0x16; // SDA => RC4 out

  SSP1MSKbits.SSPMSK = 0xe7; // matches addr 4 and/or 8
  SSP1ADDbits.SSPADD =  0; // allow any combination of addr 8 & 16
  SSP1CON1bits.SSPM  =  6; // I2C Slave, 7-bit address
  SSP1CON1bits.WCOL  =  1; // Write Collision detect
  SSP1CON1bits.CKP   =  1; // Zero holds clock low (clock stretch)
  SSP1CON1bits.SSPEN =  1; // Serial Port Enable bit
  SSP1CON2bits.SEN   =  1; // Stretch Enable
  SSP1CON2bits.GCEN  =  0; // General Call Enable 
  SSP1CON3bits.SCIE  =  0; // Start Condition Interrupt Enable 
  SSP1CON3bits.PCIE  =  0; // Stop Condition Interrupt Enable 
  SSP1CON3bits.BOEN  =  0; // Buffer Overwrite Detect Enable
  SSP1CON3bits.SBCDE =  0; // Bus Collision Detect Enable
  SSP1CON3bits.AHEN  =  0; // Address Hold Enable for software ack/nak
  SSP1CON3bits.DHEN  =  0; // Data Hold Enable for software ack/nak
  
  SSP1IF =  0; // clear I2C int flag, is used without interrupts
}

char i2cAddr; // two addresses (4 & 8) remove need for cmd byte
char packetByteIdx; // 0 for addr byte and 1-based idx for rest
unsigned int wordAddr = APP_CODE_OFFSET;  // not wise to count on this default
char buf[WRITE_FLASH_BLOCKSIZE*2]; // erase and data commands always 32 words

void doWriteAction(char dataByte) {
  if(i2cAddr == addrI2cWriteAddr) {  // set word address command
    if(packetByteIdx == 0) wordAddr = dataByte;
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
  if(!SSP1IF) return;
  SSP1IF = 0;  
  
  if (!SSP1STATbits.D_nA) { // Address char
    SSP1CON1bits.WCOL = 0;
    packetByteIdx = 0;
    i2cAddr = SSP1BUF >> 1;
    
    if(SSP1STATbits.R_nW) { // read type packet
      doReadAction();       // do erase or flash
      SSP1BUF = 0;          // single dummy byte
      SSP1CON1bits.CKP = 1; // Release clock to stop stretching
    }
  }
  else { // Data char, always an i2c write, either set addr or do actual flash
    SSP1CON1bits.WCOL = 0;
    doWriteAction(SSP1BUF);  
    packetByteIdx++;
    SSP1CON1bits.CKP = 1; // Release clock to stop stretching
  }
}
