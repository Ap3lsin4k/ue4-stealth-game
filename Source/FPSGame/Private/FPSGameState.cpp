// Fill out your copyright notice in the Description page of Project Settings.

#include "FPSGameState.h"
#include "FPSPlayerController.h"


// called for each machine 
void AFPSGameState::MulticastOnMissionComplete_Implementation(APawn* InstigatorPawn, bool bMissionSuccess)
{
	//  GetPawnIterator() what Pawns are available, and which are being controlled

	// loop for each other pawns on different machine
	//for (FConstPawnIterator It = GetWorld()->GetPawnIterator(); It; ++It) 
	//{
	//	APawn* Pawn = It->Get();
	//	// find a match (my machine and my pawn)
	//	if (Pawn && Pawn->IsLocallyControlled())
	//	{
	//		Pawn->DisableInput(nullptr);
	//	}
	//}

	// to call on mission complete 
	for (FConstPlayerControllerIterator It= GetWorld()->GetPlayerControllerIterator(); It; ++It)
	{
		AFPSPlayerController* PC = Cast<AFPSPlayerController>(It->Get());
		UE_LOG(LogTemp, Log, TEXT("GS.MulticastOnMissionComplete for:%s, ROLE_Authority: %s, IsLocalController: %s"), *AActor::GetDebugName(PC),
			Role == ROLE_Authority ? TEXT("true") : TEXT("false"),
			PC->IsLocalController() ? TEXT("true") : TEXT("false")
		);
		// 4 out of 7 for 4 clients(one is the host). Not dedicated
		if (PC && PC->IsLocalController())
		{
			// call mission complete even if PC doesn't possess a Pawn
			PC->OnMissionCompleted(InstigatorPawn, bMissionSuccess);

			APawn* Pawn = PC->GetPawn();
			if (Pawn)
			{
				Pawn->DisableInput(PC);
			}
		}
	}
}
