/**
 * \file   hs_mmcsd.c
 *
 * \brief  This file contains functions which configure the HS/MMCSD controller
 */

/*
* Copyright (C) 2010 Texas Instruments Incorporated - http://www.ti.com/
*/
/*
*  Redistribution and use in source and binary forms, with or without
*  modification, are permitted provided that the following conditions
*  are met:
*
*    Redistributions of source code must retain the above copyright
*    notice, this list of conditions and the following disclaimer.
*
*    Redistributions in binary form must reproduce the above copyright
*    notice, this list of conditions and the following disclaimer in the
*    documentation and/or other materials provided with the
*    distribution.
*
*    Neither the name of Texas Instruments Incorporated nor the names of
*    its contributors may be used to endorse or promote products derived
*    from this software without specific prior written permission.
*
*  THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
*  "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
*  LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
*  A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
*  OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
*  SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
*  LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
*  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
*  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
*  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
*  OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*
*/

#include "hw_control_AM335x.h"
#include "soc_AM335x.h"
#include "hw_cm_per.h"
#include "hw_types.h"
#include "hw_cm_dpll.h"
#include "hs_mmcsd.h"

void HSMMCSDPinMuxSetup(void)
{
    // SD-Card
    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT3) =
                   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT3_CONF_MMC0_DAT3_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT2) =
                   (0 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT2_CONF_MMC0_DAT2_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT1) =
                   (0 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT1_CONF_MMC0_DAT1_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_DAT0) =
                   (0 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_DAT0_CONF_MMC0_DAT0_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_CLK) =
                   (0 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_CLK_CONF_MMC0_CLK_RXACTIVE_SHIFT);

    HWREG(SOC_CONTROL_REGS + CONTROL_CONF_MMC0_CMD) =
                   (0 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_MMC0_CMD_CONF_MMC0_CMD_RXACTIVE_SHIFT);

     HWREG(SOC_CONTROL_REGS + CONTROL_CONF_SPI0_CS1) =
                   (5 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_MMODE_SHIFT)    |
                   (0 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_PUDEN_SHIFT)    |
                   (1 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_PUTYPESEL_SHIFT)|
                   (1 << CONTROL_CONF_SPI0_CS1_CONF_SPI0_CS1_RXACTIVE_SHIFT);

     //eMMC                                                                   //P8
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(0))=CONTROL_CONF_MUXMODE(1); //25
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(1))=CONTROL_CONF_MUXMODE(1); //24
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(2))=CONTROL_CONF_MUXMODE(1); // 5
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(3))=CONTROL_CONF_MUXMODE(1); // 6
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(4))=CONTROL_CONF_MUXMODE(1); //23
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(5))=CONTROL_CONF_MUXMODE(1); //22
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(6))=CONTROL_CONF_MUXMODE(1); // 3
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_AD(7))=CONTROL_CONF_MUXMODE(1); // 4
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_CSN(2))=CONTROL_CONF_MUXMODE(2); //20
     HWREG(SOC_CONTROL_REGS+CONTROL_CONF_GPMC_CSN(1))=CONTROL_CONF_MUXMODE(2); //21
}


/**
 * \brief   This function will configure the required clocks for HS MMC/SD instance.
 *
 * \return  None.
 *
 */
void HSMMCSDModuleClkConfig(unsigned char drive)
{
   if (drive==0)
   {
      // SD-Card
      HWREG(SOC_PRCM_REGS + CM_PER_MMC0_CLKCTRL)|=CM_PER_MMC0_CLKCTRL_MODULEMODE_ENABLE;
      while((HWREG(SOC_PRCM_REGS + CM_PER_MMC0_CLKCTRL) & CM_PER_MMC0_CLKCTRL_MODULEMODE) != CM_PER_MMC0_CLKCTRL_MODULEMODE_ENABLE);
   }
   else
   {
      // eMMC
      HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL)|=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE;
      while ((HWREG(SOC_PRCM_REGS+CM_PER_MMC1_CLKCTRL) & CM_PER_MMC1_CLKCTRL_MODULEMODE)!=CM_PER_MMC1_CLKCTRL_MODULEMODE_ENABLE);
   }
}

