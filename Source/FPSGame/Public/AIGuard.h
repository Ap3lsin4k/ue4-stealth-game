// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "AIGuard.generated.h"

class UPawnSensingComponent;


// `uint8` to expose to BP
UENUM(BlueprintType)
enum class EAIState : uint8 
{
	Idle,

	Suspicious,

	Alerted
};


UCLASS()
class FPSGAME_API AAIGuard : public ACharacter
{
	GENERATED_BODY()

public:
	// Sets default values for this character's properties
	AAIGuard();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;



	UPROPERTY(VisibleDefaultsOnly, Category="Components")
	UPawnSensingComponent* PawnSensingComp;

	UFUNCTION()
	void OnPawnSeen(APawn* SeenPawn);

	UFUNCTION()
	void OnPawnHeard(APawn* NoiseInstigator, const FVector& Location, float Volume);

	UPROPERTY(ReplicatedUsing=OnRep_GuardState)
	EAIState GuardState;

	// gets called whenever GuardState changes (only on clients)
	UFUNCTION()
	void OnRep_GuardState();

	// to set through BP
	void SetGuardState(EAIState NewState);

	UFUNCTION(BlueprintImplementableEvent, Category = "AI")
	void OnStateChanged(EAIState NewState);

private:

	FRotator OriginalRotation;

	UFUNCTION()	
	void ResetOrientation();

	// make global, so only a timer exists at a time
	FTimerHandle TimerHandle_ResetOrientation;



public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;
	
	
};
