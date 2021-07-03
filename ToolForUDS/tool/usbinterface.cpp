#include "usbinterface.h"

#include <QDebug>
#include <QStringList>
//#pragma comment(lib, "setupapi.lib")

#define LOG_DEBUG(msg) qDebug() << msg

static int wideCharToASCII(wchar_t* wchar_buffer, QByteArray& char_buffer)
{
    // Get the required number of bytes the transform needs
    DWORD dwNum = WideCharToMultiByte(CP_OEMCP, NULL, wchar_buffer, -1, NULL, 0, NULL, FALSE);
    char_buffer.resize(dwNum);
    WideCharToMultiByte(CP_OEMCP, NULL, wchar_buffer, -1, char_buffer.data(), dwNum, NULL, FALSE);
    return 0;
}

int retrieveDeviceProperty(
        int         index,
        DeviceClass deviceClass,
        QByteArray&       buffer)
{


    int res = 0;
    HDEVINFO hDevInfo;
    SP_DEVINFO_DATA DeviceInfoData = { sizeof(DeviceInfoData) };
    GUID guid;

    switch (deviceClass){
    case DeviceClass_SYSTEM:
        guid = GUID_DEVCLASS_SYSTEM;
        break;
    case DeviceClass_USB:
        guid = GUID_DEVCLASS_USB;
        break;
    case DeviceClass_MOUSE:
        guid = GUID_DEVCLASS_MOUSE;
        break;
    case DeviceClass_NET:
        guid = GUID_DEVCLASS_NET;
        break;
    case DeviceClass_KEYBOARD:
        guid = GUID_DEVCLASS_KEYBOARD;
        break;
    case DeviceClass_CANUSB:
        guid = GUID_CANUSB;
        break;
    default:
        break;
    }

    // Get device class information set handle that contains requested device information elements for a local computer
    hDevInfo = SetupDiGetClassDevs(
                &guid,
                0,
                0,
                DIGCF_PRESENT
                );
    if (hDevInfo == INVALID_HANDLE_VALUE)
    {
        res = GetLastError();
        return res;
    }

    // enumerute device information
    DWORD required_size = 0;

    // SetupDiEnumDeviceInfo: returns a SP_DEVINFO_DATA structure that specifies a device information element in a device information set
    if (index == INDEX_ALL)
    {
        for (int i = 0; SetupDiEnumDeviceInfo(hDevInfo, i, &DeviceInfoData); i++)
        {
            DWORD DataT;
            QVector<wchar_t> wcBuffer(2046);
            DWORD buffersize = 2046;
            DWORD req_bufsize = 0;
            QByteArray asciiBuffer;

            // retrieves a specified Plug and Play device property
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_DEVICEDESC,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            buffer.append(asciiBuffer);
        }
    }
    else
    {
        SetupDiEnumDeviceInfo(hDevInfo, index, &DeviceInfoData);

        DWORD DataT;
        QVector<wchar_t> wcBuffer(2046);
        DWORD buffersize = 2046;
        DWORD req_bufsize = 0;
        QByteArray asciiBuffer;

        {
            // retrieve DESCRIPTION
            LOG_DEBUG("retrieve DESCRIPTION");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_DEVICEDESC,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Device Description:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve HARDWAREID
            LOG_DEBUG("retrieve HARDWAREID");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_HARDWAREID,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Hardware ID:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve COMPATIBLEIDS
            LOG_DEBUG("retrieve COMPATIBLEIDS");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_COMPATIBLEIDS,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Compatible ID:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve SERVICE
            LOG_DEBUG("retrieve SERVICE");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_SERVICE,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Service:\t\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve CLASS
            LOG_DEBUG("retrieve CLASS");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_CLASS,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Class:\t\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve CLASSGUID
            LOG_DEBUG("retrieve CLASSGUID");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_CLASSGUID,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Class GUID:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve DRIVER
            LOG_DEBUG("retrieve DRIVER");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_DRIVER,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Driver:\t\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve MFG
            LOG_DEBUG("retrieve MFG");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_MFG,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("MFG:\t\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve LOCATION_INFORMATION
            LOG_DEBUG("retrieve LOCATION_INFORMATION");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_LOCATION_INFORMATION,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Location Information:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve DEVICE_POWER_DATA
            LOG_DEBUG("retrieve DEVICE_POWER_DATA");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_DEVICE_POWER_DATA,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Device Power Data:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve FRIENDLYNAME
            LOG_DEBUG("retrieve FRIENDLYNAME");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_FRIENDLYNAME,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Friendly name:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }

        {
            // retrieve LOCATION_PATHS
            LOG_DEBUG("retrieve LOCATION_PATHS");
            if (!SetupDiGetDeviceRegistryProperty(
                        hDevInfo,
                        &DeviceInfoData,
                        SPDRP_LOCATION_PATHS,
                        &DataT,
                        (LPBYTE)wcBuffer.data(),
                        buffersize,
                        &req_bufsize
                        ))
            {
                res = GetLastError();
                //continue;
            }

            QByteArray prefixBuffer("Location path:\t");
            wideCharToASCII(wcBuffer.data(), asciiBuffer);
            asciiBuffer.append('\n');
            prefixBuffer.append(asciiBuffer);
            buffer.append(prefixBuffer);
        }
    }

    if(!SetupDiDestroyDeviceInfoList(hDevInfo)){
        qDebug() << "destroy device infoList failed!";
    }
    return res;
}

int getDeviceProperty(
        DeviceClass deviceClass,
        Property    deviceProperty,
        int         index,
        QByteArray&       buffer)
{
    int res = 0;

    if (deviceClass == DeviceClass_NONE || deviceProperty == Property_NONE){
        res = -1;
        goto exit;
    }

    res = retrieveDeviceProperty(index, deviceClass, buffer);

exit:
    return res;
}

QStringList canDeviceList()
{
    QByteArray deviceInfo;
    QStringList deviceList;
    int res = getDeviceProperty(DeviceClass_CANUSB,
                                Property_DESCRIPTION,
                                INDEX_ALL, deviceInfo);
    if(res != 0)
        return QStringList();
    auto deviceInfoList = deviceInfo.split('\n');
    deviceInfoList.removeAt(deviceInfoList.size() - 1);
    for(int i = 0; i < deviceInfoList.size(); ++i){
        deviceList.append(QString(deviceInfoList[i]) + QString("_%1").arg(i));
    }

    return deviceList;
}
