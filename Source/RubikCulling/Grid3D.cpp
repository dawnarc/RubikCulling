// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid3D.h"

AGrid3D::AGrid3D()
{
	
}

void AGrid3D::SetBound(const FBox& InBound)
{
	Bound = InBound;
}

void AGrid3D::SetCellSize(float InCellSize)
{
	CellSize = InCellSize;
}

void AGrid3D::HandleActorOutOfSpatialBounds(const FVector& Location3D)
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

void AGrid3D::Tick(float DeltaTime)
{
	
}
