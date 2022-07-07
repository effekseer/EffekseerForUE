// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "GameFramework/Actor.h"

#include "EmitterActor.generated.h"

UCLASS()
class EFFEKSEERFORUE4_API AEmitterActor : public AActor
{
	GENERATED_BODY()

public:
	// Sets default values for this actor's properties
	AEmitterActor();

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	// Called every frame
	virtual void Tick(float DeltaSeconds) override;
};
