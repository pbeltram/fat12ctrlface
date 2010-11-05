/*
 * $Id$
 */

#include <FreeRTOS.h>

#include "usbapi.h"

//
//  Data storage area for standard requests
//
static unsigned char abStdReqData[8];

//
//  USB reset handler
//
static void usbHandleReset(unsigned char bDevStatus __attribute__ ((unused)))
{
}

//
//
//
int usbRegisterHandlers(void)
{
    usbHardwareInit();
    usbHardwareRegisterDevIntHandler(usbHandleReset);
    usbHardwareRegisterEPIntHandler(0x00, usbHandleControlTransfer);
    usbHardwareRegisterEPIntHandler(0x80, usbHandleControlTransfer);
    usbHardwareEndpointConfig(0x00, MAX_PACKET_SIZE0);
    usbHardwareEndpointConfig(0x80, MAX_PACKET_SIZE0);
    usbRegisterRequestHandler(REQTYPE_TYPE_STANDARD, usbHandleStandardRequest, abStdReqData);

    return TRUE;
}
