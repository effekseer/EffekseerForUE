// Fill out your copyright notice in the Description page of Project Settings.

#include "EmitterActor.h"

#include "EffekseerForUE4.h"

// Sets default values
AEmitterActor::AEmitterActor()
{
	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
}

// Called when the game starts or when spawned
void AEmitterActor::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AEmitterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}
