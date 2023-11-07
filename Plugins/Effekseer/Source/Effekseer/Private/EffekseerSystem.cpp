#include "EffekseerSystem.h"

namespace EffekseerRendererUE
{

namespace
{

Effekseer::Matrix44 UEViewMatrixToEffekseerCameraMatrix(const FMatrix& ueViewMatrix)
{
	::Effekseer::Matrix44 ret;

	for (int i = 0; i < 4; i++)
	{
		for (int j = 0; j < 4; j++)
		{
			ret.Values[i][j] = static_cast<float>(ueViewMatrix.M[i][j]);
		}
	}

	std::swap(ret.Values[1][0], ret.Values[2][0]);
	std::swap(ret.Values[1][1], ret.Values[2][1]);
	std::swap(ret.Values[1][2], ret.Values[2][2]);

	ret.Values[0][2] = -ret.Values[0][2];
	ret.Values[1][2] = -ret.Values[1][2];
	ret.Values[2][2] = -ret.Values[2][2];
	ret.Values[3][2] = -ret.Values[3][2];

	return ret;
}

void CalculateCameraDirectionAndPosition(const Effekseer::Matrix44& matrix, Effekseer::Vector3D& direction, Effekseer::Vector3D& position)
{
	const auto& mat = matrix;

	direction = -::Effekseer::Vector3D(matrix.Values[0][2], matrix.Values[1][2], matrix.Values[2][2]);

	{
		auto localPos = ::Effekseer::Vector3D(-mat.Values[3][0], -mat.Values[3][1], -mat.Values[3][2]);
		auto f = ::Effekseer::Vector3D(mat.Values[0][2], mat.Values[1][2], mat.Values[2][2]);
		auto r = ::Effekseer::Vector3D(mat.Values[0][0], mat.Values[1][0], mat.Values[2][0]);
		auto u = ::Effekseer::Vector3D(mat.Values[0][1], mat.Values[1][1], mat.Values[2][1]);

		position = r * localPos.X + u * localPos.Y + f * localPos.Z;
	}
}
} // namespace

void EffekseerSystem::Init(int32_t maxSprite, int32_t threadCount, EEffekseerColorSpaceType colorSpace)
{
	Manager = ::Effekseer::Manager::Create(maxSprite);
#ifndef __EMSCRIPTEN__
	if (threadCount >= 2)
	{
		Manager->LaunchWorkerThreads(threadCount);
	}
#endif
	Renderer = ::EffekseerRendererUE::RendererImplemented::Create();
	Renderer->Initialize(maxSprite, colorSpace);

	Manager->SetSpriteRenderer(Renderer->CreateSpriteRenderer());
	Manager->SetRibbonRenderer(Renderer->CreateRibbonRenderer());
	Manager->SetRingRenderer(Renderer->CreateRingRenderer());
	Manager->SetTrackRenderer(Renderer->CreateTrackRenderer());
}

void EffekseerSystem::Render(const FMatrix& localToWorld, const FMatrix& viewMatrix, int viewIndex, FMeshElementCollector& collector)
{
	Renderer->SetLocalToWorld(localToWorld);
	Renderer->SetViewIndex(viewIndex);
	Renderer->SetMeshElementCollector(&collector);

	auto cameraMatrix = UEViewMatrixToEffekseerCameraMatrix(viewMatrix);

	Renderer->SetCameraMatrix(cameraMatrix);
	Renderer->BeginRendering();

	Effekseer::Vector3D direction;
	Effekseer::Vector3D position;
	CalculateCameraDirectionAndPosition(cameraMatrix, direction, position);

	Effekseer::Manager::DrawParameter drawParam;
	drawParam.CameraFrontDirection = direction;
	drawParam.CameraPosition = position;
	drawParam.IsSortingEffectsEnabled = true;

	Manager->Draw(drawParam);

	Renderer->EndRendering();
}

void EffekseerSystem::Render(const std::vector<Effekseer::Handle>& handles, const FMatrix& localToWorld, const FMatrix& viewMatrix, int viewIndex, FMeshElementCollector& collector)
{
	Renderer->SetLocalToWorld(localToWorld);
	Renderer->SetViewIndex(viewIndex);
	Renderer->SetMeshElementCollector(&collector);

	auto cameraMatrix = UEViewMatrixToEffekseerCameraMatrix(viewMatrix);

	Renderer->SetCameraMatrix(cameraMatrix);
	Renderer->BeginRendering();

	Effekseer::Vector3D direction;
	Effekseer::Vector3D position;
	CalculateCameraDirectionAndPosition(cameraMatrix, direction, position);

	Effekseer::Manager::DrawParameter drawParam;
	drawParam.CameraFrontDirection = direction;
	drawParam.CameraPosition = position;
	drawParam.IsSortingEffectsEnabled = true;

	for (const auto& handle : handles)
	{
		Manager->DrawHandle(handle);
	}

	Renderer->EndRendering();
}

} // namespace EffekseerRendererUE
