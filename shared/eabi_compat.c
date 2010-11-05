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
// Linker workarounds.
int raise(int a_sig);
void __aeabi_unwind_cpp_pr0(void);
void __aeabi_unwind_cpp_pr1(void);
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 * This function is invoked if div0 is hit at arithmetic operations.
 * @note Including math operations in ARM FW requires this function to be
 * declared.
 */
int raise(int a_sig)
{
    while (1) ; // Loop forever here in case raise was hit.

    return 0;
}
//----------------------------------------------------------------------------

//----------------------------------------------------------------------------
/**
 *  Dummy function to avoid linker complaints.
 *  @note Check http://www.mail-archive.com/u-boot@lists.denx.de/msg32245.html.
 */
void __aeabi_unwind_cpp_pr0(void) { };
void __aeabi_unwind_cpp_pr1(void) { };
//----------------------------------------------------------------------------
