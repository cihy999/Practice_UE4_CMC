// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TestCharMovementCompoent.generated.h"

/**
 * 自定義移動元件
 */
UCLASS()
class PRACTICE_UE4_CMC_API UTestCharMovementCompoent : public UCharacterMovementComponent
{
	GENERATED_BODY()

	// 移動紀錄
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

	// 移動預測
	class FNetworkPredictionData_Client_Test : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Test(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
		virtual FSavedMovePtr AllocateNewMove() override;
	};

	bool Safe_bWantsToSprint;

public:
	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;
protected:
	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags);

public:
	UTestCharMovementCompoent() {};
};
