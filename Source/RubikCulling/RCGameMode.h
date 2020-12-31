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

protected:

	void StartPlay() override;

private:

	UPROPERTY()
	class UGrid3D* Grid3D = nullptr; 
};
