#ifndef BATTERYCOMMANDS_H
#define BATTERYCOMMANDS_H

#include "types.h"
#include "SLABCP2112.h"
#include "SMBusConfig.h"

UINT16 ReadWord(HID_SMBUS_DEVICE device, INT *data, INT battery_register);
UINT16 WriteWord(HID_SMBUS_DEVICE device, UINT16 send_data, BYTE battery_register);
UINT16 ReadTextBlock(HID_SMBUS_DEVICE device, char *block, INT *block_length, INT battery_register);
UINT16 ReadDataBlock(HID_SMBUS_DEVICE device, BYTE *block, INT *block_length, INT battery_register);
UINT16 ReadFlash(HID_SMBUS_DEVICE device, UINT16 regAddr, BYTE dataLen, BYTE *data);
UINT16 WriteFlash(HID_SMBUS_DEVICE device, UINT16 regAddr, BYTE dataLen, BYTE *data);

UINT16 writeRequest(HID_SMBUS_DEVICE device, BYTE *send_data, BYTE dataLen);
UINT16 readRequest(HID_SMBUS_DEVICE device, BYTE *return_data, BYTE dataLen);

#endif // BATTERYCOMMANDS_H
