// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "FPSLaunchPad.generated.h"

class UBoxComponent;

UCLASS()
class FPSGAME_API AFPSLaunchPad : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AFPSLaunchPad();

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	UBoxComponent* OverlapComp;
	
	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* MeshComp;

	UPROPERTY(EditAnywhere, Category="LaunchPad", meta=(ClampMin=100, ClampMax=4000))
	float ForwardVelocity;

	UPROPERTY(EditAnywhere, Category = "LaunchPad", meta = (ClampMin = 1000, ClampMax = 3000))
	float VelocityZ;

	UPROPERTY(EditDefaultsOnly, Category="LaunchPad")
	UParticleSystem* ActivateLaunchPadFX;

	UFUNCTION()
	void OverlapLaunchPad(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;
	
	
};
