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

#include <string.h>

#include <shared/debug.h>

#include "virtdisk.h"

//------------------------------------------------------------------------------
/**
 * FAT12 boot sector partition.
 */
const char g_FATboot[BLOCKSIZE] = {
    // See http://en.wikipedia.org/wiki/FAT12#Boot_Sector
    0xeb, 0x3c, 0x90, // Jump instruction.
    0x6d, 0x6b, 0x64, 0x6f, 0x73, 0x66, 0x73, 0x00, // OEM Name
    0x00, 0x02, // Bytes per sector
    0x01, // Sectors per FS cluster.
    0x01, 0x00, // Reserved sector count
    0x01, // Number of FATs
    0x70, 0x00, // Number of root directory entries
    0xe0, 0x07, // Total sectors
    0xf8, // Media descriptor
    0x01, 0x00, // Sectors per FAT table
    0x20, 0x00, // Sectors per track
    0x40, 0x00, // Number of heads
    0x00, 0x00, 0x00, 0x00, // Number of hidden sectors
    0x00, 0x00, 0x00, 0x00, // Large number of sectors.
    0x00, // Physical drive number
    0x00, // Reserved
    0x29, // Extended boot signature
    0xCE, 0xFA, 0x5C, 0xD1, // Disk ID (serial number)
    0x56, 0x49, 0x52, 0x54, 0x46, 0x53, 0x20, 0x20, 0x20, 0x20, 0x20, // Volume label
    0x46, 0x41, 0x54, 0x31, 0x32, 0x20, 0x20, 0x20, // FAT file system type
    0x0e, 0x1f, // OS boot code
    0xbe, 0x5b, 0x7c, 0xac, 0x22, 0xc0, 0x74, 0x0b, 0x56, 0xb4, 0x0e, 0xbb, 0x07, 0x00, 0xcd, 0x10,
    0x5e, 0xeb, 0xf0, 0x32, 0xe4, 0xcd, 0x16, 0xcd, 0x19, 0xeb, 0xfe, 0x54, 0x68, 0x69, 0x73, 0x20,
    0x69, 0x73, 0x20, 0x6e, 0x6f, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61, 0x62, 0x6c,
    0x65, 0x20, 0x64, 0x69, 0x73, 0x6b, 0x2e, 0x20, 0x20, 0x50, 0x6c, 0x65, 0x61, 0x73, 0x65, 0x20,
    0x69, 0x6e, 0x73, 0x65, 0x72, 0x74, 0x20, 0x61, 0x20, 0x62, 0x6f, 0x6f, 0x74, 0x61, 0x62, 0x6c,
    0x65, 0x20, 0x66, 0x6c, 0x6f, 0x70, 0x70, 0x79, 0x20, 0x61, 0x6e, 0x64, 0x0d, 0x0a, 0x70, 0x72,
    0x65, 0x73, 0x73, 0x20, 0x61, 0x6e, 0x79, 0x20, 0x6b, 0x65, 0x79, 0x20, 0x74, 0x6f, 0x20, 0x74,
    0x72, 0x79, 0x20, 0x61, 0x67, 0x61, 0x69, 0x6e, 0x20, 0x2e, 0x2e, 0x2e, 0x20, 0x0d, 0x0a, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x55, 0xaa
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
const char g_FAT[BLOCKSIZE] = {
    // See http://www.ecma-international.org/publications/standards/Ecma-107.htm
    0xf8, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// See http://en.wikipedia.org/wiki/FAT12#Directory_table
typedef struct {
    unsigned char name[8];
    unsigned char ext[3];
    unsigned char attribute;
    unsigned char reserved;
    unsigned char create_time_ms;
    unsigned short create_time;
    unsigned short create_date;
    unsigned short access_date;
    unsigned short ea_index;
    unsigned short modify_time;
    unsigned short modify_date;
    unsigned short start_cluster;
    unsigned int file_size;
} FATDirEntry_t;

// First 16 FAT root directory entries (1 sector)
const FATDirEntry_t g_FATroot[16] = {
    {
        .name = { 'T', 'E', 'S', 'T', '1', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0002,
        .file_size = 0x00000200
    },
    {
        .name = { 'T', 'E', 'S', 'T', '2', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0003,
        .file_size = 0x00000200
    },
    {
        .name = { 'T', 'E', 'S', 'T', '3', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0004,
        .file_size = 0x00000200
    },
    {
        .name = { 'T', 'E', 'S', 'T', '4', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0005,
        .file_size = 0x00000200
    },
    {
        .name = { 'T', 'E', 'S', 'T', '5', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0006,
        .file_size = 0x00000200
    },
    {
        .name = { 'T', 'E', 'S', 'T', '6', ' ', ' ', ' '},
        .ext  = { 'T', 'X', 'T'},
        .attribute = 0x20,
        .reserved = 0x00,
        .create_time_ms = 0x3E,
        .create_time = 0xA301,
        .create_date = 0x3D55,
        .access_date = 0x3D55,
        .ea_index = 0x0000,
        .modify_time = 0xA302,
        .modify_date = 0x3D55,
        .start_cluster = 0x0007,
        .file_size = 0x00000200
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    },
    {
        .name = { '\x00', ' ', ' ', ' ', ' ', ' ', ' ', ' '},
        .ext  = { ' ', ' ', ' '},
        .attribute = 0x00,
        .reserved = 0x00,
        .create_time_ms = 0x00,
        .create_time = 0x0000,
        .create_date = 0x0000,
        .access_date = 0x0000,
        .ea_index = 0x0000,
        .modify_time = 0x0000,
        .modify_date = 0x0000,
        .start_cluster = 0x0000,
        .file_size = 0x00000000
    }
};
//------------------------------------------------------------------------------

static char testfs[BLOCKSIZE];

//------------------------------------------------------------------------------
int fsRead(unsigned char * a_buff, unsigned int a_sector,
        unsigned int a_nof_sectors, unsigned char a_part_type)
{
    DBG_WRITE_STR("fsRead");
    DBG_WRITE_INT(a_sector);
//    DBG_WRITE_INT(a_nof_sectors);
//    DBG_WRITE_BYTE(a_part_type);

    if (a_sector == 0)
    {
        memcpy(a_buff, g_FATboot, BLOCKSIZE);
        return TRUE;
    }

    if (a_sector == 1/*Reserved sector count*/)
    {
        // FAT table.
        memcpy(a_buff, g_FAT, BLOCKSIZE);
        return TRUE;
    }

    if ((a_sector >= 2/*Reserved sector count+Sectors per FAT table*/) &&
             (a_sector <= 8/*Reserved sector count+Sectors per FAT table+rootSize*/))
    {
        // rotSize = (Number of root directory entries*sizeof(FATDirEntry_t)/512
        if (a_sector == 2)
        {
            // We have data only for first sector of FAT root.
            memcpy(a_buff, g_FATroot, BLOCKSIZE);
        }
        else
        {
            memset(a_buff, 0, BLOCKSIZE);
        }
        return TRUE;
    }

    // a_sector >= 9
    // a_sector to file decode: a_sector-7 => file.start_cluster.
    // 7=rootSize=(112*sizeof(FATDirEntry_t)/512)
    // 112 = Number of root directory entries.
    // 512 = sector size.
    //TODO: Return virtual file content.
    memcpy(a_buff, testfs, BLOCKSIZE);

    return TRUE;
}
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
int fsWrite(const unsigned char * a_buff, unsigned int a_sector,
        unsigned int a_nof_sectors, unsigned char a_part_type)
{
    DBG_WRITE_STR("fsWrite");
    DBG_WRITE_INT(a_sector);
//    DBG_WRITE_INT(a_nof_sectors);
//    DBG_WRITE_BYTE(a_part_type);

    if (a_sector < 9)
    {
        return TRUE;
    }

    //TODO: Virtual write to file.
    // a_sector to file decode: a_sector-7 => file.start_cluster.
    // 7=(112*sizeof(FATDirEntry_t)/512)
    // 112 = Number of root directory entries.
    // 512 = sector size.
    memcpy(testfs, a_buff, BLOCKSIZE);

    return TRUE;
}
//------------------------------------------------------------------------------