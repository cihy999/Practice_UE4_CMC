// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCharMovementCompoent.h"

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
    return uint8();
}

void UTestCharMovementCompoent::FSavedMove_Test::SetMoveFor(ACharacter* C, float InDeltaTime, FVector const& NewAccel, FNetworkPredictionData_Client_Character& ClientData)
{
}

void UTestCharMovementCompoent::FSavedMove_Test::PrepMoveFor(ACharacter* C)
{
}

UTestCharMovementCompoent::UTestCharMovementCompoent()
{
}
