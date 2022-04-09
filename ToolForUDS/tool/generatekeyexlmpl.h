#ifndef GENERATEKEYEXLMPL_H
#define GENERATEKEYEXLMPL_H

#include <QObject>

enum VKeyGenResultEx
{
  KGRE_Ok = 0,
  KGRE_BufferToSmall = 1,
  KGRE_SecurityLevelInvalid = 2,
  KGRE_VariantInvalid = 3,
  KGRE_UnspecifiedError = 4
};

VKeyGenResultEx GenerateKeyExOpt(
      const unsigned char* ipSeedArray, unsigned int iSeedArraySize,
      const unsigned int iSecurityLevel, const unsigned char* ipCfgArray, unsigned int iCfgArraySize,
      unsigned char* iopKeyArray, unsigned int iMaxKeyArraySize,
      unsigned int& oActualKeyArraySize);


#endif // GENERATEKEYEXLMPL_H
