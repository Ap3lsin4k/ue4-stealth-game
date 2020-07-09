// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSCharacter.h"
#include "FPSProjectile.h"
#include "Animation/AnimInstance.h"
#include "Camera/CameraComponent.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "UObject/ObjectMacros.h"
#include "Math/Rotator.h"
#include "Components/PawnNoiseEmitterComponent.h"
#include "Net/UnrealNetwork.h"

AFPSCharacter::AFPSCharacter()
{
	// Create a CameraComponent	
	CameraComponent = CreateDefaultSubobject<UCameraComponent>(TEXT("FirstPersonCamera"));
	CameraComponent->SetupAttachment(GetCapsuleComponent());
	CameraComponent->RelativeLocation = FVector(0, 0, BaseEyeHeight); // Position the camera
	CameraComponent->bUsePawnControlRotation = true;

	// Create a mesh component that will be used when being viewed from a '1st person' view (when controlling this pawn)
	Mesh1PComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("CharacterMesh"));
	Mesh1PComponent->SetupAttachment(CameraComponent);
	Mesh1PComponent->CastShadow = false;
	Mesh1PComponent->RelativeRotation = FRotator(2.0f, -15.0f, 5.0f);
	Mesh1PComponent->RelativeLocation = FVector(0, 0, -160.0f);

	// Create a gun mesh component
	GunMeshComponent = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("FP_Gun"));
	GunMeshComponent->CastShadow = false;
	GunMeshComponent->SetupAttachment(Mesh1PComponent, "GripPoint");

	NoiseEmitterComponent = CreateDefaultSubobject<UPawnNoiseEmitterComponent>(TEXT("NoiseEmitter"));
}


void AFPSCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	// set up gameplay key bindings
	check(PlayerInputComponent);

	PlayerInputComponent->BindAction("Jump", IE_Pressed, this, &ACharacter::Jump);
	PlayerInputComponent->BindAction("Fire", IE_Pressed, this, &AFPSCharacter::Fire);

	PlayerInputComponent->BindAxis("MoveForward", this, &AFPSCharacter::MoveForward);
	PlayerInputComponent->BindAxis("MoveRight", this, &AFPSCharacter::MoveRight);

	PlayerInputComponent->BindAxis("Turn", this, &APawn::AddControllerYawInput);
	PlayerInputComponent->BindAxis("LookUp", this, &APawn::AddControllerPitchInput);
}


void AFPSCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
	/*
	GEngine->AddOnScreenDebugMessage(-1, 0.f, FColor::Emerald, 
		FString::Printf(
			TEXT("Tick for:%s, isLocalyControlled: %s"), *AActor::GetDebugName(this), IsLocallyControlled() ? TEXT("true") : TEXT("false")
		)
	);*/
	// C doesn't see pitch for server 
	// S doesn't see pitch for C
	// When C shoots projectile S simulates it with Pitch =0
	// When S shoots projectile properly and C sees it moves up

	if (!IsLocallyControlled()) 
	{
		FRotator NewRot = CameraComponent->RelativeRotation;
		// Update not replicated variable with replicated
		// don't need to update Yaw and Roll because it is already replicated
		// reconvert from utf8
		NewRot.Pitch = RemoteViewPitch*360.f /255.f;
		
		CameraComponent->SetRelativeRotation(NewRot);
	}
	/*
	Tick in _S for _S - ignored
	Tick in _C_0 for S - update Pitch (although projectile shoots)
	Tick in _S for _C_0 - update Pitch (client can shoot projectile with different angle)
	Tick in _C_0 for _C_0 - ignored
	*/
}

void AFPSCharacter::Fire()
{
	ServerFire();

	// keep local
	// try and play the sound if specified
	if (FireSound)
	{
		UGameplayStatics::PlaySoundAtLocation(this, FireSound, GetActorLocation());
	}

	// try and play a firing animation if specified
	if (FireAnimation)
	{
		// Get the animation object for the arms mesh
		UAnimInstance* AnimInstance = Mesh1PComponent->GetAnimInstance();
		if (AnimInstance)
		{
			AnimInstance->PlaySlotAnimationAsDynamicMontage(FireAnimation, "Arms", 0.0f);
		}
	}
}

void AFPSCharacter::ServerFire_Implementation()
{
	// try and fire a projectile
	if (ProjectileClass)
	{
		FVector MuzzleLocation = GunMeshComponent->GetSocketLocation("Muzzle");
		FRotator MuzzleRotation = GunMeshComponent->GetSocketRotation("Muzzle");

		//Set Spawn Collision Handling Override
		FActorSpawnParameters ActorSpawnParams;
		ActorSpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButDontSpawnIfColliding;
		ActorSpawnParams.Instigator = this; // doesn't work without it
		// spawn the projectile at the muzzle
		GetWorld()->SpawnActor<AFPSProjectile>(ProjectileClass, MuzzleLocation, MuzzleRotation, ActorSpawnParams);

		MakeNoise(0.2f, Instigator); // shooting noise
	}

}

bool AFPSCharacter::ServerFire_Validate()
{
	return true;
}


void AFPSCharacter::MoveForward(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorForwardVector(), Value);
	}
}


void AFPSCharacter::MoveRight(float Value)
{
	if (Value != 0.0f)
	{
		// add movement in that direction
		AddMovementInput(GetActorRightVector(), Value);
	}
}

void AFPSCharacter::GetLifetimeReplicatedProps(TArray< FLifetimeProperty > & OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	/* whenever bIsCarryingObjective is set, all the clients receive the var too
	therefore bIsCarryingObjective will be common for everyone. C_0 can pickup an objective and C_1 can complete the mission
	Like those guys are in the same team.
	*/
	DOREPLIFETIME(AFPSCharacter, bIsCarryingObjective);

	// only gets send to whoever is controlling this character. We just use it for local HUD, so other guys should not be notified that we started carrying it
	//DOREPLIFETIME_CONDITION(AFPSCharacter, bIsCarryingObjective, COND_OwnerOnly);
}
