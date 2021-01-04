// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "Grid3D.generated.h"


struct FCellInfo
{
	bool IsValid() const { return StartX != -1; }
	void Reset() { StartX = -1; }
	int32 StartX=-1;
	int32 StartY;
	int32 StartZ;
	int32 EndX;
	int32 EndY;
	int32 EndZ;
};

/**
 * 
 */
UCLASS()
class RUBIKCULLING_API AGrid3D : public AActor
{
	GENERATED_BODY()

public:

	AGrid3D();

	void SetBound(const FBox& InBound);

	void SetCellSize(float InCellSize);

	void HandleActorOutOfSpatialBounds(const FVector& Location3D);
	
	const FVector& GetSpatialBias() { return SpatialBias; }

	int32 GetCellSize() { return CellSize; }

protected:

	virtual void Tick(float DeltaTime) override;
	/*virtual bool IsTickable() const override { return GIsRunning; }
	virtual TStatId GetStatId() const override { return GetStatID(); }*/

private:

	FBox Bound;
	
	FVector SpatialBias;
	
	int32 CellSize = 200;
};
