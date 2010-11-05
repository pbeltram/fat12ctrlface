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

#include "platform.h"

//----------------------------------------------------------------------------
/**
 * ARM CPU specific function that generates 4 CPU cycle delay.
 */
inline void delay_4clocks(unsigned long delayval)
{
    __asm__ __volatile__(
            "L_LOOPUS_%=:         \n"
            "subs    %0, %0, #1     \n"
            "bne    L_LOOPUS_%=    \n"
            :  /* no outputs */ : "r" (delayval));
}
//----------------------------------------------------------------------------
