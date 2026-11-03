#include "Camera/PlayerCameraMode.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"
#include "Actor/ClientActorManager.h"
#include "Character/PlayerCharacter.h"
#include "ActorComponent/TargetActorComponent.h"
#include "Engine/GameViewportClient.h"

APlayerCameraMode::APlayerCameraMode()
{
	__noop;
}

void APlayerCameraMode::Initialize( const FPlayerCameraPresetInfo& InPlayerCameraPresetInfo, const FPlayerCameraResult& PrevPlayerCameraResult )
{
	PlayerCameraPresetInfo = InPlayerCameraPresetInfo;

	CameraYaw = PrevPlayerCameraResult.CameraYaw;
	CameraPitch = PrevPlayerCameraResult.CameraPitch;
}

void APlayerCameraMode::UpdateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	UpdateCameraMode_Internal( PlayerCameraUpdateDesc );
	UpdateCameraMode_Common( PlayerCameraUpdateDesc );
}

void APlayerCameraMode::UpdateCameraMode_Common( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	UpdateCameraMode_RotateCamera( PlayerCameraUpdateDesc );
}

void APlayerCameraMode::UpdateCameraMode_RotateCamera( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	constexpr float YawRotateThresholdAngle = 45.0f;

	if( PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::None || PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::Count )
		return;


	if( PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::RotateYawBySpeed )
	{
		float DiffYaw = 0.0f;
		if( PlayerCameraRotateInfo.YawRotateDirection == 0 )
			DiffYaw = FMath::FindDeltaAngleDegrees( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw );
		else if( PlayerCameraRotateInfo.YawRotateDirection == -1 )
			DiffYaw = FMathSub::FindDeltaAngleDegreesWithThreshold( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw, -YawRotateThresholdAngle );
		else if( PlayerCameraRotateInfo.YawRotateDirection == 1 )
			DiffYaw = FMathSub::FindDeltaAngleDegreesWithThreshold( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw, YawRotateThresholdAngle );

		PlayerCameraRotateInfo.CurrentRotateSpeed += PlayerCameraUpdateDesc.DeltaTime * PlayerCameraPresetInfo.PlayerCameraPreset->RotationAccSpeed;

		const float MaxDeltaYaw = DiffYaw * PlayerCameraPresetInfo.PlayerCameraPreset->RotationLerpSpeed * PlayerCameraUpdateDesc.DeltaTime;
		const float DeltaYaw = FMath::Min( PlayerCameraRotateInfo.CurrentRotateSpeed, MaxDeltaYaw );

		PlayerCameraRotateInfo.CurrentRotateSpeed = DeltaYaw / PlayerCameraUpdateDesc.DeltaTime;

		CameraYaw += DeltaYaw;

		PlayerCameraRotateInfo.YawRotateDirection = DeltaYaw > 0.0f ? 1 : -1;
	}
	else if( PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::RotateYawPitchBySpeed )
	{
		float DiffYaw = 0.0f;
		if( PlayerCameraRotateInfo.YawRotateDirection == 0 )
			DiffYaw = FMath::FindDeltaAngleDegrees( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw );
		else if( PlayerCameraRotateInfo.YawRotateDirection == -1 )
			DiffYaw = FMathSub::FindDeltaAngleDegreesWithThreshold( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw, -YawRotateThresholdAngle );
		else if( PlayerCameraRotateInfo.YawRotateDirection == 1 )
			DiffYaw = FMathSub::FindDeltaAngleDegreesWithThreshold( CameraYaw, PlayerCameraRotateInfo.GoalCameraYaw, YawRotateThresholdAngle );

		const float DiffPitch = FMath::FindDeltaAngleDegrees( CameraPitch, PlayerCameraRotateInfo.GoalCameraPitch );
		const float DiffTotal = FMath::Abs( DiffYaw ) + FMath::Abs( DiffPitch );

		if( DiffTotal > UE_KINDA_SMALL_NUMBER )
		{
			static float MinRotateSpeed = 2.0f;

			PlayerCameraRotateInfo.CurrentRotateSpeed += PlayerCameraUpdateDesc.DeltaTime * PlayerCameraPresetInfo.PlayerCameraPreset->RotationAccSpeed;

			const float MaxDeltaYawPitch = DiffTotal * FMath::Clamp( PlayerCameraPresetInfo.PlayerCameraPreset->RotationLerpSpeed * PlayerCameraUpdateDesc.DeltaTime, 0.0f, 1.0f );
			const float DeltaYawPitch = FMath::Clamp( PlayerCameraRotateInfo.CurrentRotateSpeed * PlayerCameraUpdateDesc.DeltaTime, MinRotateSpeed * PlayerCameraUpdateDesc.DeltaTime, MaxDeltaYawPitch );
			const float DeltaYaw = DeltaYawPitch * ( FMath::Abs( DiffYaw ) / DiffTotal ) * ( DiffYaw > 0.0f ? 1.0f : -1.0f );
			const float DeltaPitch = DeltaYawPitch * ( FMath::Abs( DiffPitch ) / DiffTotal ) * ( DiffPitch > 0.0f ? 1.0f : -1.0f );

			PlayerCameraRotateInfo.CurrentRotateSpeed = DeltaYawPitch / FMath::Max( PlayerCameraUpdateDesc.DeltaTime, UE_KINDA_SMALL_NUMBER );

			CameraYaw += DeltaYaw;
			CameraPitch += DeltaPitch;

			PlayerCameraRotateInfo.YawRotateDirection = DeltaYaw > 0.0f ? 1 : -1;
		}
	}
}

