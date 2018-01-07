
#pragma once

#include "Components/PrimitiveComponent.h"
#include "Runtime/Launch/Resources/Version.h"

#include "EffekseerEffect.h"
#include "EffekseerHandle.h"

#include <map>

#include "EffekseerSystemComponent.generated.h"

enum class EffekseerUpdateData_CommandType
{
	Play,
	SetP,
	SetR,
	SetS,
	SetPRS,
	Stop,
	StopRoot,
	StopAll,
	SetSpeed,
	SetAllColor,
};


class EffekseerUpdateData_Command
{
public:
	int32_t	ID = -1;
	void*	Effect = nullptr;
	FVector	Position;
	FRotator	Rotation;
	FVector	Scale;
	FColor	AllColor;
	float	Speed;

	EffekseerUpdateData_CommandType	Type;
};

class EffekseerUpdateData
{
public:
	TArray<EffekseerUpdateData_Command>	Commands;
	TMap<UEffekseerMaterial*, UMaterialInstanceDynamic*> Materials;
	std::map<EffekseerMaterial, UMaterialInstanceDynamic*> NMaterials;

	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> LightingDynamicMaterials;

	float	DeltaTime = 0;

	EffekseerUpdateData();
	virtual ~EffekseerUpdateData();
};

UCLASS(ClassGroup = (Effekseer), meta = (BlueprintSpawnableComponent))
class EFFEKSEER_API UEffekseerSystemComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

private:
	FPrimitiveSceneProxy*	sceneProxy = nullptr;
	EffekseerUpdateData*	currentUpdateData = nullptr;
	int32_t					nextInternalHandle = 0;
	TMap<int32_t, int32_t>	internalHandle2EfkHandle;

public:
	UEffekseerSystemComponent();
	virtual ~UEffekseerSystemComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	virtual UMaterialInterface* GetMaterial(int32 ElementIndex) const override;

#if ENGINE_MINOR_VERSION == 14
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials) const override;
#else
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials = false) const override;
#endif

	virtual int32 GetNumMaterials() const override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual void BeginPlay() override;

	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

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

	std::map<EffekseerMaterial, UMaterialInstanceDynamic*> NMaterials;

	UPROPERTY(Transient)
	TMap<UEffekseerMaterial*, UMaterialInstanceDynamic*> Materials;

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

	UFUNCTION(BlueprintCallable, Category = "Control")
	FEffekseerHandle Play(UEffekseerEffect* effect, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectPosition(FEffekseerHandle handle, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectRotation(FEffekseerHandle handle, FRotator rotation);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectScaling(FEffekseerHandle handle, FVector scaling);

	UFUNCTION(BlueprintCallable, Category = "Control")
	bool Exists(FEffekseerHandle handle) const;

	UFUNCTION(BlueprintCallable, Category = "Control")
	void Stop(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopRoot(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectSpeed(FEffekseerHandle handle, float speed);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectAllColor(FEffekseerHandle handle, FColor color);
};
