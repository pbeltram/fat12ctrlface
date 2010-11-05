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

#include <shared/platform.h>
#include <shared/debug.h>

#include "hwinit.h"
#include "gpio.h"

//------------------------------------------------------------------------------
/**
 * Configure GPIO.
 */
static void config_GPIO( void )
{
    // Enable FAST mode for PORT0 and PORT1.
    SCB_SCS |= (SCB_SCS_GPIO0M | SCB_SCS_GPIO1M);

    GPIO0_FIODIR |= GP0_OUTPUT;
    GPIO0_FIODIR = GP0_OUTPUT_HIGH;

    GPIO1_FIODIR |= GP1_OUTPUT;
    GPIO1_FIODIR = GP1_OUTPUT_HIGH;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * Configure CPU.
 */
static void config_cpu( void )
{
    // Interrupt vectors are in FLASH.
    SCB_MEMMAP = SCB_MEMMAP_UFL;
    // Setup and turn on the MAM.
    MAM_TIM = MAM_TIM_4;
    MAM_CR = MAM_CR_FULL;

    // Disable all interrupts.
    VICIntEnClr = VICIntEnClr_MASK;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * Configure clocks.
 */
static void config_clocks( void )
{
    // Activate the PLL.
    SCB_PLLCON = SCB_PLLCON_PLLE;
    // Configure CPU clock.
    SCB_PLLCFG = SCB_PLL_CONFIG;
    SCB_PLLFEED = SCB_PLLFEED_FEED1;
    SCB_PLLFEED = SCB_PLLFEED_FEED2;

    // Wait for the PLL to lock...
    while (!(SCB_PLLSTAT & SCB_PLLSTAT_PLOCK));

    // ...before connecting it using the feed sequence again.
    SCB_PLLCON = (SCB_PLLCON_PLLE | SCB_PLLCON_PLLC);
    SCB_PLLFEED = SCB_PLLFEED_FEED1;
    SCB_PLLFEED = SCB_PLLFEED_FEED2;

    // Setup the peripheral bus frequency.
    SCB_VPBDIV = CPUCLK_DIV;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * Hardware initialization routine, called from main at system startup.
 */
void hardware_init(void)
{
    DBG_WRITE_STR("HW configuration.");

    // CPU initialization.
    config_cpu();

    // Clocks initialization.
    config_clocks();

    // GPIO initialization.
    config_GPIO();
}
//------------------------------------------------------------------------------
