#ifndef I2C_H
#define	I2C_H
#ifdef	__cplusplus
extern "C" {
#endif
  
// addresses do not include rw bit so they range from 1 up
#define addrI2cWriteAddr 4 /* i2c addr for write that sets 
                               word addr for erase and flash writes.
                               word addr of 0xffff triggers reset */
#define dataI2cWriteAddr 8 /* i2c addr for writing 32 words (64 bytes, high-endian */
#define eraseI2cReadAddr 4 /* i2c addr for read that triggers erasing */
#define flashI2cReadAddr 8 /* i2c addr for read that triggers actual flashing 
                               and bumps word address for next erase */
  
#define ERASE_FLASH_BLOCKSIZE 32 /* words */
#define WRITE_FLASH_BLOCKSIZE 32 /* words */

#define APP_CODE_OFFSET 0x200

void i2cInit();
void chkI2c();

#ifdef	__cplusplus
}
#endif
#endif	/* I2C_H */

