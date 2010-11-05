/*
 * $Id$
 */

#include <FreeRTOS.h>
#include <task.h>

#include "usbISR.h"
#include "usbapi.h"


/** Installed device interrupt handler */
static TFnDevIntHandler *_pfnDevIntHandler = NULL;

/** Installed endpoint interrupt handlers */
static TFnEPIntHandler *_apfnEPIntHandlers[16];

/** Installed frame interrupt handlers */
static TFnFrameHandler *_pfnFrameHandler = NULL;

/** convert from endpoint address to endpoint index */
#define EP2IDX(bEP) ((((bEP)&0xF)<<1)|(((bEP)&0x80)>>7))

/** convert from endpoint index to endpoint address */
#define IDX2EP(idx) ((((idx)<<7)&0x80)|(((idx)>>1)&0xF))

void usbISR(void) __attribute__ ((naked));

//
//  Local function to wait for a device interrupt (and clear it)
//
static void usbWaitForDeviceInterrupt(unsigned int dwIntr)
{
    while ((USB_DevIntSt & dwIntr) != dwIntr);

    USB_DevIntClr = dwIntr;
}

//
//  Local function to send a command to the USB protocol engine
//
static void usbHardwareCommand(unsigned char bCmd)
{
    USB_DevIntClr = USB_DevIntClr_CDFULL | USB_DevIntClr_CCEMTY;
    USB_CmdCode = 0x00000500 | (bCmd << 16);
    usbWaitForDeviceInterrupt(USB_DevIntSt_CCEMTY);
}

//
//  Local function to send a command + data to the USB protocol engine
//
static void usbHardwareCommandWrite(unsigned char bCmd, unsigned short bData)
{
    usbHardwareCommand(bCmd);
    USB_CmdCode = 0x00000100 | (bData << 16);
    usbWaitForDeviceInterrupt(USB_DevIntSt_CCEMTY);
}

//
//  Local function to send a command to the USB protocol engine and read data
//
static unsigned char usbHardwareCommandRead(unsigned char bCmd)
{
    usbHardwareCommand(bCmd);
    USB_CmdCode = 0x00000200 | (bCmd << 16);
    usbWaitForDeviceInterrupt(USB_DevIntSt_CDFULL);
    return USB_CmdData;
}

//
//  'Realizes' an endpoint, meaning that buffer space is reserved for
//  it. An endpoint needs to be realized before it can be used.
//    
//  From experiments, it appears that a USB reset causes USBReEP to
//  re-initialize to 3 (= just the control endpoints).
//  However, a USB bus reset does not disturb the USBMaxPSize settings.
//    
static void usbHardwareEndpointRealize(int idx, unsigned short wMaxPSize)
{
    USB_ReEP |= (1 << idx);
    USB_EpInd = idx;
    USB_MaxPSize = wMaxPSize;
    usbWaitForDeviceInterrupt(USB_DevIntSt_EPRLZED);
}

//
//  Enables or disables an endpoint
//
static void usbHardwareEndpointEnable(int idx, int fEnable)
{
    usbHardwareCommandWrite(CMD_EP_SET_STATUS | idx, fEnable ? 0 : EP_DA);
}

//
// Configures an endpoint and enables it
//
void usbHardwareEndpointConfig(unsigned char bEP, unsigned short wMaxPacketSize)
{
    int idx;

    idx = EP2IDX (bEP);
    usbHardwareEndpointRealize(idx, wMaxPacketSize);
    usbHardwareEndpointEnable(idx, TRUE);
}

//
//  Registers an endpoint event callback
//
void usbHardwareRegisterEPIntHandler(unsigned char bEP, TFnEPIntHandler *pfnHandler)
{
    int idx;

    idx = EP2IDX(bEP);

    _apfnEPIntHandlers[idx >> 1] = pfnHandler;
    USB_EpIntEn |= (1 << idx);
    USB_DevIntEn |= USB_DevIntEn_EPSLOW;
}

//
//  Registers an device status callback
//
void usbHardwareRegisterDevIntHandler(TFnDevIntHandler *pfnHandler)
{
    _pfnDevIntHandler = pfnHandler;
    USB_DevIntEn |= USB_DevIntEn_DEVSTAT;
}

//
//  Registers the frame callback
//
void usbHardwareRegisterFrameHandler(TFnFrameHandler *pfnHandler)
{
    _pfnFrameHandler = pfnHandler;
    USB_DevIntEn |= USB_DevIntEn_FRAME;
}

//
//  Sets the USB address.
//
void usbHardwareSetAddress(unsigned char bAddr)
{
    usbHardwareCommandWrite(CMD_DEV_SET_ADDRESS, DEV_EN | bAddr);
}

//
//  Connects or disconnects from the USB bus
//
void usbHardwareConnect(int fConnect)
{
    usbHardwareCommandWrite(CMD_DEV_STATUS, fConnect ? CON : 0);
}

