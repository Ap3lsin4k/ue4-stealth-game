// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "FPSCharacter.h"
#include "MockFPSCharacter.generated.h"

/**
 * 
 */

//class AFPSProjectile;

UCLASS()
class FPSGAME_API AMockFPSCharacter : public AFPSCharacter
{
	GENERATED_BODY()
	
public:

	UPROPERTY(BlueprintReadWrite)
	class AFPSProjectile* lastProjectile;



	// @Override
	void Fire();
	
};
