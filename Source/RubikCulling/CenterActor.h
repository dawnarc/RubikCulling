// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Grid3D.h"
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
	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	FCellInfo GetCellInfoForActor(const FVector& TargetLocation, float InCullDistance, float InCellSize, const FVector& InSpatialBias);

	void HandleActorOutOfSpatialBounds(const FVector& Location3D);

	void Rebuild();
	
	void DrawDebugCellInfo(const FVector& TargetLocation, float InCellSize, const FVector& InSpatialBias);

protected:

	UPROPERTY(EditAnywhere, BlueprintReadWrite)
	class AGrid3D* Grid3D = nullptr;

private:

	float CullDistance = 500;
};
