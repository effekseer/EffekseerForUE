#include "EffekseerPrivatePCH.h"
#include "EffekseerSystemComponent.h"

#include "DynamicMeshBuilder.h"

#include "EffekseerNative.h"
#include "EffekseerRendererImplemented.h"

#include <mutex>

class FEffekseerSystemSceneProxy : public FPrimitiveSceneProxy
{
private:
	static const int32_t	particleMax = 10000;
	::Effekseer::Manager*	effekseerManager = nullptr;
	::EffekseerRendererUE4::RendererImplemented*	effekseerRenderer = nullptr;
	::Effekseer::CriticalSection	effekseerCS;

public:
	FEffekseerSystemSceneProxy(const UEffekseerSystemComponent* InComponent)
		: FPrimitiveSceneProxy(InComponent)
		, Material(InComponent->Material)
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start : Construct");
		effekseerManager = ::Effekseer::Manager::Create(particleMax, false);
		effekseerRenderer = ::EffekseerRendererUE4::RendererImplemented::Create();
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "End : Construct");
	}

	virtual ~FEffekseerSystemSceneProxy()
	{
		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start : Destructer");
		if (effekseerManager != nullptr)
		{
			effekseerManager->Destroy();
			effekseerManager = nullptr;
		}

		if (effekseerRenderer != nullptr)
		{
			effekseerRenderer->Destory();
			effekseerRenderer = nullptr;
		}

		GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "End : Destructer");
	}

	virtual void GetDynamicMeshElements(const TArray<const FSceneView*>& Views, const FSceneViewFamily& ViewFamily, uint32 VisibilityMap, FMeshElementCollector& Collector) const override
	{
		// TODO いずれ高速に処理できる方法を考える。
		//effekseerCS.Enter();

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start : GetDynamicMeshElements");

		if (!Material)
		{
			return;
		}

		
		for (int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++)
		{
			if (!(VisibilityMap & (1 << ViewIndex))) continue;

			
			{
				effekseerRenderer->BeginRendering();
				effekseerManager->Draw();
				effekseerRenderer->EndRendering();
			}


			FDynamicMeshBuilder meshBuilder;
			
			meshBuilder.AddVertex(FVector(0, 0, 0), FVector2D(0, 0), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);
			meshBuilder.AddVertex(FVector(0, 100, 0), FVector2D(1, 0), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);
			meshBuilder.AddVertex(FVector(100, 0, 0), FVector2D(0, 1), FVector(1, 0, 0), FVector(1, 1, 0), FVector(0, 0, 1), FColor::White);

			meshBuilder.AddTriangle(0, 1, 2);

			FMaterialRenderProxy *matProxy = Material->GetRenderProxy(false);

			meshBuilder.GetMesh(GetLocalToWorld(), matProxy, SDPG_World, false, false, ViewIndex, Collector);
		}

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "End : GetDynamicMeshElements");

		//effekseerCS.Leave();
	}

	virtual FPrimitiveViewRelevance GetViewRelevance(const FSceneView* View) const override
	{
		bool bVisible = true;
		FPrimitiveViewRelevance Result;
		Result.bDrawRelevance = IsShown(View);
		Result.bDynamicRelevance = true;
		Result.bShadowRelevance = IsShadowCast(View);

		return Result;
	}
	virtual uint32 GetMemoryFootprint() const { return sizeof(*this) + GetAllocatedSize(); }
	uint32 GetAllocatedSize() const { return FPrimitiveSceneProxy::GetAllocatedSize(); }

	void UpdateData_RenderThread()
	{
		//effekseerCS.Enter();

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "Start : UpdateData");

		// TODO いずれ高速に処理できる方法を考える。
		
		{
			if (effekseerManager != nullptr)
			{
				effekseerManager->Update();
			}
		}
		

		//GEngine->AddOnScreenDebugMessage(-1, 15.0f, FColor::Red, "End : UpdateData");
		//effekseerCS.Leave();
	}

	// This function can be called out of renderThread.
	void UpdateData()
	{
		ENQUEUE_UNIQUE_RENDER_COMMAND_ONEPARAMETER(
			ParticleUpdateDataCommand,
			FEffekseerSystemSceneProxy*, Proxy, this,
			{
				Proxy->UpdateData_RenderThread();
			}
		);
	}

protected:
	UMaterialInterface *Material;
};

UEffekseerSystemComponent::UEffekseerSystemComponent()
{
	bWantsBeginPlay = true;
	PrimaryComponentTick.bCanEverTick = true;
}

void UEffekseerSystemComponent::BeginPlay()
{
	Super::BeginPlay();
}

void UEffekseerSystemComponent::TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction)
{
	auto sp = (FEffekseerSystemSceneProxy*)sceneProxy;
	sp->UpdateData();

	Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
}

FPrimitiveSceneProxy* UEffekseerSystemComponent::CreateSceneProxy()
{
	auto sp = new FEffekseerSystemSceneProxy(this);
	sceneProxy = sp;
	return sp;
}

UMaterialInterface* UEffekseerSystemComponent::GetMaterial(int32 ElementIndex) const
{
	if (ElementIndex == 0)
	{
		return Material;
	}
	else
	{
		return nullptr;
	}
}

void UEffekseerSystemComponent::GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials) const
{
	if (Material)
	{
		OutMaterials.Add(Material);
	}
}

int32 UEffekseerSystemComponent::GetNumMaterials()const
{
	if (Material)
	{
		return 1;
	}
	else
	{
		return 0;
	}
}


FBoxSphereBounds UEffekseerSystemComponent::CalcBounds(const FTransform& LocalToWorld) const
{
	return FBoxSphereBounds(LocalToWorld.GetLocation(), FVector(10, 10, 10), 10);
}

void UEffekseerSystemComponent::Play(UEffekseerEffect* effect, FVector position)
{
	if (effect == nullptr) return;
	if (effect->GetNativePtr() == nullptr) return;

	auto p = (::Effekseer::Effect*)effect->GetNativePtr();

	p->AddRef();
	// TODO send Effect
}