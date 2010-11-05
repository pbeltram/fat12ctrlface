/*
 * $Id$
 */

/*
  LPCUSB, an USB device driver for LPC microcontrollers 
  Copyright (C) 2006 Bertrik Sikken (bertrik@sikken.nl)

  Redistribution and use in source and binary forms, with or without
  modification, are permitted provided that the following conditions are met:

  1. Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
  2. Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
  3. The name of the author may not be used to endorse or promote products
     derived from this software without specific prior written permission.

  THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
  IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
  OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
  IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, 
  INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
  NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
  DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
  THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
  (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
  THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#include <string.h>

#include <FreeRTOS.h>

#include <virtdisk/virtdisk.h>
#include <shared/debug.h>

#include "mscscsi.h"

/**
 * SBC2 mandatory SCSI commands.
 */
#define SCSI_CMD_TEST_UNIT_READY        0x00
#define SCSI_CMD_REQUEST_SENSE          0x03
#define SCSI_CMD_FORMAT_UNIT            0x04
#define SCSI_CMD_READ_6                 0x08 // not implemented yet.
#define SCSI_CMD_INQUIRY                0x12
#define SCSI_CMD_SEND_DIAGNOSTIC        0x1D // not implemented yet.
#define SCSI_CMD_READ_CAPACITY          0x25
#define SCSI_CMD_READ                   0x28
#define SCSI_CMD_REPORT_LUNS            0xA0 // not implemented yet.
#define SCSI_CMD_MODE_SENSE             0x1A
#define SCSI_CMD_PREVENTALLOW_REMOVAL   0x1E

/**
 * SBC2 optional SCSI commands.
 */
#define SCSI_CMD_WRITE_6           0x0A  // not implemented yet.
#define SCSI_CMD_WRITE             0x2A
#define SCSI_CMD_VERIFY            0x2F  // required for windows format.
#define SCSI_CMD_START_STOP        0x1B
#define SCSI_CMD_SYNCHRONIZE_CACHE 0x35

/**
 * Sense codes
 * Sense code, which is set on error conditions.
 * Hex: 00aabbcc, where aa=KEY, bb=ASC, cc=ASCQ
 */
#define WRITE_ERROR           0x00030C00
#define READ_ERROR            0x00031100
#define INVALID_CMD_OPCODE    0x00052000
#define INVALID_FIELD_IN_CDB  0x00052400
static unsigned int dwSense = 0x00000000;

const unsigned char g_InquiryRsp[] =
{
    0x00,   // PDT = direct-access device
    0x80,   // removeable medium bit = set
    0x05,   // version = complies to SPC3
    0x02,   // response data format = SPC3
    0x1F,   // additional length
    0x00,
    0x00,
    0x00,
    'L','P','C','U','S','B',' ',' ',  // vendor
    'M','a','s','s',' ','s','t','o',  // product
    'r','a','g','e',' ',' ',' ',' ',
    '0','.','1',' '                   // revision
};

//  Data for "request sense" command. The 0xff are filled in later
const unsigned char g_SenseRsp[] =
{
    0x70, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x0a,
    0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00,
    0x00, 0x00
};

//  Buffer for holding one block of disk data
static unsigned char abBlockBuf[BLOCKSIZE];

typedef struct {
    unsigned char bOperationCode;
    unsigned char abLBA[3];
    unsigned char bLength;
    unsigned char bControl;
} TCDB6;

void mscscsiReset(void)
{
    dwSense = 0x00000000;
}

/**
 *  Verifies a SCSI CDB and indicates the direction and amount of data
 *  that the device wants to transfer.
 *
 *  If this call fails, a sense code is set in dwSense.
 *
 *  IN  pbCDB     - Command data block
 *      iCDBLen   - Command data block len
 *  OUT *piRspLen - Length of intended response data:
 *      *pfDevIn  - TRUE if data is transferred from device-to-host
 *
 *  Returns a pointer to the data exchange buffer if successful,
 *  return NULL otherwise.
 */
