#pragma once

#include "CoreMinimal.h"
#include <Effekseer.h>

class EffekseerInternalTexture
	: public ::Effekseer::Backend::Texture
{
public:
	EffekseerInternalTexture() = default;
	virtual ~EffekseerInternalTexture() override = default;

	void* UserData = nullptr;
};