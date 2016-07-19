
#pragma once

#include "Object.h"
#include "EffekseerEffect.generated.h"

UCLASS()
class EFFEKSEER_API UEffekseerEffect : public UObject
{
	GENERATED_BODY()

private:
	void*	effectPtr = nullptr;
	void ReleaseEffect();
public:
	void Load(const uint8_t* data, int32_t size);

	void BeginDestroy() override;

	UPROPERTY(EditAnywhere)
	int32 Version = -1;
};