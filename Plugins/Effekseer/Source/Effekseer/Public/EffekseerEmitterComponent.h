
#pragma once

#include "Components/PrimitiveComponent.h"

#include "EffekseerEffect.h"
#include "EffekseerHandle.h"

#include "EffekseerEmitterComponent.generated.h"

UCLASS(ClassGroup = (Effekseer), meta = (BlueprintSpawnableComponent))
class EFFEKSEER_API UEffekseerEmitterComponent : public UPrimitiveComponent
{
	GENERATED_BODY()

private:
	bool		shouldActivate = false;
	bool		isPlaying = false;
	FEffekseerHandle		handle;

	FColor AllColor_ = FColor(255, 255, 255, 255);
	float Speed_ = 1.0f;

public:
	UEffekseerEmitterComponent();
	virtual ~UEffekseerEmitterComponent();

#if WITH_EDITOR
	virtual void PostEditChangeProperty(struct FPropertyChangedEvent& PropertyChangedEvent) override;
#endif

	virtual void BeginPlay() override;

	virtual void BeginDestroy() override;

	virtual void TickComponent(float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction) override;

	virtual void Activate(bool bReset = false) override;

	virtual void Deactivate() override;

	virtual void OnUnregister() override;

	UPROPERTY()
	uint8 bAutoDestroy : 1;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	UEffekseerEffect* Effect = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Effect)
	AActor* System = nullptr;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	bool IsLooping = false;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	FColor AllColor = FColor(255, 255, 255, 255);

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = Property)
	float Speed = 1.0f;

	UPROPERTY(Transient)
	UEffekseerSystemComponent* system_ = nullptr;

	UFUNCTION(BlueprintCallable, Category = Effect)
	void Stop();

	UFUNCTION(BlueprintCallable, Category = Effect)
	void StopRoot();

	UFUNCTION(BlueprintCallable, Category = Effect)
	bool Exists() const;

	UFUNCTION(BlueprintCallable, Category = Deprecated)
	FEffekseerHandle Play();
};
