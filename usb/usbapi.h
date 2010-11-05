/*
 * $Id$
 */

#ifndef _USBAPI_H_
#define _USBAPI_H_

#include "usbstruct.h"    // for TSetupPacket

#define TRUE (1)
#define FALSE (0)
#define MIN(x,y) ((x)<(y)?(x):(y))
#define MAX(x,y)((x)>(y)?(x):(y))
#ifndef NULL
#define NULL ((void *) 0)
#endif // NULL

/*************************************************************************
  USB configuration
**************************************************************************/

#define MAX_PACKET_SIZE0  64    /**< maximum packet size for EP 0 */
#define MAX_PACKET_SIZE64   64  /**< 64 byte packet size */

/*************************************************************************
  USB hardware interface
**************************************************************************/

// endpoint status sent through callback
#define EP_STATUS_DATA    (1<<0)    /**< EP has data */
#define EP_STATUS_STALLED (1<<1)    /**< EP is stalled */
#define EP_STATUS_SETUP   (1<<2)    /**< EP received setup packet */
#define EP_STATUS_ERROR   (1<<3)    /**< EP data was overwritten by setup packet */
#define EP_STATUS_NACKED  (1<<4)    /**< EP sent NAK */

// device status sent through callback
#define DEV_STATUS_CONNECT  (1<<0)  /**< device just got connected */
#define DEV_STATUS_SUSPEND  (1<<2)  /**< device entered suspend state */
#define DEV_STATUS_RESET    (1<<4)  /**< device just got reset */

// interrupt bits for NACK events in usbHardwareNakIntEnable
// (these bits conveniently coincide with the LPC214x USB controller bit)
#define INACK_NONE  (0)         /**< no interrupts */
#define INACK_CI    (1<<1)      /**< interrupt on NACK for control in */
#define INACK_CO    (1<<2)      /**< interrupt on NACK for control out */
#define INACK_II    (1<<3)      /**< interrupt on NACK for interrupt in */
#define INACK_IO    (1<<4)      /**< interrupt on NACK for interrupt out */
#define INACK_BI    (1<<5)      /**< interrupt on NACK for bulk in */
#define INACK_BO    (1<<6)      /**< interrupt on NACK for bulk out */

int usbHardwareInit(void);
void usbSetupInterruptHandler(void);
void usbHardwareNakIntEnable(unsigned char bIntBits);
void usbHardwareConnect(int fConnect);
void usbHardwareSetAddress(unsigned char bAddr);
void usbHardwareConfigDevice(int fConfigured);

//
//  Endpoint operations
//
void usbHardwareEndpointConfig(unsigned char bEP, unsigned short wMaxPacketSize);
int  usbHardwareEndpointRead(unsigned char bEP, unsigned char *pbBuf, int iMaxLen);
int  usbHardwareEndpointWrite(unsigned char bEP, unsigned char *pbBuf, int iLen);
void usbHardwareEndpointStall(unsigned char bEP, int fStall);
int usbHardwareEndpointIsStalled(unsigned char bEP);

//
//  Endpoint interrupt handler callback
//
typedef int (TFnEPIntHandler)(unsigned char bEP, unsigned char bEPStatus);
void usbHardwareRegisterEPIntHandler(unsigned char bEP, TFnEPIntHandler *pfnHandler);

//
//  Device status handler callback
//
typedef void (TFnDevIntHandler)(unsigned char bDevStatus);
void usbHardwareRegisterDevIntHandler(TFnDevIntHandler *pfnHandler);

//
//  Frame event handler callback
//
typedef void (TFnFrameHandler)(unsigned short wFrame);
void usbHardwareRegisterFrameHandler(TFnFrameHandler *pfnHandler);


/*************************************************************************
  USB application interface
**************************************************************************/

// initialise the complete stack, including HW
int usbRegisterHandlers(void);

/** Request handler callback (standard, vendor, class) */
typedef int (TFnHandleRequest)(TSetupPacket *pSetup, int *piLen,
        unsigned char **ppbData);
void usbRegisterRequestHandler(int iType, TFnHandleRequest *pfnHandler,
        unsigned char *pbDataStore);
void usbRegisterCustomReqHandler(TFnHandleRequest *pfnHandler);

/** Descriptor handler callback */
typedef int (TFnGetDescriptor)(unsigned short wTypeIndex, unsigned short wLangID,
        int *piLen, unsigned char **ppbData);

/** Default standard request handler */
int usbHandleStandardRequest(TSetupPacket *pSetup, int *piLen,
        unsigned char **ppbData);

/** Default EP0 handler */
int usbHandleControlTransfer(unsigned char bEP, unsigned char bEPStat);

/** Descriptor handling */
void usbRegisterDescriptors(const unsigned char *pabDescriptors);
int usbGetDescriptor(unsigned short wTypeIndex, unsigned short wLangID,
        int *piLen, unsigned char **ppbData);

#endif // _USBAPI_H_
