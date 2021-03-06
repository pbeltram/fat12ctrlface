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

#include "FreeRTOS.h"

#include <usb/usbapi.h>
#include <virtdisk/virtdisk.h>
#include <shared/debug.h>

#include "mscbot.h"
#include "usbmass.h"

#define LE_WORD(x) ((x) & 0xff), ((x) >> 8)

static unsigned char abClassReqData[4];

static const unsigned char abDescriptors[] =
{
    //  Device descriptor
    0x12,
    DESC_DEVICE,
    LE_WORD(0x0200),        // bcdUSB
    0x00,                   // bDeviceClass
    0x00,                   // bDeviceSubClass
    0x00,                   // bDeviceProtocol
    MAX_PACKET_SIZE0,       // bMaxPacketSize
    LE_WORD(0xFFFF),        // idVendor
    LE_WORD(0x0003),        // idProduct
    LE_WORD(0x0100),        // bcdDevice
    0x01,                   // iManufacturer
    0x02,                   // iProduct
    0x03,                   // iSerialNumber
    0x01,                   // bNumConfigurations

    //  Configuration descriptor
    0x09,
    DESC_CONFIGURATION,
    LE_WORD(32),             // wTotalLength
    0x01,                    // bNumInterfaces
    0x01,                    // bConfigurationValue
    0x00,                    // iConfiguration
    0xC0,                    // bmAttributes
    0x32,                    // bMaxPower

    //  Interface
    0x09,
    DESC_INTERFACE,
    0x00,                    // bInterfaceNumber
    0x00,                    // bAlternateSetting
    0x02,                    // bNumEndPoints
    0x08,                    // bInterfaceClass = mass storage
    0x06,                    // bInterfaceSubClass = transparent SCSI
    0x50,                    // bInterfaceProtocol = BOT
    0x00,                    // iInterface

    //  EP
    0x07,
    DESC_ENDPOINT,
    MSC_BULK_IN_EP,           // bEndpointAddress
    0x02,                     // bmAttributes = bulk
    LE_WORD(MAX_PACKET_SIZE64), // wMaxPacketSize
    0x00,                     // bInterval

    //  EP
    0x07,
    DESC_ENDPOINT,
    MSC_BULK_OUT_EP,           // bEndpointAddress
    0x02,                      // bmAttributes = bulk
    LE_WORD(MAX_PACKET_SIZE64),  // wMaxPacketSize
    0x00,                      // bInterval

    //  String descriptors
    0x04,
    DESC_STRING,
    LE_WORD(0x0409),

    0x0E,
    DESC_STRING,
    'L', 0, 'P', 0, 'C', 0, 'U', 0, 'S', 0, 'B', 0,

    0x12,
    DESC_STRING,
    'P', 0, 'r', 0, 'o', 0, 'd', 0, 'u', 0, 'c', 0, 't', 0, 'X', 0,

    0x1A,
    DESC_STRING,
    'D', 0, 'E', 0, 'A', 0, 'D', 0,
    'C', 0, '0', 0, 'D', 0, 'E', 0,
    'C', 0, 'A', 0, 'F', 0, 'E', 0,

    // terminating zero
    0
};


static int usbmassHandleClassRequest(TSetupPacket *pSetup, int *piLen,
        unsigned char **ppbData)
{
    if (pSetup->wIndex != 0)
    {
        DBG_WRITE_STR("Invalid idx.");
        return FALSE;
    }

    if (pSetup->wValue != 0)
    {
        DBG_WRITE_STR("Invalid val.");
        return FALSE;
    }

    switch (pSetup->bRequest)
    {
        //  Get max LUN (always return no LUNs)
        case 0xFE:
        {
            *ppbData[0] = 0;
            *piLen = 1;
        }
        break;

        //  MSC reset
        case 0xFF:
        {
            if (pSetup->wLength > 0)
            {
                return FALSE;
            }

            mscbotReset();
        }
        break;

        default:
        {
            DBG_WRITE_STR("Unhandled class.");
        }
        return FALSE;
    }

    return TRUE;
}

void usbmassInit(void)
{
    portENTER_CRITICAL();

    if (diskInitialize(0))
    {
        usbRegisterHandlers();
        usbRegisterDescriptors(abDescriptors);
        usbRegisterRequestHandler(REQTYPE_TYPE_CLASS, usbmassHandleClassRequest, abClassReqData);
        usbHardwareRegisterEPIntHandler(MSC_BULK_IN_EP, mscbotBulkIn);
        usbHardwareRegisterEPIntHandler(MSC_BULK_OUT_EP, mscbotBulkOut);
        usbHardwareNakIntEnable(INACK_BI);
        usbSetupInterruptHandler();
    }
    else
    {
        DBG_WRITE_STR("Failed to initialize disk.");
    }

    portEXIT_CRITICAL();

    usbHardwareConnect(TRUE);

    DBG_WRITE_STR("Exit usbmassInit.");
}
