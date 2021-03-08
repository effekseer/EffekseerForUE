
#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>

class UEffekseerModel;

class UEFfekseerProceduralModel;

class EffekseerInternalModel
	: public ::Effekseer::Model
{
public:
	UEffekseerModel* UserData = nullptr;

	UEFfekseerProceduralModel* ProceduralData = nullptr;

	EffekseerInternalModel(void* data, int32_t size)
		: ::Effekseer::Model(data, size)
	{
	}

	EffekseerInternalModel(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces)
		: ::Effekseer::Model(vertecies, faces)
	{
	}
};