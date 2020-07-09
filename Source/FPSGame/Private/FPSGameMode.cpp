// Copyright 1998-2017 Epic Games, Inc. All Rights Reserved.

#include "FPSGameMode.h"
#include "FPSHUD.h"
#include "FPSCharacter.h"
#include "UObject/ConstructorHelpers.h"
#include "Kismet/GameplayStatics.h"
#include "FPSGameState.h"

AFPSGameMode::AFPSGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/Blueprints/BP_Player"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = AFPSHUD::StaticClass();

	GameStateClass = AFPSGameState::StaticClass();
}

/* if C_0 triggered the Extraction Zone it runs for C_0
	 if S triggered the Extraction Zone it runs for S
	*/
// runs on server because GameMode exists only on a server
void AFPSGameMode::CompleteMission(APawn* InstigatorPawn, bool bMissionSuccess)
{
	// since GameMode exists only on the server
	// InstigatorPawn is the one who triggered the event (either C or S)
	if (InstigatorPawn)
	{
		// disable Pawn for any PlayerController (on a machine)
		//InstigatorPawn->DisableInput(nullptr);

		if (SpectatingViewpointClass) 
		{
			TArray<AActor*> ReturnedActors;

			UGameplayStatics::GetAllActorsOfClass(this, SpectatingViewpointClass, ReturnedActors);

			// Change viewpoint if CameraActor is valid
			if (ReturnedActors.Num() > 0)
			{
				AActor* NewViewTarget = ReturnedActors[0];

				for (FConstPlayerControllerIterator It = GetWorld()->GetPlayerControllerIterator(); It; ++It)
				{
					// do not cast to FPSPlayerController because we only use SetViewTargetWithBlend which is common for every Controller
					APlayerController* PC = It->Get();
					// we run it on the server thus we don't need to call IsLocalController()
					if (PC)
					{
						PC->SetViewTargetWithBlend(NewViewTarget, 1.1f, EViewTargetBlendFunction::VTBlend_Cubic);
					}
				}
				APlayerController* PC = Cast<APlayerController>(InstigatorPawn->GetController());
				
			}
		}
		else 
		{
			UE_LOG(LogTemp, Warning, TEXT("SpactatingViewpointClass is nullptr. Please update GameMode with valid subclass. Cannot change spectating view target."));
		}

	}

	// easy to get a game state in a GM
	AFPSGameState* GS = GetGameState<AFPSGameState>();

	if (GS)
	{
		GS->MulticastOnMissionComplete(InstigatorPawn, bMissionSuccess);
	}

	OnMissionCompleted(InstigatorPawn, bMissionSuccess);

	
}
