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

#ifndef _DEBUG_H_
#define _DEBUG_H_

#ifdef DEBUG
#include <dcc/dcc_stdio.h>
  #define DBG_WRITE_STR(_x_)   dbg_write_str((const char *) _x_)
  #define DBG_WRITE_INT(_x_)   dbg_write_u32(_x_)
  #define DBG_WRITE_SHORT(_x_) dbg_write_u16(_x_);
  #define DBG_WRITE_BYTE(_x_)  dbg_write_u8(_x_);
#else
  #define DBG_WRITE_STR(_x_)
  #define DBG_WRITE_INT(_x_)
  #define DBG_WRITE_SHORT(_x_)
  #define DBG_WRITE_BYTE(_x_)
#endif // DEBUG

#endif // _DEBUG_H_
