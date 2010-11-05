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

#include <FreeRTOS.h>
#include <task.h>

#include <shared/lpc214x.h>
#include <shared/debug.h>

#include <usbmass/usbmass.h>

#include "gpio.h"
#include "hwinit.h"

//------------------------------------------------------------------------------
#ifdef DEBUG
const char __attribute__((used)) g_boardVer[] = "$BoardVersion: OLMEX2148-DEBUG $";
#else
const char __attribute__((used)) g_boardVer[] = "$BoardVersion: OLMEX2148 $";
#endif

#define MAIN_PROCESSING_TASK_PRIORITY_d     (tskIDLE_PRIORITY + 4)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
/**
 * Main processing task.
 */
static void main_processing(void * a_param)
{
    DBG_WRITE_STR("Main processing task.");

    for (;;)
    {
        // Toggle REDLED.
        P1_BX(P1_GPO_GREENLED);

        // Sleep for 200 msec
        vTaskDelay(MSEC2TICKS_m(200));
    }
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int main( void )
{
    DBG_WRITE_STR("Starting main.");

    hardware_init();

    //    usbserInit();
    usbmassInit();

    // Start main processing task.
    xTaskCreate(main_processing, (const signed portCHAR *) "MainProc",
            configMINIMAL_STACK_SIZE, NULL, MAIN_PROCESSING_TASK_PRIORITY_d, NULL);

    vTaskStartScheduler();

    // Should never end here.
    return 0;
}
//------------------------------------------------------------------------------
