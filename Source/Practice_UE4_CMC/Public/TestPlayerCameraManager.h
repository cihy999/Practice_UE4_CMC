// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "TestPlayerCameraManager.generated.h"

/**
 * 自訂角色攝影機管理
 */
UCLASS()
class PRACTICE_UE4_CMC_API ATestPlayerCameraManager : public APlayerCameraManager
{
	GENERATED_BODY()

public:
	ATestPlayerCameraManager();
	
protected:
	UPROPERTY(EditDefaultsOnly)
	float CrouchBlendDuration = 0.5f;

	float CrouchBlendTime = 0;

	virtual void UpdateViewTarget(FTViewTarget& OutVT, float DeltaTime) override;
};
