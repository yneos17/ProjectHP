#include "Camera/PlayerCameraLockOnMode.h"
#include "Actor/ClientActorManager.h"

FPlayerCameraResult APlayerCameraLockOnMode::GetPlayerCameraModeResult() const
{
	// MJTODO .. 멤버로 가지고 있고 레퍼런스로 뱉는게?

	return FPlayerCameraResult();
}

void APlayerCameraLockOnMode::UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	const UPlayerCameraLockOnPreset* CastedPreset = static_cast<const UPlayerCameraLockOnPreset*>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	const AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey );
	if( PivotActor == nullptr )
	{
		bIsValid = false;
		return;
	}

	TArray<const AActor*> TargetActorList;
	const int32 TargetActorListCount = PlayerCameraPresetInfo.TargetActorKeyList.Num();
	for( int32 TargetActorIndex = 0; TargetActorIndex < TargetActorListCount; ++TargetActorIndex )
	{
		const AActor* TargetActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.TargetActorKeyList[TargetActorIndex]);
		if( TargetActor == nullptr )
		{
			bIsValid = false;
			return;
		}

		TargetActorList.Push( TargetActor );
	}

	CameraFovDeg = FMath::DegreesToRadians( CastedPreset->CameraFovDeg );

	const float GoalZoomDistance = FMath::Clamp( UpdateCameraMode_GetNeedMinCameraDistance( CastedPreset, PivotActor, TargetActorList ), CastedPreset->ClampCameraDistanceRange.X, CastedPreset->ClampCameraDistanceRange.Y );
	ZoomDistance = FMath::Lerp( ZoomDistance, GoalZoomDistance, CastedPreset->DistanceLerpSpeed * PlayerCameraUpdateDesc.DeltaTime );

	PivotLocation = GetActorCameraLocation( PivotActor, CastedPreset->PivotActorHeightRatio );

	FVector TargetActorLocation = GetActorCameraLocation( PivotActor, CastedPreset->TargetActorHeightRatio );
	LootAtLocation = FMath::Lerp( PivotLocation, TargetActorLocation, CastedPreset->CameraCenterRatio );

	const FRotationMatrix CameraRotationMatrix( FRotator( CameraPitch, CameraYaw, 0.0f ) );
	CameraLocation = LootAtLocation + ( ZoomDistance * -CameraRotationMatrix.GetUnitAxis( EAxis::X ) );

	// MJTODO
}

float APlayerCameraLockOnMode::UpdateCameraMode_GetNeedMinCameraDistance( const UPlayerCameraLockOnPreset* CastedPreset, const AActor* PivotActor, const TArray<const AActor*>& TargetActorList )
{
	float NeedMinCameraDistance = 0.0f;

	const FVector PivotActorLowerPoint = GetActorCameraLocation( PivotActor, CastedPreset->ClampPointHeightRatioRange.X );
	const FVector PivotActorUpperPoint = GetActorCameraLocation( PivotActor, CastedPreset->ClampPointHeightRatioRange.Y );

	NeedMinCameraDistance = FMath::Max( UpdateCameraMode_GetNeedMinCameraDistancePerPoint( PivotActorLowerPoint ), UpdateCameraMode_GetNeedMinCameraDistancePerPoint( PivotActorUpperPoint ) );

	const int32 TargetActorListCount = TargetActorList.Num();
	for( int32 TargetActorIndex = 0; TargetActorIndex < TargetActorListCount; ++TargetActorIndex )
	{
		const FVector TargetActorLowerPoint = GetActorCameraLocation( TargetActorList[TargetActorIndex], CastedPreset->ClampPointHeightRatioRange.X );
		const FVector TargetActorUpperPoint = GetActorCameraLocation( TargetActorList[TargetActorIndex], CastedPreset->ClampPointHeightRatioRange.Y );

		NeedMinCameraDistance = FMath::Max( NeedMinCameraDistance, UpdateCameraMode_GetNeedMinCameraDistancePerPoint( TargetActorLowerPoint ) );
		NeedMinCameraDistance = FMath::Max( NeedMinCameraDistance, UpdateCameraMode_GetNeedMinCameraDistancePerPoint( TargetActorUpperPoint ) );
	}

	return NeedMinCameraDistance;
}

float APlayerCameraLockOnMode::UpdateCameraMode_GetNeedMinCameraDistancePerPoint( const FVector& Point )
{
	// MJTODO 잘 아는 그거

	return 0.0f;
}