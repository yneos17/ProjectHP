#include "Camera/PlayerCameraTPSMode.h"
#include "Camera/PlayerCameraModeManager.h"
#include "Actor/ClientActorManager.h"

APlayerCameraTPSMode::APlayerCameraTPSMode()
{
	__noop;
}

FPlayerCameraResult APlayerCameraTPSMode::GetPlayerCameraModeResult() const
{
	FPlayerCameraResult PlayerCameraResult;

	PlayerCameraResult.PivotLocation = PivotLocation;
	PlayerCameraResult.CameraLocation = CameraLocation;
	PlayerCameraResult.CameraYaw = CameraYaw;
	PlayerCameraResult.CameraPitch = CameraPitch;
	PlayerCameraResult.ZoomDistance = ZoomDistance;
	PlayerCameraResult.CameraFovDeg = CameraFovDeg;

	return PlayerCameraResult;
}

void APlayerCameraTPSMode::UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	const UPlayerCameraTPSPreset* CastedPreset = static_cast<const UPlayerCameraTPSPreset*>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey );
	if( PivotActor == nullptr )
	{
		bIsValid = false;
		return;
	}

	CameraYaw += PlayerCameraUpdateDesc.DeltaYaw;
	CameraPitch += PlayerCameraUpdateDesc.DeltaPitch;

	CameraFovDeg = CastedPreset->CameraFovDeg;

	UpdateCameraMode_CalcRightUpOffsetInWorld( PlayerCameraUpdateDesc );
	ZoomDistance = FMath::GetMappedRangeValueClamped( FVector2D( 0.0f, 1.0f ), CastedPreset->ZoomDistanceRange, PlayerCameraUpdateDesc.ZoomRatio );

	PivotLocation = GetActorCameraLocation( PivotActor, CastedPreset->PivotActorHeightRatio );
	LootAtLocation = PivotLocation + RightUpOffsetInWorld;

	CameraLocation = LootAtLocation;
}

void APlayerCameraTPSMode::UpdateCameraMode_CalcRightUpOffsetInWorld( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	const UPlayerCameraTPSPreset* CastedPreset = static_cast<const UPlayerCameraTPSPreset*>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	const float RightOffset = FMath::GetMappedRangeValueClamped( FVector2D( 0.0f, 1.0f ), CastedPreset->RightOffsetRange, PlayerCameraUpdateDesc.ZoomRatio );
	const float UpOffset = FMath::GetMappedRangeValueClamped( FVector2D( 0.0f, 1.0f ), CastedPreset->UpOffsetRange, PlayerCameraUpdateDesc.ZoomRatio );

	const FRotationMatrix CameraRotationMatrix( FRotator( CameraPitch, CameraYaw, 0.0f ) );
	const FVector CameraRightDirection = CameraRotationMatrix.GetUnitAxis( EAxis::Y );
	const FVector CameraUpDirection = CameraRotationMatrix.GetUnitAxis( EAxis::Z );
	
	RightUpOffsetInWorld = ( CameraRightDirection * RightOffset ) + ( CameraUpDirection * UpOffset );
}