
#pragma once

#include "Components/PrimitiveComponent.h"
#include "CoreMinimal.h"
#include "EffekseerColorSpaceType.h"
#include "EffekseerEffect.h"
#include "EffekseerHandle.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Materials/MaterialInstanceDynamic.h"
#include "Runtime/Launch/Resources/Version.h"
#include <map>

#include "EffekseerSystemComponent.generated.h"

class EffekseerUpdateData;

UCLASS(ClassGroup = (Effekseer), meta = (BlueprintSpawnableComponent))
class EFFEKSEER_API UEffekseerSystemComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

private:
	FPrimitiveSceneProxy* sceneProxy_ = nullptr;
	EffekseerUpdateData* currentUpdateData = nullptr;
	int32_t nextInternalHandle_ = 0;
	TMap<int32_t, int32_t> internalHandle2EfkHandle_;
	bool isNetworkRunning_ = false;

	int dynamicMaterialCount_ = 0;

public:
	UEffekseerSystemComponent();
	virtual ~UEffekseerSystemComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;

	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;

	virtual int32 GetNumMaterials() const override;

	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	int MaxSprite = 10000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	EEffekseerColorSpaceType ColorSpace = EEffekseerColorSpaceType::Gamma;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	int ThreadCount = 2;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* OpaqueMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* TranslucentMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* AdditiveMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* SubtractiveMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* ModulateMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* LightingMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* DistortionTranslucentMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* DistortionAdditiveMaterial = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* Opaque_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* Translucent_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* Additive_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* Subtractive_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* Modulate_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* DistortionTranslucent_DD_Material = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Material)
	UMaterialInstanceConstant* DistortionAdditive_DD_Material = nullptr;

	std::map<EffekseerEffectMaterialKey, UMaterialInstanceDynamic*> NMaterials;

	UPROPERTY(Transient)
	TMap<UEffekseerEffectMaterialParameterHolder*, UMaterialInstanceDynamic*> GeneratedFixedMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> LightingDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> DistortionTranslucentDynamicMaterials;

	UPROPERTY(Transient)
	TMap<UTexture2D*, UMaterialInstanceDynamic*> DistortionAdditiveDynamicMaterials;

	void AssignMaterials(UEffekseerEffect* effect, TArray<UMaterialInterface*>* currentMaterials);

	UFUNCTION(BlueprintCallable, Category = "Control")
	FEffekseerHandle Play(UEffekseerEffect* effect, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectPosition(FEffekseerHandle handle, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectRotation(FEffekseerHandle handle, FQuat rotation);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectScaling(FEffekseerHandle handle, FVector scaling);

	UFUNCTION(BlueprintCallable, Category = "Control")
	bool Exists(FEffekseerHandle handle) const;

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SendTrigger(FEffekseerHandle handle, int index);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void Stop(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopRoot(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectSpeed(FEffekseerHandle handle, float speed);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectAllColor(FEffekseerHandle handle, FColor color);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectDynamicInput(FEffekseerHandle handle, int index, float value);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	int NetworkPort = 60000;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Network")
	bool DoStartNetworkAutomatically = false;

	UFUNCTION(BlueprintCallable, Category = "Network")
	void StartNetwork();

	UFUNCTION(BlueprintCallable, Category = "Network")
	void StopNetwork();
};
