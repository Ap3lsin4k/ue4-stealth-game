// Fill out your copyright notice in the Description page of Project Settings.

#include "BlackHole.h"
#include "Components/SphereComponent.h"



// Sets default values
ABlackHole::ABlackHole()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	BlackMeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("BlackMeshComp"));
	BlackMeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	BlackMeshComp->SetWorldScale3D(FVector(3, 3, 3));
	RootComponent = BlackMeshComp;
	

	AttractorSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("AttractorSphereComp"));
	AttractorSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	AttractorSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	AttractorSphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	AttractorSphereComp->SetSphereRadius(10000);
	AttractorSphereComp->SetupAttachment(BlackMeshComp);

	DestroyerSphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("DestroyerSphereComp"));
	DestroyerSphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	DestroyerSphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	DestroyerSphereComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap);
	DestroyerSphereComp->SetSphereRadius(50);
	DestroyerSphereComp->SetupAttachment(BlackMeshComp);

}



void ABlackHole::BeginPlay()
{
	Super::BeginPlay();
	
	//Should be in the Begin Play
	DestroyerSphereComp->OnComponentBeginOverlap.AddDynamic(this, &ABlackHole::OverlapDestroyerSphereComp);
}

FVector GetGravityAxialForce(const FVector& BlackHole, const FVector& PhysicalBody)
{
	FVector difference(BlackHole - PhysicalBody);
	float radius = difference.Size()/100;

	difference *= 10000;
	if (radius < 1)
	{
		UE_LOG(LogTemp, Error, TEXT("Should never get here. Check the DestroyerSphereComp Location. Radius: %f. Adjusting to 1..."), radius);
		radius = 1;
	}
	
	UE_LOG(LogTemp, Warning, TEXT("R: %f"), radius);
	difference= difference / radius / radius;
	//GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Emerald, FString::Printf(TEXT("Force d: %f"), difference.Size()));
	
	return difference;
}

float GetGravityRadialForce(FVector difference)
{
	float radius = difference.Size() / 100;
	
	// prevent dividing by zero.
	if (radius < 1)
	{
		UE_LOG(LogTemp, Warning, TEXT("Should never get here. Check the Transform of DestroyerSphereComp. Radius: %f. Adjusting to 1..."), radius);
		radius = 1; 
	}

	// The Newton's Law of Universal Gravitation
	//there is squared radius in orignal formula, but that was too hard to catch for the human eye
	return 	100000/radius;
}

void ABlackHole::OverlapDestroyerSphereComp(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor)
	{
		OtherActor->Destroy();
	}

}


// Called every frame
void ABlackHole::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

	TSet<UPrimitiveComponent*> PhysicalBodies;
	AttractorSphereComp->GetOverlappingComponents(PhysicalBodies);
	PhysicalBodies.Num();

	// components close to the Black Hole
	for (UPrimitiveComponent* PhysicsBody : PhysicalBodies)
	{
		if (PhysicsBody && PhysicsBody->IsSimulatingPhysics())
		{

			const float SphereRadius = AttractorSphereComp->GetScaledSphereRadius();

			// Negative value to make it pull towards the origin instead of pushing away
			const float ForceStrength = -GetGravityRadialForce(GetActorLocation() - PhysicsBody->GetComponentLocation()); 
			
			// Ignore mass
			PhysicsBody->AddRadialForce(GetActorLocation(), SphereRadius, ForceStrength,
				ERadialImpulseFalloff::RIF_Linear, true);
		
			UE_LOG(LogTemp, VeryVerbose, TEXT("Radial Force: %f"), GetGravityRadialForce(GetActorLocation()-(PhysicsBody->GetComponentLocation())));
		}
	}

	
}