bool APlayerCameraMode::IsSameCamera( const FPlayerCameraPresetInfo& Other ) const
{
	return PlayerCameraPresetInfo == Other;
}

void APlayerCameraMode::SetGoalYaw( const float GoalYaw )
{
	PlayerCameraRotateInfo.PlayerCameraRotateType = EPlayerCameraRotateType::RotateYawBySpeed;
	PlayerCameraRotateInfo.GoalCameraYaw = GoalYaw;
}

void APlayerCameraMode::SetGoalYawPitch( const float GoalYaw, const float GoalPitch )
{
	PlayerCameraRotateInfo.PlayerCameraRotateType = EPlayerCameraRotateType::RotateYawPitchBySpeed;
	PlayerCameraRotateInfo.GoalCameraYaw = GoalYaw;
	PlayerCameraRotateInfo.GoalCameraPitch = FMath::Clamp( GoalPitch, PlayerCameraPresetInfo.PlayerCameraPreset->PitchMinMax.X, PlayerCameraPresetInfo.PlayerCameraPreset->PitchMinMax.Y );
}

float APlayerCameraMode::GetGoalYaw() const
{
	if( PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::RotateYawBySpeed || PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::RotateYawPitchBySpeed )
		return PlayerCameraRotateInfo.GoalCameraYaw;

	return CameraYaw;
}

float APlayerCameraMode::GetGoalPitch() const
{
	if( PlayerCameraRotateInfo.PlayerCameraRotateType == EPlayerCameraRotateType::RotateYawPitchBySpeed )
		return PlayerCameraRotateInfo.GoalCameraPitch;

	return CameraPitch;
}

FVector APlayerCameraMode::GetActorCameraLocation( const AActor* Actor, const float HeightRatio ) const
{
	MJ_ASSERT_DEV( "김민진", Actor != nullptr, "GetActorCameraLocation의 인자 Actor가 nullptr입니다. 호출자가 보장해주어야 합니다." );
	return Actor->GetActorLocation() + GetActorHeightOffset( Actor, HeightRatio );
}

FVector APlayerCameraMode::GetActorHeightOffset( const AActor* Actor, const float HeightRatio ) const
{
	MJ_ASSERT_DEV( "김민진", Actor != nullptr, "GetActorCameraLocation의 인자 Actor가 nullptr입니다. 호출자가 보장해주어야 합니다." );

	float HeightDistance = 0.0f;
	if( const ACharacter* CharacterActor = Cast<const ACharacter>( Actor ) )
	{
		HeightDistance = CharacterActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		const USceneComponent* Root = Actor->GetRootComponent();
		if( const UCapsuleComponent* Capsule = Cast<const UCapsuleComponent>( Root ) )
			HeightDistance = Capsule->GetScaledCapsuleHalfHeight();
		else if( const USphereComponent* Sphere = Cast<const USphereComponent>( Root ) )
			HeightDistance = Sphere->GetScaledSphereRadius();
		else if( const UBoxComponent* Box = Cast<const UBoxComponent>( Root ) )
			HeightDistance = Box->GetScaledBoxExtent().Z;
		else
			MJ_ASSERT_DEV( "김민진", Actor != nullptr, "Root가 Capsule, Sphere, Box 중에 없습니다. 대응 필요!" );
	}

	return Actor->GetActorUpVector() * ( HeightDistance * HeightRatio );
}

TSortedArray<FObjectKey> APlayerCameraMode::GetDefaultTargetActorKeyList() const
{
	TSortedArray<FObjectKey> TargetActorKeyList;

	const AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey );
	if( PivotActor == nullptr )
		return TargetActorKeyList;

	if( const APlayerCharacter* PivotCharacter = Cast<APlayerCharacter>( PivotActor ) )
		TargetActorKeyList.Push( PivotCharacter->GetTargetActorComponent()->GetTargetActorKey() );

	return TargetActorKeyList;
}