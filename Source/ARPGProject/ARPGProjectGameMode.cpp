// Copyright Epic Games, Inc. All Rights Reserved.

#include "ARPGProjectGameMode.h"
#include "ARPGProjectCharacter.h"
#include "UObject/ConstructorHelpers.h"

AARPGProjectGameMode::AARPGProjectGameMode()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnBPClass(TEXT("/Game/ThirdPersonCPP/Blueprints/ThirdPersonCharacter"));
	if (PlayerPawnBPClass.Class != NULL)
	{
		DefaultPawnClass = PlayerPawnBPClass.Class;
	}
}
