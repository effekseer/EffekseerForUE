
#pragma once

#include "Components/PrimitiveComponent.h"

#include "EffekseerEffect.h"

#include "EffekseerEmitterComponent.generated.h"

UCLASS(ClassGroup = (Effekseer), meta = (BlueprintSpawnableComponent))
class EFFEKSEER_API UEffekseerEmitterComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

private:

public:
	UEffekseerEmitterComponent();
	virtual ~UEffekseerEmitterComponent();

	virtual void BeginPlay() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	UEffekseerEffect* Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	AActor* System = nullptr;

	UFUNCTION(BlueprintCallable, Category = "Control")
	void Play();
};
