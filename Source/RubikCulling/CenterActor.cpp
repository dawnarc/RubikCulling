// Fill out your copyright notice in the Description page of Project Settings.


#include "CenterActor.h"



#include "DrawDebugHelpers.h"
#include "RCGameMode.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
ACenterActor::ACenterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	PrimaryActorTick.bStartWithTickEnabled = true;
}

// Called when the game starts or when spawned
void ACenterActor::BeginPlay()
{
	Super::BeginPlay();
	
	if(AGameModeBase* GameModeBase = UGameplayStatics::GetGameMode(this))
	{
		if(ARCGameMode* RCGameMode = Cast<ARCGameMode>(GameModeBase))
		{
			Grid3D = RCGameMode->GetGrid3D();
		}
	}
}

// Called every frame
void ACenterActor::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	if(Grid3D)
	{
		HandleActorOutOfSpatialBounds(GetActorLocation());
		
		/*FCellInfo NewCellInfo = GetCellInfoForActor(CullDistance, Grid3D->GetCellSize(), Grid3D->GetSpatialBias());
		PrevCellInfo = NewCellInfo;*/

		//DrawDebugCellInfo(CellSize, SpatialBias);
		DrawDebugCellInfo(Grid3D->GetCellSize(), Grid3D->GetSpatialBias());
	}
}

FCellInfo ACenterActor::GetCellInfoForActor(float InCullDistance, float InCellSize, const FVector& InSpatialBias)
{
	FVector Location3D = GetActorLocation();
	FVector ClampedLocation = Location3D;

	// Sanity check the actor's location. If it's garbage, we could end up with a gigantic allocation in GetGridNodesForActor as we adjust the grid.
	if (Location3D.X < -HALF_WORLD_MAX || Location3D.X > HALF_WORLD_MAX ||
        Location3D.Y < -HALF_WORLD_MAX || Location3D.Y > HALF_WORLD_MAX ||
        Location3D.Z < -HALF_WORLD_MAX || Location3D.Z > HALF_WORLD_MAX)
	{
		ClampedLocation = Location3D.BoundToCube(HALF_WORLD_MAX);
	}

	FCellInfo CellInfo;
	const float LocationBiasX = (ClampedLocation.X - InSpatialBias.X);
	const float LocationBiasY = (ClampedLocation.Y - InSpatialBias.Y);
	const float LocationBiasZ = (ClampedLocation.Z - InSpatialBias.Z);

	const float Dist = InCullDistance;
	const float MinX = LocationBiasX - Dist;
	const float MinY = LocationBiasY - Dist;
	const float MinZ = LocationBiasZ - Dist;
	float MaxX = LocationBiasX + Dist;
	float MaxY = LocationBiasY + Dist;
	float MaxZ = LocationBiasZ + Dist;

	CellInfo.StartX = FMath::Max<int32>(0, MinX / InCellSize);
	CellInfo.StartY = FMath::Max<int32>(0, MinY / InCellSize);
	CellInfo.StartZ = FMath::Max<int32>(0, MinZ / InCellSize);

	CellInfo.EndX = FMath::Max<int32>(0, MaxX / InCellSize);
	CellInfo.EndY = FMath::Max<int32>(0, MaxY / InCellSize);
	CellInfo.EndZ = FMath::Max<int32>(0, MaxZ / InCellSize);
	
	return CellInfo;
}

void ACenterActor::HandleActorOutOfSpatialBounds(const FVector& Location3D)
{
	if (SpatialBias.X > Location3D.X)
	{
		SpatialBias.X = Location3D.X - (CellSize / 2.f);
	}
	if (SpatialBias.Y > Location3D.Y)
	{
		SpatialBias.Y = Location3D.Y - (CellSize / 2.f);
	}
	if (SpatialBias.Z > Location3D.Z)
	{
		SpatialBias.Z = Location3D.Z - (CellSize / 2.f);
	}
}

void ACenterActor::DrawDebugCellInfo(float InCellSize, const FVector& InSpatialBias)
{
	FCellInfo NewCellInfo = GetCellInfoForActor(CullDistance, InCellSize, InSpatialBias);
	for(int32 X = NewCellInfo.StartX; X <= NewCellInfo.EndX; X++)
	{
		for(int32 Y = NewCellInfo.StartY; Y <= NewCellInfo.EndY; Y++)
		{
			for(int32 Z = NewCellInfo.StartZ; Z <= NewCellInfo.EndZ; Z++)
			{
				FVector TargetCell;
				TargetCell.X = X * InCellSize + InSpatialBias.X;
				TargetCell.Y = Y * InCellSize + InSpatialBias.Y;
				TargetCell.Z = Z * InCellSize + InSpatialBias.Z;
				DrawDebugBox(GetWorld(), TargetCell + InCellSize / 2, FVector(InCellSize, InCellSize, InCellSize), FColor::Yellow, false, 0.1, 0, 2.f);
			}
		}	
	}
}

