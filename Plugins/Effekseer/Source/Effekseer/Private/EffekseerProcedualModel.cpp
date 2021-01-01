#include "EffekseerProcedualModel.h"
#include "StaticMeshResources.h"

class FProcedualModelMeshRenderData
{
public:
	FStaticMeshVertexBuffers VertexBuffers;
	FDynamicMeshIndexBuffer32 IndexBuffer;
	FLocalVertexFactory VertexFactory;

	FProcedualModelMeshRenderData(Effekseer::Model& model, ERHIFeatureLevel::Type InFeatureLevel)
		: VertexFactory(InFeatureLevel, "FProcedualModelMeshProxy")

	{
		TArray<uint32> indexBuffer;
		TArray<FDynamicMeshVertex> vertices;
		vertices.SetNumUninitialized(model.GetVertexCount());

		for (int i = 0; i < model.GetVertexCount(); i++)
		{
			auto v = model.GetVertexes()[i];

			vertices[i] = FDynamicMeshVertex(
				FVector(v.Position.X, v.Position.Y, v.Position.Z), 
				FVector(v.Tangent.X, v.Tangent.Y, v.Tangent.Z),
				FVector(v.Normal.X, v.Normal.Y, v.Normal.Z),
				FVector2D(v.UV.X, v.UV.Y),
				FColor(v.VColor.R, v.VColor.B, v.VColor.B, v.VColor.A));
		}

		for (int i = 0; i < model.GetFaceCount(); i++)
		{
			indexBuffer.Add(model.GetFaces()[i].Indexes[0]);
			indexBuffer.Add(model.GetFaces()[i].Indexes[1]);
			indexBuffer.Add(model.GetFaces()[i].Indexes[2]);
		}

		VertexBuffers.InitFromDynamicVertex(&VertexFactory, vertices, 4);
		IndexBuffer.Indices = indexBuffer;

		BeginInitResource(&VertexBuffers.PositionVertexBuffer);
		BeginInitResource(&VertexBuffers.StaticMeshVertexBuffer);
		BeginInitResource(&VertexBuffers.ColorVertexBuffer);
		BeginInitResource(&IndexBuffer);
		BeginInitResource(&VertexFactory);
	}

	~FProcedualModelMeshRenderData()
	{
		VertexBuffers.PositionVertexBuffer.ReleaseResource();
		VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();
		VertexBuffers.ColorVertexBuffer.ReleaseResource();
		IndexBuffer.ReleaseResource();
		VertexFactory.ReleaseResource();
	}
};

UEFfekseerProcedualModel::UEFfekseerProcedualModel()
{
}

UEFfekseerProcedualModel::~UEFfekseerProcedualModel()
{
	ES_SAFE_DELETE(renderData_);
}

void UEFfekseerProcedualModel::Init(Effekseer::ModelRef modelPtr)
{
	modelPtr_ = modelPtr;
}

bool UEFfekseerProcedualModel::GenerateRenderDataIfRequired(ERHIFeatureLevel::Type inFeatureLevel)
{
	if (renderData_)
	{
		return true;
	}

	if (modelPtr_->GetFaceCount() == 0)
	{
		return false;
	}

	renderData_ = new FProcedualModelMeshRenderData(*modelPtr_.Get(), inFeatureLevel);

	return true;
}

void UEFfekseerProcedualModel::Render(int32_t viewIndex, FMeshElementCollector& collector, const FMatrix& localToWorld, FMaterialRenderProxy* materialRenderProxy)
{
	if (!renderData_)
	{
		return;
	}

	auto& meshElement = collector.AllocateMesh();
	auto& element = meshElement.Elements[0];

	FDynamicPrimitiveUniformBuffer& dynamicPrimitiveUniformBuffer = collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
	dynamicPrimitiveUniformBuffer.Set(localToWorld, localToWorld, FBoxSphereBounds(EForceInit::ForceInit), FBoxSphereBounds(EForceInit::ForceInit), false, false, false, false);
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