
#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>

class UEffekseerModel;

class UEFfekseerProcedualModel;

class EffekseerInternalModel
	: public ::Effekseer::Model
{
public:
	UEffekseerModel* UserData = nullptr;

	UEFfekseerProcedualModel* ProcedualData = nullptr;

	EffekseerInternalModel(void* data, int32_t size)
		: ::Effekseer::Model(data, size)
	{
	}

	EffekseerInternalModel(const Effekseer::CustomVector<Vertex>& vertecies, const Effekseer::CustomVector<Face>& faces)
		: ::Effekseer::Model(vertecies, faces)
	{
	}
};