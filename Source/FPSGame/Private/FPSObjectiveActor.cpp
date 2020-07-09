// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSObjectiveActor.h"
#include "Components/SphereComponent.h"
#include "Kismet/GameplayStatics.h"
#include "FPSCharacter.h"

// Sets default values
AFPSObjectiveActor::AFPSObjectiveActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

	MeshComp = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("MeshComp"));
	MeshComp->SetCollisionEnabled(ECollisionEnabled::NoCollision);
	RootComponent = MeshComp;

	SphereComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	SphereComp->SetCollisionEnabled(ECollisionEnabled::QueryOnly);
	SphereComp->SetCollisionResponseToAllChannels(ECR_Ignore);
	SphereComp->SetCollisionResponseToChannel(ECC_Pawn, ECR_Overlap); // our player channel
	SphereComp->SetupAttachment(MeshComp);

	SetReplicates(true);
}

// Called when the game starts or when spawned
void AFPSObjectiveActor::BeginPlay()
{
	Super::BeginPlay();
	
	PlayEffects();
}

void AFPSObjectiveActor::PlayEffects()
{
	UGameplayStatics::SpawnEmitterAtLocation(this, PickupFX, GetActorLocation());
}


void AFPSObjectiveActor::NotifyActorBeginOverlap(AActor* OtherActor)
{
	Super::NotifyActorBeginOverlap(OtherActor);

	//works for C and S
	PlayEffects();
	/*
	When C pickup, it destroys for S but stays for C
	When S pickup, it destroys on S but stays for C

	SetReplicates(true)
	When C pickup, it destroys for S. C has no authority to destroy but it replicate from server
	When S pickups it destroys for S. C has no authority to destroy but it replicate from server
	*/
	/*
	SetReplicates(false) // server won't notify C if S destroys something
	
	When _C_0
1	NotifyObjectiveBeginOverlap for:BP_Player_C_0, ROLE_Authority: false (Client 1) <= First Person
		has no authority to call Destroy

2	NotifyObjectiveBeginOverlap for:BP_Player_C_1, ROLE_Authority: true (Server)
	Destroy()

	When _S pickups
1	NotifyObjectiveBeginOverlap for:BP_Player_C_1, ROLE_Authority: false
		has no authority to call Destroy

2	NotifyObjectiveBeginOverlap for:BP_Player_C_0, ROLE_Authority: true (Server) <= first person
	Destroy()
*/

	/*
	When C pickup 
	*/
	// we only want to run Destroy on the server because Objective Replicates(to clients): true
	if (Role == ROLE_Authority)
	{

		AFPSCharacter* MyCharacter = Cast<AFPSCharacter>(OtherActor);

		// whether cast succeeded, i.e. type of OtherActor is either AFPSCharacter or its children
		if (MyCharacter)
		{
			MyCharacter->bIsCarryingObjective = true;

			Destroy(); /* You should always Spawn and Destroy a replicated Actor on the server.A client has no authority to destroy a server replicated Actor.It would be easy to cheat as a client if you could just Destroy any Actor the Server attempts to replicate.*/
		}

	}
	/*
	Client 1 and Server SetReplicates(true)
	_C pickups
	1) NotifyObjectiveBeginOverlap for:BP_Player_C_0, ROLE_Authority: false (Client 1) <= First Person
	NotifyObjectiveBeginOverlap for:BP_Player_C_1, ROLE_Authority: true (Server)

	_S pickups
	NotifyObjectiveBeginOverlap for:BP_Player_C_1, ROLE_Authority: false
	NotifyObjectiveBeginOverlap for:BP_Player_C_0, ROLE_Authority: true (Server) <= first person
	*/

	//GEngine->AddOnScreenDebugMessage(-1, 5.f, FColor::Blue,
	//	FString::Printf(
	//		TEXT("NotifyObjectiveBeginOverlap for:%s"), *AActor::GetDebugName(OtherActor)
	//	)
	//);	
	UE_LOG(LogTemp, Log, TEXT("NotifyObjectiveBeginOverlap for:%s, ROLE_Authority: %s"), *AActor::GetDebugName(OtherActor),
		Role == ROLE_Authority ? TEXT("true") : TEXT("false")
		);
}