//
//  Enables interrupt on NAK condition
//    
//  For IN endpoints a NAK is generated when the host wants to read data
//  from the device, but none is available in the endpoint buffer.
//  For OUT endpoints a NAK is generated when the host wants to write data
//  to the device, but the endpoint buffer is still full.
//  
//  The endpoint interrupt handlers can distinguish regular (ACK) interrupts
//  from NAK interrupt by checking the bits in their bEPStatus argument.
//  
void usbHardwareNakIntEnable(unsigned char bIntBits)
{
    usbHardwareCommandWrite(CMD_DEV_SET_MODE, bIntBits);
}

//
//  Gets the stalled property of an endpoint
//
int usbHardwareEndpointIsStalled(unsigned char bEP)
{
    int idx = EP2IDX(bEP);

    return (usbHardwareCommandRead(CMD_EP_SELECT | idx) & EP_STATUS_STALLED);
}

//
//  Sets the stalled property of an endpoint
//
void usbHardwareEndpointStall (unsigned char bEP, int fStall)
{
    int idx = EP2IDX(bEP);

    usbHardwareCommandWrite(CMD_EP_SET_STATUS | idx, fStall ? EP_ST : 0);
}

//
//  Writes data to an endpoint buffer
//
int usbHardwareEndpointWrite(unsigned char bEP, unsigned char *pbBuf, int iLen)
{
    int idx;

    idx = EP2IDX(bEP);

    USB_Ctrl = USB_Ctrl_WREN | ((bEP & 0xf) << 2);
    USB_TxPLen = iLen;

    while (USB_Ctrl & USB_Ctrl_WREN)
    {
        USB_TxData = (pbBuf[3] << 24) | (pbBuf[2] << 16) | (pbBuf[1] << 8) | pbBuf[0];
        pbBuf += 4;
    }

    USB_Ctrl = 0;

    usbHardwareCommand(CMD_EP_SELECT | idx);
    usbHardwareCommand(CMD_EP_VALIDATE_BUFFER);

    return iLen;
}

//
//  Reads data from an endpoint buffer
//
int usbHardwareEndpointRead(unsigned char bEP, unsigned char *pbBuf, int iMaxLen)
{
    int i, idx;
    unsigned int dwData, dwLen;

    idx = EP2IDX(bEP);

    USB_Ctrl = USB_Ctrl_RDEN | ((bEP & 0xf) << 2);

    do
    {
        dwLen = USB_RxPLen;
    }
    while ((dwLen & USB_RxPLen_PKTRDY) == 0);

    if ((dwLen & USB_RxPLen_DV) == 0)
        return -1;

    dwLen &= USB_RxPLen_PKTLENGTH_MASK;

    for (dwData = 0, i = 0; i < (int) dwLen; i++)
    {
        if (!(i % 4))
            dwData = USB_RxData;

        if (pbBuf && (i < iMaxLen))
            pbBuf[i] = dwData & 0xff;

        dwData >>= 8;
    }

    USB_Ctrl = 0;

    usbHardwareCommand(CMD_EP_SELECT | idx);
    usbHardwareCommand(CMD_EP_CLEAR_BUFFER);

    return dwLen;
}


//
//  Sets the 'configured' state.
//
void usbHardwareConfigDevice(int fConfigured)
{
    usbHardwareCommandWrite(CMD_DEV_CONFIG, fConfigured ? CONF_DEVICE : 0);
}

//
//
//
void usbSetupInterruptHandler(void)
{
    //
    //  Set up USB interrupt, use highest priority ISR slot
    //
    VICIntSelect &= ~VICIntSelect_USB;
    VICVectAddr1 = (portLONG) usbISR;
    VICVectCntl1 = VICVectCntl_ENABLE | VICChannel_USB;
    VICIntEnable = VICIntEnable_USB;
}

