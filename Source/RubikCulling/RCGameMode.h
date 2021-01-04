// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "RCGameMode.generated.h"

/**
 * 
 */
UCLASS()
class RUBIKCULLING_API ARCGameMode : public AGameModeBase
{
	GENERATED_BODY()
	
	public:

	class AGrid3D* GetGrid3D(){ return Grid3D; };

protected:

	void StartPlay() override;

private:

	UPROPERTY()
	class AGrid3D* Grid3D = nullptr; 
};
