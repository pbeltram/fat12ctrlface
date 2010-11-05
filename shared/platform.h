/*
 * Copyright 2010 Primoz Beltram
 *
 *   Licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *       http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 * $Id$
 */

#ifndef PLATFORM_H
#define PLATFORM_H

//----------------------------------------------------------------------------
#if lpc2478
    #include <shared/lpc24xx.h>
    //NOTE: config_clocks() will set up HW to provide clock rates below.
    #define CPUCLK_FREQUENCY (72000000)
    #define USBCLK_FREQUENCY (48000000)
    #define CPUCLK_DIV (4)
#elif lpc2148
    #include <shared/lpc214x.h>
    //Fosc Crystal @12 MHz
    //CCLK = Fosc * MSEL; -> 60MHz
    //MSEL [4:0] = 00100b -> M=5, PSEL [6:5] = 01b -> P=2
    #define CPUCLK_FREQUENCY (60000000)
    #define SCB_PLL_CONFIG (SCB_PLLCFG_MUL5 | SCB_PLLCFG_DIV2)
    #define CPUCLK_DIV (SCB_VPBDIV_100)
    // Fosc = 12MHz. Crystal @12 MHz
    // USBCLK = Fosc * MSEL; -> 48MHz
    // MSEL [4:0] = 00011b -> M=4, PSEL [6:5] = 01b -> P=2
    #define USBCLK_FREQUENCY (48000000)
    #define USB_PLL_CONFIG (USB_PLLCFG_MUL4 | USB_PLLCFG_DIV2)
#elif lpc2106
    #include <shared/lpc210x.h>
    #define CPUCLK_FREQUENCY (58982400)
    #define CPUCLK_DIV (1)
#else
    #error unsupported ARMCPU
#endif

/** Peripheral Clock. */
#define PHERCLK_FREQUENCY (CPUCLK_FREQUENCY/CPUCLK_DIV)

/** Convert mili seconds to kernel ticks. */
#define MSEC2TICKS_m(_x_) ((portTickType) _x_ / portTICK_RATE_MS)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
//NOTE: With interrupts enabled, delay RMS is ~25%!
// E.g. 500usec will drift between 500 and 600 usec.
//#define TUNE_FAC 0.96154
#define TUNE_FAC 0.76154
#ifdef DEBUG
//NOTE: Debug bits double delay execution.
#define delay_usec(_usecdel_) delay_4clocks((unsigned long)((CPU_CLK_FREQUENCY) / (1000000/_usecdel_) /8 * TUNE_FAC))
#else
#define delay_usec(_usecdel_) delay_4clocks((unsigned long)((CPU_CLK_FREQUENCY) / (1000000/_usecdel_) /4 * TUNE_FAC))
#endif
#define delay_msec(_msecdel_) delay_usec((_msecdel_*1000))
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
inline void delay_4clocks(unsigned long delayval);
//----------------------------------------------------------------------------

#endif // PLATFORM_H

