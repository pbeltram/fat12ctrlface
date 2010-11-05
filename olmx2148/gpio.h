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

//----------------------------------------------------------------------------
// P0.x
//----------------------------------------------------------------------------
#define P0_GPI_VBUS_DIN     	14

#define GP0_INPUT (\
	(1 << P0_GPI_VBUS_DIN))

#define GP0_OUTPUT (0)

#define GP0_OUTPUT_HIGH (0)
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
// P1.x
//----------------------------------------------------------------------------
#define P1_GPO_GREENLED         24 ///< Set P1.24 as GREEN LED output.

#define GP1_INPUT (0)

#define GP1_OUTPUT (\
    (1 << P1_GPO_GREENLED))

#define GP1_OUTPUT_HIGH (\
    (1 << P1_GPO_GREENLED))
//----------------------------------------------------------------------------
