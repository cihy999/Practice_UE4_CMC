// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "TestCharMovementCompoent.generated.h"

// 自定義移動模式
UENUM(BlueprintType)
enum ECustomMovementMode
{
	CMOVE_None		UMETA(Hidden),
	CMOVE_Slide		UMETA(DisplayName = "Slide"),
	CMOVE_MAX		UMETA(Hidden),
};

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

		// Flag
		uint8 Saved_bWantsToSprint : 1;

		uint8 Saved_bPrevWantsToCrouch: 1;

	public:
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

	// 移動記錄傳輸管理
	class FNetworkPredictionData_Client_Test : public FNetworkPredictionData_Client_Character
	{
	public:
		FNetworkPredictionData_Client_Test(const UCharacterMovementComponent& ClientMovement);

		typedef FNetworkPredictionData_Client_Character Super;

		/** Allocate a new saved move. Subclasses should override this if they want to use a custom move class. */
		virtual FSavedMovePtr AllocateNewMove() override;
	};

public:
	UTestCharMovementCompoent();

	/** Get prediction data for a client game. Should not be used if not running as a client. Allocates the data on demand and can be overridden to allocate a custom override if desired. Result must be a FNetworkPredictionData_Client_Character. */
	virtual FNetworkPredictionData_Client* GetPredictionData_Client() const override;

	virtual bool IsMovingOnGround() const override;
	/** Returns true if the character is allowed to crouch in the current state. By default it is allowed when walking or falling, if CanEverCrouch() is true. */
	virtual bool CanCrouchInCurrentState() const override;

protected:
	/**
	 * Initializes the component.  Occurs at level startup or actor spawn. This is before BeginPlay (Actor or Component).
	 * All Components in the level will be Initialized on load before any Actor/Component gets BeginPlay
	 * Requires component to be registered, and bWantsInitializeComponent to be true.
	 */
	virtual void InitializeComponent() override;

	/** Unpack compressed flags from a saved move and set state accordingly. See FSavedMove_Character. */
	virtual void UpdateFromCompressedFlags(uint8 Flags) override;
	
	/// <summary>
	/// 在Perform Move最後呼叫，主要更新角色移動狀態(位置、旋轉、轉換模式...)
	/// </summary>
	virtual void OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity) override;

	/** Update the character state in PerformMovement right before doing the actual position change */
	virtual void UpdateCharacterStateBeforeMovement(float DeltaSeconds) override;

	/// <summary>
	/// 專門提供給自訂移動使用(從這進入移動位置計算)
	/// </summary>
	/// <param name="deltaTime"></param>
	/// <param name="Iterations"></param>
	virtual void PhysCustom(float deltaTime, int32 Iterations) override;

public:
	UFUNCTION(BlueprintCallable)
	void SprintPressed();
	UFUNCTION(BlueprintCallable)
	void SprintReleased();

	UFUNCTION(BlueprintCallable)
	void CrouchPressed();

	UFUNCTION(BlueprintPure)
	bool IsCustomMovementMode(ECustomMovementMode InCustomMovementMode) const;

private:
	void EnterSlide();
	void ExitSlide();
	void PhysSlide(float deltaTime, int32 Iterations);
	bool GetSlideSurface(FHitResult& Hit) const;

public:
	UPROPERTY(EditDefaultsOnly) float Sprint_MaxWalkSpeed;
	UPROPERTY(EditDefaultsOnly) float Walk_MaxWalkSpeed;

	UPROPERTY(EditDefaultsOnly) float Slide_MinSpeed = 350.f;
	UPROPERTY(EditDefaultsOnly) float Slide_EnterImpulse = 500.f;
	UPROPERTY(EditDefaultsOnly) float Slide_GravityForce = 5000.f;
	UPROPERTY(EditDefaultsOnly) float Slide_Friction = 1.3f;

	UPROPERTY(Transient) class APractice_UE4_CMCCharacter* TestCharacterOwner;

	bool Safe_bWantsToSprint;
	bool Safe_bPrevWantsToCrouch;	// 紀錄上次是否按蹲下, 第二次就會進滑行判定
};
