
#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>

class EffekseerInternalModel
	: public ::Effekseer::Model
{
public:

	void*	UserData = nullptr;

	EffekseerInternalModel(void* data, int32_t size)
		: ::Effekseer::Model(data, size)
	{
	}
};