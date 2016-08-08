
#pragma once

#include "Components/PrimitiveComponent.h"

#include "EffekseerEffect.h"
#include "EffekseerHandle.h"

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
};

class EffekseerUpdateData_Command
{
public:
	int32_t	ID = -1;
	void*	Effect = nullptr;
	FVector	Position;
	FRotator	Rotation;
	FVector	Scale;

	EffekseerUpdateData_CommandType	Type;
};

class EffekseerUpdateData
{
public:
	TArray<EffekseerUpdateData_Command>	Commands;

	TMap<UTexture2D*, UMaterialInstanceDynamic*> OpaqueDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> TranslucentDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> AdditiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> SubtractiveDynamicMaterials;
	TMap<UTexture2D*, UMaterialInstanceDynamic*> ModulateDynamicMaterials;

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
	virtual void GetUsedMaterials(TArray<UMaterialInterface*>& OutMaterials) const override;
	virtual int32 GetNumMaterials() const override;
	virtual FBoxSphereBounds CalcBounds(const FTransform& LocalToWorld) const override;

	virtual void BeginPlay() override;

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

	UFUNCTION(BlueprintCallable, Category = "Control")
	FEffekseerHandle Play(UEffekseerEffect* effect, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectPosition(FEffekseerHandle handle, FVector position);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectRotation(FEffekseerHandle handle, FRotator rotation);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void SetEffectScaling(FEffekseerHandle handle, FVector scaling);

	UFUNCTION(BlueprintCallable, Category = "Control")
	bool Exists(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void Stop(FEffekseerHandle handle);

	UFUNCTION(BlueprintCallable, Category = "Control")
	void StopRoot(FEffekseerHandle handle);
};
