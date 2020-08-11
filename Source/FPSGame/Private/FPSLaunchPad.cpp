// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSLaunchPad.h"
#include "Components/BoxComponent.h"
#include "FPSCharacter.h"
#include "Components/DecalComponent.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AFPSLaunchPad::AFPSLaunchPad()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	OverlapComp = CreateDefaultSubobject<UBoxComponent>(TEXT("OverlapComp"));
	OverlapComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	OverlapComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	OverlapComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap);
	OverlapComp->SetCollisionResponseToChannel(ECC_PhysicsBody, ECR_Overlap); // to overlap boxes
	OverlapComp->SetBoxExtent(FVector(75, 75, 50));
	RootComponent = OverlapComp;

	OverlapComp->SetHiddenInGame(false);

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunchPad);

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	MeshComp->SetupAttachment(RootComponent);

	ForwardVelocity = 600;
	VelocityZ = 2000;
}

// Called when the game starts or when spawned
void AFPSLaunchPad::BeginPlay()
{
	Super::BeginPlay();

	OverlapComp->OnComponentBeginOverlap.AddDynamic(this, &AFPSLaunchPad::OverlapLaunchPad);
}

void AFPSLaunchPad::OverlapLaunchPad(UPrimitiveComponent* OverlappedComponent, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
	int32 OtherBodyIndex, bool bFromSweep, const FHitResult & SweepResult)
{
	UE_LOG(LogTemp, Log, TEXT("Overlapped with Launch Pad: %s, OtherComp: %s"), *AActor::GetDebugName(OtherActor), *OtherComp->GetName());
	
	float LaunchPadYaw = FMath::DegreesToRadians(GetActorRotation().Yaw);

	if (OtherActor && OtherComp->IsSimulatingPhysics())
	{
		OtherComp->AddImpulse(800.0f*FVector(ForwardVelocity*FMath::Cos(LaunchPadYaw), 
			ForwardVelocity*FMath::Sin(LaunchPadYaw), 
			VelocityZ),
			NAME_None, false
		);	

		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadFX, GetActorLocation());


		UE_LOG(LogTemp, Log, TEXT("Launching an AFPSCharacter, NewVelocityX:%f, NewVelocityY:%f, NewVelocityZ:%f"),
			ForwardVelocity * FMath::Cos(LaunchPadYaw), ForwardVelocity * FMath::Sin(LaunchPadYaw), VelocityZ);
	}

	AFPSCharacter* MyPawn = Cast<AFPSCharacter>(OtherActor);

	if (MyPawn)
	{
		MyPawn->LaunchCharacter(FVector(ForwardVelocity*FMath::Cos(LaunchPadYaw), ForwardVelocity*FMath::Sin(LaunchPadYaw), VelocityZ), 
			true, true);
		
		UGameplayStatics::SpawnEmitterAtLocation(GetWorld(), ActivateLaunchPadFX, GetActorLocation());
		
		UE_LOG(LogTemp, Log, TEXT("Launching an AFPSCharacter, NewVelocityX:%f, NewVelocityY:%f, NewVelocityZ:%f"),
			ForwardVelocity * FMath::Cos(LaunchPadYaw), ForwardVelocity * FMath::Sin(LaunchPadYaw), VelocityZ);

	}
	UE_LOG(LogTemp, Log, TEXT("LaunchPad's Yaw(degrees): %f"), FMath::RadiansToDegrees(LaunchPadYaw));

}