unsigned char *mscscsiHandleCmd(unsigned char *pbCDB, unsigned char iCDBLen,
        unsigned int *piRspLen, int *pfDevIn)
{
    static const unsigned char aiCDBLen[] = { 6, 10, 10, 0, 16, 12, 0, 0 };
    TCDB6 * pCDB = (TCDB6 *) pbCDB;

    // default direction is from device to host
    *pfDevIn = TRUE;

    // check CDB length
    if (iCDBLen < aiCDBLen[(pCDB->bOperationCode >> 5) & 0x07])
    {
        DBG_WRITE_STR("SRB:Invalid CBD len");
        return NULL;
    }

    switch (pCDB->bOperationCode)
    {
        case SCSI_CMD_START_STOP:
        {
//            DBG_WRITE_STR("SRB:START_STOP");
            *piRspLen = 0;
        }
        break;

        case SCSI_CMD_SYNCHRONIZE_CACHE:
        {
//            DBG_WRITE_STR("SRB:SYNCHRONIZE_CACHE");
            *piRspLen = 0;
        }
        break;

        case SCSI_CMD_PREVENTALLOW_REMOVAL:
        {
//            DBG_WRITE_STR("SRB:PREVENTALLOW_REMOVAL");
            *piRspLen = 0;
        }
        break;

        case SCSI_CMD_MODE_SENSE:
        {
//            DBG_WRITE_STR("SRB:MODE_SENSE");
//            DBG_WRITE_BYTE(pbCDB[2] & 0x3F); // PageCode
//            DBG_WRITE_BYTE(pbCDB[2] & 0xC0); // Pcf
//            DBG_WRITE_BYTE(pbCDB[4]); // Allocation Length.

            dwSense = INVALID_FIELD_IN_CDB;
            *piRspLen = 0;
            return NULL;
        }
        break;

        case SCSI_CMD_TEST_UNIT_READY:
        {
//            DBG_WRITE_STR("SRB:TEST_UNIT_READY");
            *piRspLen = 0;
        }
        break;

        case SCSI_CMD_REQUEST_SENSE:
        {
//            DBG_WRITE_STR("SRB:REQUEST_SENSE");
            *piRspLen = MIN(18, pCDB->bLength);
        }
        break;

        case SCSI_CMD_FORMAT_UNIT:
        {
//            DBG_WRITE_STR("SRB:FORMAT_UNIT");
            *piRspLen = 0;
        }
        break;

        case SCSI_CMD_INQUIRY:
        {
//            DBG_WRITE_STR("SRB:INQUIRY");
            *piRspLen = MIN(36, pCDB->bLength);
        }
        break;

        case SCSI_CMD_READ_CAPACITY:
        {
//            DBG_WRITE_STR("SRB:READ_CAPACITY");
            *piRspLen = 8;
        }
        break;

        case SCSI_CMD_READ:
        {
//            unsigned int lba = (pbCDB[2] << 24) | (pbCDB[3] << 16) |
//                    (pbCDB[4] << 8) | (pbCDB[5]);
            unsigned int len = (pbCDB[7] << 8) | pbCDB[8];

//            DBG_WRITE_STR("SRB:READ");
//            DBG_WRITE_INT(lba);
//            DBG_WRITE_INT(len);

            *piRspLen = len * BLOCKSIZE;
        }
        break;

        case SCSI_CMD_WRITE:
        {
//            unsigned int lba = (pbCDB[2] << 24) | (pbCDB[3] << 16) |
//                    (pbCDB[4] << 8) | (pbCDB[5]);
            unsigned int len = (pbCDB[7] << 8) | pbCDB[8];

//            DBG_WRITE_STR("SRB:WRITE");
//            DBG_WRITE_INT(lba);
//            DBG_WRITE_INT(len);

            *piRspLen = len * BLOCKSIZE;
            *pfDevIn = FALSE;
        }
        break;

        case SCSI_CMD_VERIFY:
        {
//            unsigned int lba = (pbCDB[2] << 24) | (pbCDB[3] << 16) |
//                    (pbCDB[4] << 8) | (pbCDB[5]);
//            unsigned short len = (pbCDB[7] << 8) | pbCDB[8];

//            DBG_WRITE_STR("SRB:VERIFY");
//            DBG_WRITE_INT(lba);
//            DBG_WRITE_INT(len);

            *piRspLen = 0;

            if ((pbCDB[1] & 0x02) != 0)
            {
                DBG_WRITE_STR("SRB:VERIFY-BYTCHK not supported.");

                dwSense = INVALID_FIELD_IN_CDB;
                return NULL;
            }
        }
        break;

        default :
        {
            DBG_WRITE_STR("SRB:INVALID_CMD_OPCODE");
            DBG_WRITE_BYTE(pCDB->bOperationCode);

            dwSense = INVALID_CMD_OPCODE;
            *piRspLen = 0;
        }
        return NULL;
    }

    return abBlockBuf;
}

/**
 *  Handles a block of SCSI data.
 *
 *  IN      pbCDB    . Command data block
 *          iCDBLen  - Command data block len
 *  IN/OUT  pbData   - Data buffer
 *  IN      dwOffset - Offset in data
 *
 * Returns a pointer to the next data to be exchanged if successful,
 * returns NULL otherwise.
 */
