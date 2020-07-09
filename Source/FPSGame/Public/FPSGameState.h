// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameStateBase.h"
#include "FPSGameState.generated.h"

/**
 * exist on each machine
 */
UCLASS()
class FPSGAME_API AFPSGameState : public AGameStateBase
{
	GENERATED_BODY()

public:

	// called on all the clients (including the server)
	UFUNCTION(NetMulticast, Reliable)
	void MulticastOnMissionComplete(APawn* InstigatorPawn, bool bMissionSuccess);
	
	
};
