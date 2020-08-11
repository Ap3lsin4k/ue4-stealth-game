// Fill out your copyright notice in the Description page of Project Settings.

#include "AIGuard.h"
#include "Perception/PawnSensingComponent.h"
#include "DrawDebugHelpers.h"
#include "FPSGameMode.h"
#include "Net/UnrealNetwork.h"

// Sets default values
AAIGuard::AAIGuard()
{
 	// Set this character to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	// it is not a scene component, therefore it cannot be attached
	PawnSensingComp = CreateDefaultSubobject<UPawnSensingComponent>(TEXT("PawnSensingComp"));

	GuardState = EAIState::Idle;
}

// Called when the game starts or when spawned
// to setup basic logic, a timer, or change some properties now that its fully initialized
void AAIGuard::BeginPlay()
{
	Super::BeginPlay();

	PawnSensingComp->OnSeePawn.AddDynamic(this, &AAIGuard::OnPawnSeen);
	PawnSensingComp->OnHearNoise.AddDynamic(this, &AAIGuard::OnPawnHeard);

	OriginalRotation = GetActorRotation();
}

void AAIGuard::OnPawnSeen(APawn* SeenPawn)
{
	if (SeenPawn == nullptr)
		return;

	// if mode is multiplayer GameMode exists on the server
//    if mode is singleplayer GameMode is accessible from any Actor class
	AFPSGameMode* GM = Cast<AFPSGameMode>(GetWorld()->GetAuthGameMode());
	if (GM)
	{
		GM->CompleteMission(SeenPawn, false);
	}

	DrawDebugSphere(GetWorld(), SeenPawn->GetActorLocation(), 64.f, 12, FColor::Red, false, 10.0f);

	SetGuardState(EAIState::Alerted);
}

void AAIGuard::OnPawnHeard(APawn* Instigator, const FVector& Location, float Volume)
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	DrawDebugSphere(GetWorld(), Location, 32.f, 12, FColor::Green, false, 10.0f);

	FVector Direction = Location - GetActorLocation();
	// accuracy (?)
	Direction.Normalize();

	// X is a direction vector
	// location is a unit vector
	FRotator NewLookAt = FRotationMatrix::MakeFromX(Direction).Rotator();

	NewLookAt.Pitch = 0.0f;
	NewLookAt.Roll = 0.0f;
	
	SetActorRotation(NewLookAt);

	GetWorldTimerManager().ClearTimer(TimerHandle_ResetOrientation);
	GetWorldTimerManager().SetTimer(TimerHandle_ResetOrientation, this, &AAIGuard::ResetOrientation, 3.0f);


	SetGuardState(EAIState::Suspicious);
	
}

// Called after 3.f seconds when SetTimer
void AAIGuard::ResetOrientation()
{
	if (GuardState == EAIState::Alerted)
	{
		return;
	}

	SetActorRotation(OriginalRotation);

	SetGuardState(EAIState::Idle);
}

void AAIGuard::OnRep_GuardState()
{
	OnStateChanged(GuardState);


}

void AAIGuard::SetGuardState(EAIState NewState)
{
	if (GuardState==NewState)
	{
		return;
	}

	GuardState = NewState;
	// is not called for the server by default
	OnRep_GuardState();
}

// Called every frame
void AAIGuard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}


// Synchronize for ALL the clients not a few of them
void AAIGuard::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// aply default rule to replicate this variable. Applies to all the Clients
	DOREPLIFETIME(AAIGuard, GuardState);
}

