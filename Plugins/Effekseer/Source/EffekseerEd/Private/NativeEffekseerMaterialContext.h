
#pragma once

#include "CoreMinimal.h"
#include "EffekseerMaterial/efkMat.Library.h"
#include "EffekseerMaterial/efkMat.Models.h"
#include "EffekseerMaterial/efkMat.TextExporter.h"

struct NativeEffekseerMaterialContext
{
	std::shared_ptr<EffekseerMaterial::Material> material;
	std::shared_ptr<EffekseerMaterial::Library> library;
	std::shared_ptr<EffekseerMaterial::Node> outputNode;
	EffekseerMaterial::TextExporterResult result;
	std::map<uint64_t, std::string> uniformNames;
	std::map<uint64_t, std::string> gradientNames;
	std::map<uint64_t, std::string> textureNames;
	bool isEffectScaleRequired = false;

	static std::shared_ptr<NativeEffekseerMaterialContext> Load(const uint8* data, int size, const char* basePath);
};