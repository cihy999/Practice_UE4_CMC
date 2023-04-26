// Fill out your copyright notice in the Description page of Project Settings.

#include "TestPlayerCameraManager.h"
#include "Practice_UE4_CMCCharacter.h"
#include "TestCharMovementCompoent.h"
#include "Components/CapsuleComponent.h"

ATestPlayerCameraManager::ATestPlayerCameraManager()
{}

void ATestPlayerCameraManager::UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime)
{
	Super::UpdateViewTarget(OutVT, DeltaTime);

	// 獲取角色
	if (APractice_UE4_CMCCharacter* c = Cast<APractice_UE4_CMCCharacter>(GetOwningPlayerController()->GetPawn()))
	{
		// 獲取角色移動元件
		UTestCharMovementCompoent* mc = c->GetTestCharMovement();
		if (!IsValid(mc))
		{
			return;
		}

		FVector TargetCrouchOffset = FVector(
			0, 
			0,
			mc->CrouchedHalfHeight - c->GetClass()->GetDefaultObject<ACharacter>()->GetCapsuleComponent()->GetScaledCapsuleHalfHeight());
		FVector Offset = FMath::Lerp(FVector::ZeroVector, TargetCrouchOffset, FMath::Clamp(CrouchBlendTime / CrouchBlendDuration, 0.f, 1.f));

		// 獲取角色移動狀態
		if (mc->IsCrouching())
		{
			// 蹲下
			
			// 調整Lerp Alpha可以從0→1
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime + DeltaTime, 0.f, CrouchBlendDuration);
			Offset += TargetCrouchOffset;
		}
		else
		{
			// 調整Lerp Alpha可以從1→0
			CrouchBlendTime = FMath::Clamp(CrouchBlendTime - DeltaTime, 0.f, CrouchBlendDuration);
		}

		// 空中不需要這種調法
		if (mc->IsMovingOnGround())
		{
			OutVT.POV.Location += Offset;
		}
	}
}
