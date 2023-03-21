// Copyright Epic Games, Inc. All Rights Reserved.

#include "Practice_UE4_CMCGameMode.h"
#include "Practice_UE4_CMCCharacter.h"
#include "UObject/ConstructorHelpers.h"

APractice_UE4_CMCGameMode::APractice_UE4_CMCGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