unsigned char *mscscsiHandleData(unsigned char *pbCDB,
        unsigned char iCDBLen __attribute__ ((unused)),
        unsigned char *pbData, unsigned int a_offset)
{
    TCDB6 *pCDB = (TCDB6 *) pbCDB;
    switch (pCDB->bOperationCode)
    {
        case SCSI_CMD_START_STOP:
        {
            DBG_WRITE_STR("SRR:START_STOP");
            DBG_WRITE_BYTE(pbCDB[1] & 0x01); // Immed
        }
        break;

        case SCSI_CMD_SYNCHRONIZE_CACHE:
        {
            DBG_WRITE_STR("SRR:SYNCHRONIZE_CACHE");
            DBG_WRITE_BYTE(pbCDB[1] & 0x02); // Immed
        }
        break;

        case SCSI_CMD_PREVENTALLOW_REMOVAL:
        {
//            DBG_WRITE_STR("SRR:PREVENTALLOW_REMOVAL");
//            DBG_WRITE_BYTE((pbCDB[4] & 0x03)); // Prevent.
        }
        break;

        case SCSI_CMD_MODE_SENSE:
        {
            DBG_WRITE_STR("SRR:MODE_SENSE.");
            DBG_WRITE_BYTE(pbCDB[2] & 0x3F); // PageCode
            DBG_WRITE_BYTE(pbCDB[2] & 0xC0); // Pcf
            DBG_WRITE_BYTE(pbCDB[4]); // Allocation Length.
        }
        break;

        case SCSI_CMD_TEST_UNIT_READY:
        {
//            DBG_WRITE_STR("SRR:TEST_UNIT_READY");

            if (dwSense != 0)
            {
                return NULL;
            }
        }
        break;

        case SCSI_CMD_REQUEST_SENSE:
        {
            DBG_WRITE_STR("SRR:REQEST_SENSE");

            memcpy(pbData, g_SenseRsp, sizeof(g_SenseRsp));
            pbData[2]  = (dwSense >> 16) & 0xff;
            pbData[12] = (dwSense >> 8) & 0xff;
            pbData[13] = (dwSense >> 0) & 0xff;
            dwSense = 0;
        }
        break;

        case SCSI_CMD_FORMAT_UNIT:
        {
            DBG_WRITE_STR("SRR:FORMAT_UNIT");
        }
        break;

        case SCSI_CMD_INQUIRY:
        {
            DBG_WRITE_STR("SRR:INQUIRY");

            memcpy(pbData, g_InquiryRsp, sizeof(g_InquiryRsp));
        }
        break;

        case SCSI_CMD_READ_CAPACITY:
        {
            DBG_WRITE_STR("SRR:READ_CAPACITY");

            unsigned int block_count = 0;

            if (!diskIoctl(0, IOCTL_GET_SECTOR_COUNT, &block_count))
            {
                dwSense = READ_ERROR;
                DBG_WRITE_STR("SRR:READ_CAPACITY - diskIoctl failed.");
                return NULL;
            }

            pbData[0] = (block_count >> 24) & 0xFF;
            pbData[1] = (block_count >> 16) & 0xFF;
            pbData[2] = (block_count >> 8) & 0xFF;
            pbData[3] = (block_count >> 0) & 0xFF;
            pbData[4] = (BLOCKSIZE >> 24) & 0xFF;
            pbData[5] = (BLOCKSIZE >> 16) & 0xFF;
            pbData[6] = (BLOCKSIZE >> 8) & 0xFF;
            pbData[7] = (BLOCKSIZE >> 0) & 0xFF;
        }
        break;

        case SCSI_CMD_VERIFY:
        {
            DBG_WRITE_STR("SRR:VERIFY");
        }
        break;

        case SCSI_CMD_READ:
        {
            unsigned int lba = (pbCDB[2] << 24) | (pbCDB[3] << 16) |
                    (pbCDB[4] << 8) | (pbCDB[5]);
            unsigned int buf_pos = (a_offset & (BLOCKSIZE - 1));

//            DBG_WRITE_STR("SRR:READ");
//            DBG_WRITE_INT(lba);
//            DBG_WRITE_INT(a_offset);

            if (buf_pos == 0)
            {
                unsigned int block_no = lba + (a_offset / BLOCKSIZE);
                if (!diskRead(0, abBlockBuf, block_no, 1))
                {
                    DBG_WRITE_STR("SRR:READ - diskRead failed.");

                    dwSense = READ_ERROR;
                    return NULL;
                }
            }
            return abBlockBuf + buf_pos;
        }

        case SCSI_CMD_WRITE:
        {
            unsigned int lba = (pbCDB[2] << 24) | (pbCDB[3] << 16) |
                    (pbCDB[4] << 8) | (pbCDB[5]);
            unsigned int buf_pos = (a_offset + MAX_PACKET_SIZE64) & (BLOCKSIZE - 1);

//            DBG_WRITE_STR("SRR:WRITE");
//            DBG_WRITE_INT(lba);
//            DBG_WRITE_INT(a_offset);

            if (buf_pos == 0)
            {
                unsigned int block_no = lba + (a_offset / BLOCKSIZE);
                if (!diskWrite(0, abBlockBuf, block_no, 1))
                {
                    DBG_WRITE_STR("SRR:WRITE - diskWrite failed.");

                    dwSense = WRITE_ERROR;
                    return NULL;
                }
            }

            return abBlockBuf + buf_pos;
        }

        default :
        {
            DBG_WRITE_STR("SRR:INVALID_CMD_OPCODE");
            DBG_WRITE_BYTE(pCDB->bOperationCode);

            dwSense = INVALID_CMD_OPCODE;
        }
        return NULL;
    }

    return abBlockBuf;
}
