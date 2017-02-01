
// I2C bootloader for PIC16LF15355

// code used with the bootloader should offset the code by 0x200
// search xc8 user manual for --CODEOFFSET and --ROM

#pragma config FEXTOSC = OFF      // External Oscillator mode selection bits (Oscillator not enabled)
#pragma config RSTOSC = HFINT32   // Power-up default value for COSC bits (HFINTOSC with OSCFRQ= 32 MHz and CDIV = 1:1)
#pragma config CLKOUTEN = OFF     // Clock Out Enable bit (CLKOUT function is disabled; i/o or oscillator function on OSC2)
#pragma config CSWEN = ON         // Clock Switch Enable bit (Writing to NOSC and NDIV is allowed)
#pragma config FCMEN = ON         // Fail-Safe Clock Monitor Enable bit (FSCM timer enabled)
#pragma config MCLRE = ON         // Master Clear Enable bit (MCLR pin is Master Clear function)
#pragma config PWRTE = OFF        // Power-up Timer Enable bit (PWRT disabled)
#pragma config LPBOREN = OFF      // Low-Power BOR enable bit (ULPBOR disabled)
#pragma config BOREN = ON         // Brown-out reset enable bits (Brown-out Reset Enabled, SBOREN bit is ignored)
#pragma config BORV = LO          // Brown-out Reset Voltage Selection (Brown-out Reset Voltage (VBOR) set to 1.9V on LF, and 2.45V on F Devices)
#pragma config ZCD = OFF          // Zero-cross detect disable (Zero-cross detect circuit is disabled at POR.)
#pragma config PPS1WAY = ON       // Peripheral Pin Select one-way control (The PPSLOCK bit can be cleared and set only once in software)
#pragma config STVREN = ON        // Stack Overflow/Underflow Reset Enable bit (Stack Overflow or Underflow will cause a reset)
#pragma config WDTCPS = WDTCPS_31 // WDT Period Select bits (Divider ratio 1:0x10000; software control of WDTPS)
#pragma config WDTE = OFF         // WDT operating mode (WDT enabled regardless of sleep; SWDTEN ignored)
#pragma config WDTCWS = WDTCWS_7  // WDT Window Select bits (window always open (100%); software control; keyed access not required)
#pragma config WDTCCS = SC        // WDT input clock selector (Software Control)
#pragma config BBSIZE = BB512     //  (512 char boot block size)
#pragma config BBEN = OFF          //  (Boot Block enabled)
#pragma config SAFEN = OFF        //  (SAF disabled)
#pragma config WRTAPP = OFF       //  (Application Block not write protected)
#pragma config WRTB = OFF         //  (Boot Block not write protected)
#pragma config WRTC = OFF         //  (Configuration Register not write protected)
#pragma config WRTSAF = OFF       //  (SAF not write protected)
#pragma config LVP = OFF          // Low Voltage Programming Enable bit (High Voltage on MCLR/Vpp must be used for programming)
#pragma config CP = OFF           // UserNVM Program memory code protection bit (UserNVM code protection disabled)
 
#define _XTAL_FREQ 32000000  

#include <xc.h>
#include "main.h"
#include "i2c.h"
#include "flash.h"


#define _str(x)  #x
#define str(x)  _str(x)

void interrupt main_isr() { // redirect 0x0004 int to 0x0204
    asm ("pagesel  " str (NEW_INTERRUPT_VECTOR));
    asm ("goto   "   str (NEW_INTERRUPT_VECTOR));
}     

void main(void) {
  // after first app load, to re-flash, the app must first erase 0x200
  if(haveApp()) {   // jump to app
    STKPTR = 0x1F;
    asm ("pagesel " str(NEW_RESET_VECTOR));
    asm ("goto  "  str(NEW_RESET_VECTOR));
  }
  //  run bootloader
  i2cInit();
  while(1) chkI2c();
}

/*
 // sample app code to re-enter boot loader
    GIE = 0; 
    flash_memory_erase(NEW_RESET_VECTOR);
    STKPTR = 0x1F;             // could do reset() here instead
    asm ("pagesel " str(0));
    asm ("goto  "  str(0));
 */
