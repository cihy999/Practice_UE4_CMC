// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TestCharMovementCompoent.generated.h"

/**
 * 
 */
UCLASS()
class PRACTICE_UE4_CMC_API UTestCharMovementCompoent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	bool Safe_bWantsToSprint;

public:
	UTestCharMovementCompoent();
};
