
#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>

class EffekseerInternalCurve
	: public ::Effekseer::Curve
{
public:
	void* UserData = nullptr;

	EffekseerInternalCurve(void* data, int32_t size)
		: ::Effekseer::Curve(data, size)
	{
	}
};