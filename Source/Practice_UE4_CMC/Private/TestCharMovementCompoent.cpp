// Fill out your copyright notice in the Description page of Project Settings.

#include "TestCharMovementCompoent.h"
#include "GameFramework/Character.h"
#include "Practice_UE4_CMCCharacter.h"
#include "Components/CapsuleComponent.h"

#pragma region SavedMove

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

#pragma endregion

#pragma region NetworkClientData

UTestCharMovementCompoent::FNetworkPredictionData_Client_Test::FNetworkPredictionData_Client_Test(const UCharacterMovementComponent& ClientMovement)
    : Super(ClientMovement)
{

}

FSavedMovePtr UTestCharMovementCompoent::FNetworkPredictionData_Client_Test::AllocateNewMove()
{
    return FSavedMovePtr(new FSavedMove_Test());
}

#pragma endregion

#pragma region CMC

UTestCharMovementCompoent::UTestCharMovementCompoent()
{
    // 要加，不然不能蹲...
    NavAgentProps.bCanCrouch = true;
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

void UTestCharMovementCompoent::InitializeComponent()
{
    Super::InitializeComponent();

    TestCharacterOwner = Cast<APractice_UE4_CMCCharacter>(GetOwner());
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

#pragma endregion

#pragma region Input

void UTestCharMovementCompoent::SprintPressed()
{
    Safe_bWantsToSprint = true;
}

void UTestCharMovementCompoent::SprintReleased()
{
    Safe_bWantsToSprint = false;
}

void UTestCharMovementCompoent::CrouchPressed()
{
    bWantsToCrouch = ~bWantsToCrouch;
}

#pragma endregion

bool UTestCharMovementCompoent::IsCustomMovementMode(ECustomMovementMode InCustomMovementMode)
{
    return MovementMode == MOVE_Custom && CustomMovementMode == InCustomMovementMode;
}

void UTestCharMovementCompoent::EnterSlide()
{
    // Crouch會改變Capsule Height, 利用它來調整高度
    bWantsToCrouch = true;
    Velocity += Velocity.GetSafeNormal2D() * Slide_EnterImpulse;
    SetMovementMode(MOVE_Custom, CMOVE_Slide);
}

void UTestCharMovementCompoent::ExitSlide()
{
    bWantsToCrouch = false;

    // 既然離開Slide的話, 那把角度調回垂直(人物站好的角度)
    FQuat newRotation = FRotationMatrix::MakeFromXZ(UpdatedComponent->GetForwardVector().GetSafeNormal2D(), FVector::UpVector).ToQuat();
    FHitResult hit;
    SafeMoveUpdatedComponent(FVector::ZeroVector, newRotation, true, hit);
    SetMovementMode(MOVE_Walking);
}

void UTestCharMovementCompoent::PhysSlide(float deltaTime, int32 Iterations)
{
    if (deltaTime < MIN_TICK_TIME)
    {
        return;
    }

    // 有使用RootMotion需要呼叫
    RestorePreAdditiveRootMotionVelocity();

    // 如果不在滑行表面 or 速度不夠就離開slide mode
    FHitResult surfaceHit;
    if (GetSlideSurface(surfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
    {
        ExitSlide();
        StartNewPhysics(deltaTime, Iterations);
        return;
    }

    // Surface Gravity
    // v += a * dt
    Velocity += Slide_GravityForce * FVector::DownVector * deltaTime;

    // Strafe
    // 加速度基本上是Input Vector(把WASD輸入變成世界座標的方式紀錄)
    if (FMath::Abs(FVector::DotProduct(Acceleration.GetSafeNormal(), UpdatedComponent->GetRightVector())) > .5f)
    {
        Acceleration = Acceleration.ProjectOnTo(UpdatedComponent->GetRightVector());
    }
    else
    {
        Acceleration = FVector::ZeroVector;
    }

    // 計算速度
    if (!HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        CalcVelocity(deltaTime, Slide_Friction, false, GetMaxBrakingDeceleration());
    }
    ApplyRootMotionToVelocity(deltaTime);

    // 處理移動
    Iterations++;
    bJustTeleported = false;

    FVector oldLocation = UpdatedComponent->GetComponentLocation();
    FQuat oldRotation = UpdatedComponent->GetComponentRotation().Quaternion();
    // 初始化並將Time設為1，大概是先定為未偵測到物體吧...
    FHitResult hit(1.f);
    // 新位移 = v * dt
    FVector adjusted = Velocity * deltaTime;
    // 將新Rotaion緊貼著滑行的平面上(隨著地形起伏旋轉)
    FVector velPlaneDir = FVector::VectorPlaneProject(Velocity, surfaceHit.Normal).GetSafeNormal();
    FQuat newRotaion = FRotationMatrix::MakeFromXZ(velPlaneDir, surfaceHit.Normal).ToQuat();
    // 更新位置(包含Location(給位移量)、Rotation、碰撞偵測，比如adjusted計算的位移已經是牆壁裡，偵測可以將實際位置變成牆壁前)
    SafeMoveUpdatedComponent(adjusted, newRotaion, true, hit);

    // 撞到東西
    if (hit.Time < 1.f)
    {
        HandleImpact(hit, deltaTime, adjusted);
        // 沿著阻擋物滑行(如果斜斜的話，就延邊移動)
        SlideAlongSurface(adjusted, 1.f - hit.Time, hit.Normal, hit, true);
    }

    // 再檢查滑行條件
    FHitResult newSurfaceHit;
    if (GetSlideSurface(newSurfaceHit) || Velocity.SizeSquared() < pow(Slide_MinSpeed, 2))
    {
        ExitSlide();
    }

    if (!bJustTeleported && !HasAnimRootMotion() && !CurrentRootMotion.HasOverrideVelocity())
    {
        // 因為SafeMoveUpdatedComponent開啟碰撞偵測，遇到阻擋物的話，實際位置與速度就不合
        // 校正速度 = (新位置 - 舊位置) / dt
        Velocity = (UpdatedComponent->GetComponentLocation() - oldLocation) / deltaTime;
    }
}

bool UTestCharMovementCompoent::GetSlideSurface(FHitResult& Hit) const
{
    FVector start = UpdatedComponent->GetComponentLocation();
    FVector end = start + CharacterOwner->GetCapsuleComponent()->GetScaledCapsuleHalfHeight() * 2.f * FVector::DownVector;
    FName profileName = TEXT("BlockAll");   // =Collision Preset
    return GetWorld()->LineTraceSingleByProfile(Hit, start, end, profileName, TestCharacterOwner->GetIgnoreCharacterParams());
}
