#include <string.h>

#include "pruss.h"
#include "hw_types.h"
#include "soc_AM335x.h"

/*
   PRU driver implementation, based on code code and documentation from
   https://groups.google.com/forum/#!category-topic/beagleboard/pru/rCO-2nKynVE

   For a description how to compile PRU code in a suitable way, please also refer
   to this description!
*/


/**
 * Initialise both PRUs for later usage
 * clkFlags: optional flags CM_PER_PRU_ICSS_CLKSTCTRL_UART_GCLK and/or CM_PER_PRU_ICSS_CLKSTCTRL_IEP_GCLK
 */
void PRUSSInit(unsigned int clkFlags)
{
   volatile int i=0;

   // reset the PRU, this may not be necessary in case of initial start-up
   HWREG(SOC_PRM_PER_REGS)|=0x00000002;
   while ((HWREG(SOC_PRM_PER_REGS) & 0x00000002)==0); //wait until reset was done
   HWREG(SOC_PRM_PER_REGS)&=0xFFFFFFFD; // clear reset bit

   // wake-up and enable PRU, enable OCP-clock (mandatory)
   // UART and IEP clock have to be enabled here too when needed
   HWREG(SOC_CM_PER_REGS+CM_PER_PRU_ICSS_CLKCTRL)=0x00000002;
   HWREG(SOC_CM_PER_REGS+CM_PER_PRU_ICSS_CLKSTCTRL)=(CM_PER_PRU_ICSS_CLKSTCTRL_OCP_GCLK|clkFlags);

   // have a short delay before next step
   while (i<10000)
   {
      i++;
   }
   HWREG(PRUSS_CFG_BASE+PRUSS_CFG_BASE_SYSCFG)=(0x00000005);
   while ((HWREG(PRUSS_CFG_BASE+PRUSS_CFG_BASE_SYSCFG) & 0x00000020)!=0); // check wait state bit
}


/**
 * Load text and data binaries into PRU0 or PRU1 buffer and execute it. The code needs to be compiled in a way
 * where start address is located at address 0, to ensure this, a linker command file with following structure has 
 * to be used for compilation of the PRU-code which forces it to always use start address 0x000:

-cr
-stack 0x100
-heap 0x100

MEMORY
{
    PAGE 0:
      PRUIMEM:   o = 0x00000000  l = 0x00001000
    PAGE 1:
      PRUDMEM:   o = 0x00000000  l = 0x00001000
}

SECTIONS
{
   GROUP: load = PRUIMEM
   {
      .text:_c_int00* :
      .text           :
   }
   .stack          >  PRUDMEM, PAGE 1
   .bss            >  PRUDMEM, PAGE 1
   .cio            >  PRUDMEM, PAGE 1
   .const          >  PRUDMEM, PAGE 1
   .data           >  PRUDMEM, PAGE 1
   .switch         >  PRUDMEM, PAGE 1
   .sysmem         >  PRUDMEM, PAGE 1
   .cinit          >  PRUDMEM, PAGE 1
}

 */
int  PRUSSExecBuffer(unsigned int pruNum,void *textBuffer,unsigned int textNumBytes,void *dataBuffer,unsigned int dataNumBytes)
{
   if (pruNum==0)
   {
      // copy text and data into PRU0 instruction and data RAM
      memcpy((void*)PRU0IRAM_PHYS_BASE,(void*)textBuffer,textNumBytes);
      if (dataBuffer) memcpy((void*)DATARAM0_PHYS_BASE,(void*)dataBuffer,dataNumBytes);

      // set start address and execute
      HWREG(PRU0CONTROL_PHYS_BASE+PRU_PHYS_BASE_CTRL)|=0x04200000; // set start address
      HWREG(PRU0CONTROL_PHYS_BASE+PRU_PHYS_BASE_CTRL)|=0x00000002; // execute
   }
   else if (pruNum==1)
   {
      // copy text and data into PRU0 instruction and data RAM
      memcpy((void*)PRU1IRAM_PHYS_BASE,(void*)textBuffer,textNumBytes);
      if (dataBuffer) memcpy((void*)DATARAM1_PHYS_BASE,(void*)dataBuffer,dataNumBytes);

      // set start address and execute
      HWREG(PRU1CONTROL_PHYS_BASE+PRU_PHYS_BASE_CTRL)|=0x04200000; // set start address
      HWREG(PRU1CONTROL_PHYS_BASE+PRU_PHYS_BASE_CTRL)|=0x00000002; // execute
   }
   else return -1;
   return 0;
}


