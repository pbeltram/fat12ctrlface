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

#ifndef _VIRTDISK_H_
#define _VIRTDISK_H_

//------------------------------------------------------------------------------
#define TRUE (1)
#define FALSE (0)
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#define BLOCKSIZE   512
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
#define IOCTL_GET_SECTOR_COUNT          1
#define IOCTL_GET_SECTOR_SIZE           2
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int diskInitialize(unsigned char drv);
int diskRead(unsigned char disk, unsigned char *buff, unsigned int sector,
        unsigned char count);
int diskWrite(unsigned char disk, const unsigned char *buff,
        unsigned int sector, unsigned char count);
int diskIoctl(unsigned char disk, unsigned char ctrl, void *buff);

int fsRead(unsigned char * a_buff, unsigned int a_sector,
        unsigned int a_nof_sectors, unsigned char a_part_type);
int fsWrite(const unsigned char * a_buff, unsigned int a_sector,
        unsigned int a_nof_sectors, unsigned char a_part_type);
//------------------------------------------------------------------------------

#endif // _VIRTDISK_H_
