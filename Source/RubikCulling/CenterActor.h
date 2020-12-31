// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CenterActor.generated.h"

UCLASS()
class RUBIKCULLING_API ACenterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACenterActor();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	virtual bool ShouldTickIfViewportsOnly() const override
	{
		return true;
	}

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

};
