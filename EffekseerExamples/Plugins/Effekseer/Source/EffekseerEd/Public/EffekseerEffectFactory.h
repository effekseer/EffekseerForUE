
#pragma once

#include "Factories/Factory.h"
#include "EffekseerEffectFactory.generated.h"

UCLASS()
class UEffekseerEffectFactory : public UFactory
{
	GENERATED_UCLASS_BODY()


	virtual bool DoesSupportClass(UClass* Class) override;
	virtual UClass* ResolveSupportedClass() override;

	virtual UObject* FactoryCreateBinary(
		UClass* InClass,
		UObject* InParent,
		FName InName,
		EObjectFlags Flags,
		UObject* Context,
		const TCHAR* Type,
		const uint8*& Buffer,
		const uint8* BufferEnd,
		FFeedbackContext* Warn) override;

};
