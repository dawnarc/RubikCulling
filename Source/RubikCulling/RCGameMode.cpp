// Fill out your copyright notice in the Description page of Project Settings.


#include "RCGameMode.h"
#include "Grid3D.h"

void ARCGameMode::StartPlay()
{
	Super::StartPlay();

	Grid3D = NewObject<UGrid3D>();
}