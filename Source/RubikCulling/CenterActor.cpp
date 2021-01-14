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
		Grid3D->HandleActorOutOfSpatialBounds(GetActorLocation());

		//Grid3D->Rebuild();
		
		//DrawDebugCellInfo(CellSize, SpatialBias);
		Grid3D->DrawDebugCellInfo(GetActorLocation(), CullDistance, Grid3D->GetCellSize(), Grid3D->GetSpatialBias());
	}
}
