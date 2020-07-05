// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BlackHole.generated.h"

class USphereComponent;

UCLASS()
class FPSGAME_API ABlackHole : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABlackHole();

protected:

	UPROPERTY(VisibleAnywhere, Category="Components")
	UStaticMeshComponent* BlackMeshComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* AttractorSphereComp;

	UPROPERTY(VisibleAnywhere, Category = "Components")
	USphereComponent* DestroyerSphereComp;
	

	UFUNCTION()
	void OverlapDestroyerSphereComp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

	
	
};
