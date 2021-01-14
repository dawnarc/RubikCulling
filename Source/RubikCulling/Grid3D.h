// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "DrawDebugHelpers.h"
#include "UObject/NoExportTypes.h"
#include "GameFramework/Actor.h"
#include "Grid3D.generated.h"

UCLASS()
class RUBIKCULLING_API UCellNode : public UObject
{
	GENERATED_BODY()

public:
	
	void DrawGreen(int32 X, int32 Y, int32 Z, float InCellSize, const FVector& InSpatialBias)
	{
		FVector TargetCell;
		TargetCell.X = X * InCellSize + InSpatialBias.X;
		TargetCell.Y = Y * InCellSize + InSpatialBias.Y;
		TargetCell.Z = Z * InCellSize + InSpatialBias.Z;
		DrawDebugBox(GetWorld(), TargetCell + InCellSize / 2, FVector(InCellSize, InCellSize, InCellSize),
			FColor::Green, false, 10.f, 0, 4.f);
	}

	void DrawRed(int32 X, int32 Y, int32 Z, float InCellSize, const FVector& InSpatialBias)
	{
		FVector TargetCell;
		TargetCell.X = X * InCellSize + InSpatialBias.X;
		TargetCell.Y = Y * InCellSize + InSpatialBias.Y;
		TargetCell.Z = Z * InCellSize + InSpatialBias.Z;
		DrawDebugBox(GetWorld(), TargetCell + InCellSize / 2, FVector(InCellSize, InCellSize, InCellSize),
			FColor::Red, false, 10.f, 0, 4.f);
	}
};

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

	UCellNode* GetCellNode(UCellNode*& NodePtr)
	{
		if (NodePtr == nullptr)
		{
			UCellNode* NewNode = NewObject<UCellNode>(this);
			AllChildNodes.Add(NewNode);
			
			NodePtr = NewNode;
		}

		return NodePtr;
	}

	TArray<TArray<UCellNode*>>& GetGridX(int32 X);

	UCellNode*& GetCell(TArray<UCellNode*>& GridY, int32 Z);

	TArray<UCellNode*>& GetGridY(TArray<TArray<UCellNode*>>& GridX, int32 Y);

	FCellInfo GetCellInfoForActor(const FVector& TargetLocation, float InCullDistance, float InCellSize, const FVector& InSpatialBias);

	void DrawDebugCellInfo(const FVector& TargetLocation, float InCullDistance, float InCellSize, const FVector& InSpatialBias);

	void Rebuild(const FVector& TargetLocation);
	
	const FVector& GetSpatialBias() { return SpatialBias; }

	int32 GetCellSize() { return CellSize; }

protected:

	virtual void Tick(float DeltaTime) override;
	/*virtual bool IsTickable() const override { return GIsRunning; }
	virtual TStatId GetStatId() const override { return GetStatID(); }*/

private:

	FBox Bound;
	
	FVector SpatialBias;
	
	int32 CellSize = 400;

	int32 CullDistance = 500;
	
	FCellInfo PrevCellInfo;

	TArray< TArray< TArray<UCellNode*> > > Cells;

	UPROPERTY()
	TArray< UCellNode* > AllChildNodes;
};