//
//  USB interrupt handler
//
static void usbISR_Handler(void)
{
    unsigned int dwStatus;
    unsigned int dwIntBit;
    unsigned int dwEpIntSt, dwIntMask;
    unsigned char bEPStat, bDevStat, bStat;
    unsigned short wFrame;
    int i;
    portBASE_TYPE higherPriorityTaskWoken = pdFALSE;

    dwStatus = USB_DevIntSt;

    if (dwStatus & USB_DevIntSt_FRAME)
    {
        USB_DevIntClr = USB_DevIntClr_FRAME;

        if (_pfnFrameHandler != NULL)
        {
            wFrame = usbHardwareCommandRead(CMD_DEV_READ_CUR_FRAME_NR);
            _pfnFrameHandler(wFrame);
        }
    }

    if (dwStatus & USB_DevIntSt_DEVSTAT)
    {
        USB_DevIntClr = USB_DevIntClr_DEVSTAT;
        bDevStat = usbHardwareCommandRead(CMD_DEV_STATUS);

        if (bDevStat & (CON_CH | SUS_CH | RST))
        {
            bStat = ((bDevStat & CON) ? DEV_STATUS_CONNECT : 0) |
                    ((bDevStat & SUS) ? DEV_STATUS_SUSPEND : 0) |
                    ((bDevStat & RST) ? DEV_STATUS_RESET   : 0);

            if (_pfnDevIntHandler != NULL)
                _pfnDevIntHandler(bStat);
        }
    }

    if (dwStatus & USB_DevIntSt_EPSLOW)
    {
        USB_DevIntClr = USB_DevIntClr_EPSLOW;
        dwIntMask = 0xffffffff;

        for (i = 0; i < 32; i++)
        {
            dwIntBit = (1 << i);
            dwIntMask <<= 1;
            dwEpIntSt = USB_EpIntSt;

            if (dwEpIntSt & dwIntBit)
            {
                USB_EpIntClr = dwIntBit;
                usbWaitForDeviceInterrupt(USB_DevIntSt_CDFULL);
                bEPStat = USB_CmdData;

                bStat = ((bEPStat & EPSTAT_FE)  ? EP_STATUS_DATA    : 0) |
                        ((bEPStat & EPSTAT_ST)  ? EP_STATUS_STALLED : 0) |
                        ((bEPStat & EPSTAT_STP) ? EP_STATUS_SETUP   : 0) |
                        ((bEPStat & EPSTAT_EPN) ? EP_STATUS_NACKED  : 0) |
                        ((bEPStat & EPSTAT_PO)  ? EP_STATUS_ERROR   : 0);

                if (_apfnEPIntHandlers[i >> 1]) {
                    higherPriorityTaskWoken |= _apfnEPIntHandlers[i >> 1](IDX2EP(i), bStat);
                }
            }

            if (!(dwEpIntSt & dwIntMask))
                break;
        }
    }

    VICVectAddr = 0;

    if (higherPriorityTaskWoken)
    {
        portYIELD_FROM_ISR();
    }
}

#pragma GCC diagnostic ignored "-Wnested-externs"
#pragma GCC diagnostic ignored "-Wredundant-decls"
void usbISR(void)
{
    portSAVE_CONTEXT ();
    usbISR_Handler();
    portRESTORE_CONTEXT ();
}
#pragma GCC diagnostic warning "-Wnested-externs"
#pragma GCC diagnostic warning "-Wredundant-decls"

//
//  Initializes the USB hardware
//     
//  This function assumes that the hardware is connected as shown in
//  section 10.1 of the LPC2148 data sheet:
//  * P0.31 controls a switch to connect a 1.5k pull-up to D+ if low.
//  * P0.23 is connected to USB VCC.
//
int usbHardwareInit(void)
{
    // Configure P0.23 for Vbus sense.
    PCB_PINSEL1 = (PCB_PINSEL1 & ~PCB_PINSEL1_P023_GPIO) | PCB_PINSEL1_P023_VBUS;
    GPIO0_FIODIR &= ~GPIO_IO_P23;
    // Configure P0.31 as USB connect indicator
    PCB_PINSEL1 = (PCB_PINSEL1 & ~PCB_PINSEL1_P031_GPIO) | PCB_PINSEL1_P031_CONNECT;

    //  Enable PUSB
    SCB_PCONP |= SCB_PCONP_PUSB;

    // Configure USB clock.
    USB_PLLCON = USB_PLLCON_PLLE; // Enable PLL. PLLE = 1
    USB_PLLCFG = (USB_PLLCFG_MUL4 | USB_PLLCFG_DIV2);
    // Feed PLL with settings.
    USB_PLLFEED = USB_PLLFEED_FEED1;
    USB_PLLFEED = USB_PLLFEED_FEED2;

    while (!(USB_PLLSTAT & USB_PLLSTAT_PLOCK)); // Wait until PLL locked (PLOCK==1).

    USB_PLLCON = (USB_PLLCON_PLLE | USB_PLLCON_PLLC); // Enable and connect PLLE = 1; PLLC = 1
    // Feed PLL with settings.
    USB_PLLFEED = USB_PLLFEED_FEED1;
    USB_PLLFEED = USB_PLLFEED_FEED2;

    //  Disable/clear all interrupts for now
    USB_DevIntEn = USB_DevIntEn_NONE;
    USB_DevIntClr = USB_DevIntClr_ALL;
    USB_DevIntPri = USB_DevIntPri_NONE;
    USB_EpIntEn = USB_EpIntEn_NONE;
    USB_EpIntClr = USB_EpIntClr_ALL;
    USB_EpIntPri = USB_EpIntPri_NONE;

    //  By default, only ACKs generate interrupts
    usbHardwareNakIntEnable(0);

    return TRUE;
}
