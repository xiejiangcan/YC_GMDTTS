#ifndef USBINTERFACE_H
#define USBINTERFACE_H

#include <Windows.h>
#include <SetupAPI.h>
#include <devguid.h>
#include <InitGuid.h>
#include <stdio.h>
#include <wchar.h>
#include <dbt.h>

#include "ToolForUDS_global.h"


#define INDEX_ALL -1

enum DeviceClass{
    DeviceClass_NONE = -1,
    DeviceClass_MOUSE,
    DeviceClass_SYSTEM,
    DeviceClass_USB,
    DeviceClass_NET,
    DeviceClass_KEYBOARD,
    DeviceClass_CANUSB
};

enum Property{
    Property_NONE          = -1,
    Property_DESCRIPTION   = 0x00000000,
    Property_HARDWAREID    = 0x00000001,
    Property_COMPATIBLEIDS = 0x00000002
};


int getDeviceProperty(
    DeviceClass deviceClass,
    Property    deviceProperty,
    int         index,
    char*       buffer);

QStringList canDeviceList();

const GUID GUID_CANUSB = {0xaa87d8ea, 0xe78f, 0x466d,
              {0x90, 0x40, 0x02, 0x25, 0x4a, 0x61, 0xe3, 0x09}};

#endif // USBINTERFACE_H
