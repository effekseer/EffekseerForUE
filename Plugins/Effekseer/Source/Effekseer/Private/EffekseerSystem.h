#pragma once

#include "CoreMinimal.h"
#include "EffekseerColorSpaceType.h"
#include "EffekseerRendererImplemented.h"
#include <Effekseer.h>
#include <mutex>
#include <stdint.h>

class FMeshElementCollector;

namespace EffekseerRendererUE
{

class EffekseerSystem
{
public:
	std::mutex Mutex;
	::Effekseer::ManagerRef Manager = nullptr;
	::Effekseer::RefPtr<RendererImplemented> Renderer = nullptr;
	::Effekseer::ServerRef Server = nullptr;

	void Init(int32_t maxSprite, int32_t threadCount, EEffekseerColorSpaceType colorSpace);

	void Render(const FMatrix& localToWorld, const FMatrix& viewMatrix, int viewIndex, FMeshElementCollector& collector);

	void Render(const std::vector<Effekseer::Handle>& handles, const FMatrix& localToWorld, const FMatrix& viewMatrix, int viewIndex, FMeshElementCollector& collector);
};

} // namespace EffekseerRendererUE