// Fill out your copyright notice in the Description page of Project Settings.


#include "Grid3D.h"

UGrid3D::UGrid3D()
{
	
}

void UGrid3D::SetBound(const FBox& InBound)
{
	Bound = InBound;
}

void UGrid3D::SetCellSize(float InCellSize)
{
	CellSize = InCellSize;
}

void UGrid3D::Tick(float DeltaTime)
{
	
}