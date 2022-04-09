#include "generatekeyexlmpl.h"

#include <string>
#include <QDebug>

bool HexToDec(char* shex, unsigned char& idec)
{
    int   i, mid;
    int   len = strlen(shex);
    if (len > 8)     return   false;
    mid = 0;   idec = 0;
    for (i = 2; i < len; i++)
    {
        char ch = shex[i];
        if (shex[i] >= '0' && shex[i] <= '9')   mid = shex[i] - '0';
        else   if (shex[i] >= 'a' && shex[i] <= 'f')   mid = shex[i] - 'a' + 10;
        else   if (shex[i] >= 'A' && shex[i] <= 'F')   mid = shex[i] - 'A' + 10;
        else   return   false;
        mid <<= ((len - i - 1) << 2);
        idec |= mid;
    }
    return  true;
}

//val表示需要移位的数 n表示移位位数
//字节数乘以8代表一共多少位
//向右循环移n位的结果：假设数据一共size位，向左移size-n位，再与原数右移n位进行或操作的结果
unsigned char bit_move(unsigned char val, int n) {
    unsigned int size = 8;
    n = n % size;
    //return (val >> (size - n) | (val << n));//左移
    return (val << (size - n) | (val >> n));//右移
}

VKeyGenResultEx GenerateKeyExOpt(
        const unsigned char* ipSeedArray,           /* Array for the seed [in] */
        unsigned int iSeedArraySize,                /* Length of the array for the seed [in] */
        const unsigned int iSecurityLevel,          /* Security level [in] */
        const unsigned char* ipCfgArray,            /* Cfg Array [in]*/
        unsigned int iCfgArraySize,                 /* Cfg Array length [in]*/
        unsigned char* iopKeyArray,                 /* Array for the key [in, out] */
        unsigned int iMaxKeyArraySize,              /* Maximum length of the array for the key [in] */
        unsigned int& oActualKeyArraySize)          /* Length of the key [out] */
{
    if(iSeedArraySize != 4 || iCfgArraySize != 4)
    {
        qDebug() << "length is error";
        return KGRE_BufferToSmall;
    }

    QString seedStr = QString("seed:%1 %2 %3 %4").arg(ipSeedArray[0], 2, 16, QChar('0'))
            .arg(ipSeedArray[1], 2, 16, QChar('0'))
            .arg(ipSeedArray[2], 2, 16, QChar('0'))
            .arg(ipSeedArray[3], 2, 16, QChar('0'));
    QString constantStr = QString("constant:%1 %2 %3 %4").arg(ipCfgArray[0], 2, 16, QChar('0'))
            .arg(ipCfgArray[1], 2, 16, QChar('0'))
            .arg(ipCfgArray[2], 2, 16, QChar('0'))
            .arg(ipCfgArray[3], 2, 16, QChar('0'));

    qDebug() << seedStr;
    qDebug() << constantStr;
    //for security access with Services 0x27 03 -> 0x27 04
    unsigned int i;
    unsigned long  v0, v1, sum=0, delta=0x9E3779B9;
    unsigned char k[4];
    v0=(((unsigned long)ipSeedArray[0])<<24)+(((unsigned long)ipSeedArray[1])<<16)+(((unsigned long)ipSeedArray[2])<<8)+(unsigned long)ipSeedArray[3];
    v1=((unsigned long)(~v0));

    memcpy(k, ipCfgArray, 4);

    //
    if (iSecurityLevel > 1)
    {
        unsigned int right_offset = (iSecurityLevel - 1) * 3;
        for (int i = 0; i < 4; i++)
        {
            k[i] = bit_move(k[i], right_offset);
        }
    }


    for (i = 0; i < 2; i++)
    {
        v0 += (((v1 << 4) ^ (v1 >> 5)) + v1) ^ (sum + k[sum & 3]);
        sum += delta;
        v1 += (((v0 << 4) ^ (v0 >> 5)) + v0) ^ (sum + k[(sum >> 11) & 3]);
    }
    iopKeyArray[0]=(unsigned char)((v0&0xff000000)>>24);
    iopKeyArray[1]=(unsigned char)((v0&0x00ff0000)>>16);
    iopKeyArray[2]=(unsigned char)((v0&0x0000ff00)>>8);
    iopKeyArray[3]=(unsigned char)(v0&0x000000ff);
    oActualKeyArraySize=4;

    return KGRE_Ok;
}


