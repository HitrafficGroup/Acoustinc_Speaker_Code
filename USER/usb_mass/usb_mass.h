

#ifndef __USB_MOUSE_H
#define __USB_MOUSE_H

#include "usb_type.h"

#define CURSOR_STEP     5

void InitUsbMouse(void);
void RunUsbMouse(void);
void UsbConnect(void);
void UsbDisconnect(void);
void Enter_LowPowerMode(void);
void Leave_LowPowerMode(void);
void Get_SerialNum(void);

#endif
