// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGameMode.h"
#include "Grid3D.h"

void ARCGameMode::StartPlay()
{
	Super::StartPlay();

	Grid3D = NewObject<AGrid3D>();
	FBox Bound(FVector(-HALF_WORLD_MAX, -HALF_WORLD_MAX, -HALF_WORLD_MAX), FVector(HALF_WORLD_MAX, HALF_WORLD_MAX, HALF_WORLD_MAX));
	Grid3D->SetBound(Bound);
	Grid3D->SetCellSize(1000);
}