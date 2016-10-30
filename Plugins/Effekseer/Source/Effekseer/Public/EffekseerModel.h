
#pragma once

#include "Object.h"
#include "EffekseerModel.generated.h"

UCLASS()
class EFFEKSEER_API UEffekseerModel : public UObject
{
	GENERATED_BODY()
private:
	void*			modelPtr = nullptr;
	TArray<uint8>	buffer;

	void LoadModel(const uint8_t* data, int32_t size, const TCHAR* path);
	void ReleaseModel();

public:
};