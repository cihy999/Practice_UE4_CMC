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

	class FSavedMove_Test : public FSavedMove_Character 
	{
		typedef FSavedMove_Character Super;

		uint8 Saved_bWantsToSprint : 1;

		/** Returns true if this move can be combined with NewMove for replication without changing any behavior */
		virtual bool CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const override;
		/** Clear saved move properties, so it can be re-used. */
		virtual void Clear() override;
		/** Returns a byte containing encoded special movement information (jumping, crouching, etc.)	 */
		virtual uint8 GetCompressedFlags() const override;
		/** Called to set up this saved move (when initially created) to make a predictive correction. */
		virtual void SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, class FNetworkPredictionData_Client_Character & ClientData) override;
		/** Called before ClientUpdatePosition uses this SavedMove to make a predictive correction	 */
		virtual void PrepMoveFor(ACharacter* C) override;
	};

	bool Safe_bWantsToSprint;

public:
	UTestCharMovementCompoent();
};
