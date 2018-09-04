#include "EffekseerCustomVersion.h"

const int FEffekseerCustomVersion::EffekseerVersion = 1;
const FGuid FEffekseerCustomVersion::GUID = FGuid(0x11334A12, 0x2E554231, 0xA36791A3, 0xC5A1082C);
static FCustomVersionRegistration GRegisterEffekseerCustomVersion(FEffekseerCustomVersion::GUID, FEffekseerCustomVersion::EffekseerVersion, TEXT("EffekseerVer"));