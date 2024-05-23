#include "EffekseerProceduralModel.h"

#include "EffekseerUECompatibility.h"
#include "StaticMeshResources.h"

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
#include "DynamicMeshBuilder.h"
#endif

class FProceduralModelMeshRenderData
{
public:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	FProceduralModelMeshRenderData(Effekseer::Model& model, ERHIFeatureLevel::Type InFeatureLevel)
		: VertexFactory(InFeatureLevel, "FProceduralModelMeshProxy")

	{
		TArray<uint32> indexBuffer;
		TArray<FDynamicMeshVertex> vertices;
		vertices.SetNumUninitialized(model.GetVertexCount());

		for (int i = 0; i < model.GetVertexCount(); i++)
		{
			auto v = model.GetVertexes()[i];

			vertices[i] = FDynamicMeshVertex(
				EffekseerUE::UEFVector3f(v.Position.X, v.Position.Z, v.Position.Y),
				EffekseerUE::UEFVector3f(v.Tangent.X, v.Tangent.Z, v.Tangent.Y),
				EffekseerUE::UEFVector3f(v.Normal.X, v.Normal.Z, v.Normal.Y),
				EffekseerUE::UEFVector2f(v.UV.X, v.UV.Y),
				FColor(v.VColor.R, v.VColor.G, v.VColor.B, v.VColor.A));
		}

		for (int i = 0; i < model.GetFaceCount(); i++)
		{
			indexBuffer.Add(model.GetFaces()[i].Indexes[2]);
			indexBuffer.Add(model.GetFaces()[i].Indexes[1]);
			indexBuffer.Add(model.GetFaces()[i].Indexes[0]);
		}

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, vertices, 4);
		IndexBuffer.Indices = indexBuffer;

		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);
	}

	void ReleaseResouces()
	{
		BeginReleaseResource(&VertexBuffers.PositionVertexBuffer);
		BeginReleaseResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginReleaseResource(&VertexBuffers.ColorVertexBuffer);
		BeginReleaseResource(&IndexBuffer);
		BeginReleaseResource(&VertexFactory);
	}
};

UEFfekseerProceduralModel::UEFfekseerProceduralModel()
{
}

UEFfekseerProceduralModel::~UEFfekseerProceduralModel()
{
	ES_SAFE_DELETE(renderData_);
}

void UEFfekseerProceduralModel::BeginDestroy()
{
	if (renderData_ != nullptr)
	{
		renderData_->ReleaseResouces();
	}
	Super::BeginDestroy();
}

void UEFfekseerProceduralModel::Init(Effekseer::ModelRef modelPtr)
{
	modelPtr_ = modelPtr;
}

bool UEFfekseerProceduralModel::GenerateRenderDataIfRequired(ERHIFeatureLevel::Type inFeatureLevel)
{
	if (renderData_)
	{
		return true;
	}

	if (modelPtr_->GetFaceCount() == 0)
	{
		return false;
	}

	renderData_ = new FProceduralModelMeshRenderData(*modelPtr_.Get(), inFeatureLevel);

	return true;
}

void UEFfekseerProceduralModel::Render(int32_t viewIndex, FMeshElementCollector& collector, const FMatrix& localToWorld, FMaterialRenderProxy* materialRenderProxy)
{
	if (!renderData_)
	{
		return;
	}

	auto& meshElement = collector.AllocateMesh();
	auto& element = meshElement.Elements[0];

	FDynamicPrimitiveUniformBuffer& dynamicPrimitiveUniformBuffer = collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

#if ENGINE_MAJOR_VERSION == 5 && ENGINE_MINOR_VERSION >= 4
	auto bounds = FBoxSphereBounds(EForceInit::ForceInit);
	dynamicPrimitiveUniformBuffer.Set(collector.GetRHICommandList(), localToWorld, localToWorld, bounds, bounds, bounds, false, false, false);
#else
	dynamicPrimitiveUniformBuffer.Set(localToWorld, localToWorld, FBoxSphereBounds(EForceInit::ForceInit), FBoxSphereBounds(EForceInit::ForceInit), false, false, false, false);
#endif

	element.PrimitiveUniformBufferResource = &dynamicPrimitiveUniformBuffer.UniformBuffer;

	meshElement.MaterialRenderProxy = materialRenderProxy;
	meshElement.VertexFactory = &renderData_->VertexFactory;
	meshElement.Type = PT_TriangleList;
	element.IndexBuffer = &renderData_->IndexBuffer;
	element.FirstIndex = 0;
	element.NumPrimitives = modelPtr_->GetFaceCount();

	element.MinVertexIndex = 0;
	element.MaxVertexIndex = modelPtr_->GetVertexCount() - 1;
	meshElement.LODIndex = 0;

	element.MaxScreenSize = 0.0f;
	element.MinScreenSize = -1.0f;

	collector.AddMesh(viewIndex, meshElement);
}