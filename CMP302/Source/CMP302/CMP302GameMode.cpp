// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#include "CMP302GameMode.h"
#include "CMP302HUD.h"
#include "CMP302Character.h"
#include "UObject/ConstructorHelpers.h"

ACMP302GameMode::ACMP302GameMode()
	: Super()
{
	// set default pawn class to our Blueprinted character
	static ConstructorHelpers::FClassFinder<APawn> PlayerPawnClassFinder(TEXT("/Game/FirstPersonCPP/Blueprints/FirstPersonCharacter"));
	DefaultPawnClass = PlayerPawnClassFinder.Class;

	// use our custom HUD class
	HUDClass = ACMP302HUD::StaticClass();
}
