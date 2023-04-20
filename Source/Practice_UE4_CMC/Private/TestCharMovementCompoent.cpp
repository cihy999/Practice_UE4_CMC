// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCharMovementCompoent.h"
#include "GameFramework/Character.h"

bool UTestCharMovementCompoent::FSavedMove_Test::CanCombineWith(const FSavedMovePtr& NewMove, ACharacter* InCharacter, float MaxDelta) const
{
    // 如果資料相同，不需要送上server(=true)

    FSavedMove_Test* pNewTestMove = static_cast<FSavedMove_Test*>(NewMove.Get());
    if (Saved_bWantsToSprint != pNewTestMove->Saved_bWantsToSprint)
    {
        return false;
    }

    return Super::CanCombineWith(NewMove, InCharacter, MaxDelta);
}

void UTestCharMovementCompoent::FSavedMove_Test::Clear()
{
    Super::Clear();

    Saved_bWantsToSprint = 0;
}

uint8 UTestCharMovementCompoent::FSavedMove_Test::GetCompressedFlags() const
{
    // 辨識移動狀態

    uint8 Result = Super::GetCompressedFlags();

    if (Saved_bWantsToSprint) Result |= FLAG_Custom_0;

    return Result;
}

void UTestCharMovementCompoent::FSavedMove_Test::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
    // 紀錄角色移動相關資料，以便送給server

    Super::SetMoveFor(C, InDeltaTime, NewAccel, ClientData);

    UTestCharMovementCompoent* CharacterMovement = Cast<UTestCharMovementCompoent>(C->GetCharacterMovement());

    Saved_bWantsToSprint = CharacterMovement->Safe_bWantsToSprint;
}

void UTestCharMovementCompoent::FSavedMove_Test::PrepMoveFor(ACharacter* C)
{
    // 從Server端校正移動

    Super::PrepMoveFor(C);

    UTestCharMovementCompoent* CharacterMovement = Cast<UTestCharMovementCompoent>(C->GetCharacterMovement());

    CharacterMovement->Safe_bWantsToSprint = Saved_bWantsToSprint;
}

UTestCharMovementCompoent::FNetworkPredictionData_Client_Test::FNetworkPredictionData_Client_Test(const UCharacterMovementComponent& ClientMovement) 
    : Super(ClientMovement)
{

}

FSavedMovePtr UTestCharMovementCompoent::FNetworkPredictionData_Client_Test::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_Test());
}

FNetworkPredictionData_Client* UTestCharMovementCompoent::GetPredictionData_Client() const
{
    check(PawnOwner != nullptr)

    if (ClientPredictionData == nullptr)
    {
        UTestCharMovementCompoent* MutableThis = const_cast<UTestCharMovementCompoent*>(this);
        MutableThis->ClientPredictionData = new FNetworkPredictionData_Client_Test(*this);
        MutableThis->ClientPredictionData->MaxSmoothNetUpdateDist = 92.f;
        MutableThis->ClientPredictionData->NoSmoothNetUpdateDist = 140.f;
    }

    return ClientPredictionData;
}

void UTestCharMovementCompoent::UpdateFromCompressedFlags(uint8 Flags)
{
    Super::UpdateFromCompressedFlags(Flags);

    Safe_bWantsToSprint = (Flags & FSavedMove_Character::FLAG_Custom_0) != 0;
}

void UTestCharMovementCompoent::OnMovementUpdated(float DeltaSeconds, const FVector& OldLocation, const FVector& OldVelocity)
{
    Super::OnMovementUpdated(DeltaSeconds, OldLocation, OldVelocity);

    if (MovementMode == EMovementMode::MOVE_Walking)
    {
        if (Safe_bWantsToSprint)
        {
            MaxWalkSpeed = Sprint_MaxWalkSpeed;
        }
        else
        {
            MaxWalkSpeed = Walk_MaxWalkSpeed;
        }
    }
}

void UTestCharMovementCompoent::SprintPressed()
{
    Safe_bWantsToSprint = true;
}

void UTestCharMovementCompoent::SprintReleased()
{
    Safe_bWantsToSprint = false;
}
