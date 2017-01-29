/* 
 * File:   flash.h
 * Author: root
 *
 * Created on January 28, 2017, 10:03 PM
 */

#ifndef FLASH_H
#define	FLASH_H

#ifdef	__cplusplus
extern "C" {
#endif

//unsigned int flash_memory_read (unsigned int address); // not used
void flash_memory_erase (unsigned int address);
void flash_memory_write (unsigned int address, unsigned char *data);


#ifdef	__cplusplus
}
#endif

#endif	/* FLASH_H */

