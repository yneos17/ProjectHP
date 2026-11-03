#include "Camera/PlayerCameraLockOnMode.h"
#include "Actor/ClientActorManager.h"
#include "Character/PlayerCharacter.h"
#include "ActorComponent/TargetActorComponent.h"

#ifdef MJ_EDITOR
#include "DrawDebugHelpers.h"
#endif

FPlayerCameraResult APlayerCameraLockOnMode::GetPlayerCameraModeResult() const
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

void APlayerCameraLockOnMode::Initialize( const FPlayerCameraPresetInfo& InPlayerCameraPresetInfo, const FPlayerCameraResult& PrevPlayerCameraResult )
{
	Super::Initialize( InPlayerCameraPresetInfo, PrevPlayerCameraResult );

	const UPlayerCameraLockOnPreset* CastedPreset = Cast<UPlayerCameraLockOnPreset>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	const AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey );
	if( PivotActor == nullptr )
	{
		bIsValidCamera = false;
		return;
	}

	if( PlayerCameraPresetInfo.TargetActorKeyList.IsEmpty() == true || CastedPreset->FixedTarget == false )
		PlayerCameraPresetInfo.TargetActorKeyList = GetDefaultTargetActorKeyList();

	if( PlayerCameraPresetInfo.TargetActorKeyList.IsEmpty() )
	{
		bIsValidCamera = false;
		return;
	}

	TArray<const AActor*> TargetActorList;
	if( UpdateCameraMode_GetTargetActorList( TargetActorList ) == false )
	{
		bIsValidCamera = false;
		return;
	}

	PivotLocation = GetActorCameraLocation( PivotActor, CastedPreset->PivotActorHeightRatio );
	UpdateCameraMode_UpdateTargetListCenterLocation( CastedPreset, TargetActorList );
	LookAtLocation = FMath::Lerp( PivotLocation, TargetListCenterLocation, CastedPreset->CameraCenterRatio );

	if( CastedPreset->AutoRotate )
	{
		UpdateCameraMode_AutoRotate( CastedPreset );

		CameraYaw = GetGoalYaw();
		CameraPitch = GetGoalPitch();
	}

	CameraFovDeg = CastedPreset->CameraFovDeg;

	ZoomDistance = FMath::Clamp( UpdateCameraMode_GetNeedMinCameraDistance( CastedPreset, PivotActor, TargetActorList ), CastedPreset->ClampCameraDistanceRange.X, CastedPreset->ClampCameraDistanceRange.Y );

	const FRotationMatrix CameraRotationMatrix( FRotator( CameraPitch, CameraYaw, 0.0f ) );
	CameraLocation = LookAtLocation;
}

void APlayerCameraLockOnMode::UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	const UPlayerCameraLockOnPreset* CastedPreset = Cast<UPlayerCameraLockOnPreset>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	const AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey );
	if( PivotActor == nullptr )
	{
		bIsValidCamera = false;
		return;
	}

	if( PlayerCameraPresetInfo.TargetActorKeyList.IsEmpty() == true || CastedPreset->FixedTarget == false )
		PlayerCameraPresetInfo.TargetActorKeyList = GetDefaultTargetActorKeyList();

	if( PlayerCameraPresetInfo.TargetActorKeyList.IsEmpty() )
	{
		bIsValidCamera = false;
		return;
	}

	TArray<const AActor*> TargetActorList;
	if( UpdateCameraMode_GetTargetActorList( TargetActorList ) == false )
	{
		bIsValidCamera = false;
		return;
	}

	PivotLocation = GetActorCameraLocation( PivotActor, CastedPreset->PivotActorHeightRatio );
	UpdateCameraMode_UpdateTargetListCenterLocation( CastedPreset, TargetActorList );
	LookAtLocation = FMath::Lerp( PivotLocation, TargetListCenterLocation, CastedPreset->CameraCenterRatio );

	UpdateCameraMode_Rotate( CastedPreset, PlayerCameraUpdateDesc );

	CameraFovDeg = CastedPreset->CameraFovDeg;
									
	const float GoalZoomDistance = FMath::Clamp( UpdateCameraMode_GetNeedMinCameraDistance( CastedPreset, PivotActor, TargetActorList ), CastedPreset->ClampCameraDistanceRange.X, CastedPreset->ClampCameraDistanceRange.Y );
	ZoomDistance = FMath::Lerp( ZoomDistance, GoalZoomDistance, FMath::Clamp( CastedPreset->DistanceLerpSpeed * PlayerCameraUpdateDesc.DeltaTime, 0.0f, 1.0f ) );

	const FRotationMatrix CameraRotationMatrix( FRotator( CameraPitch, CameraYaw, 0.0f ) );
	CameraLocation = LookAtLocation;
}

void APlayerCameraLockOnMode::UpdateCameraMode_Rotate( const UPlayerCameraLockOnPreset* CastedPreset, const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	if( bIsBlendOut )
		return;

	if( CastedPreset->AutoRotate )
	{
		UpdateCameraMode_AutoRotate( CastedPreset );
	}
	else
	{
		CameraYaw += PlayerCameraUpdateDesc.DeltaYaw;
		CameraPitch += PlayerCameraUpdateDesc.DeltaPitch;

		CameraPitch = FMath::Clamp( CameraPitch, CastedPreset->PitchMinMax.X, CastedPreset->PitchMinMax.Y );
	}
}

void APlayerCameraLockOnMode::UpdateCameraMode_AutoRotate( const UPlayerCameraLockOnPreset* CastedPreset )
{
	static float ClampPitchAngleDeg = -15.0f;

	const FVector PivotToTargetDirection = ( TargetListCenterLocation - PivotLocation ).GetSafeNormal();
	const FRotator PivotToTargetRotator = PivotToTargetDirection.ToOrientationRotator();

	float GoalCameraYaw = PivotToTargetRotator.Yaw + CastedPreset->OffsetYaw;

	float GoalCameraPitch;
	if( CastedPreset->IsClampPitch )
		GoalCameraPitch = FMath::Min( PivotToTargetRotator.Pitch + CastedPreset->OffsetPitch, ClampPitchAngleDeg );
	else
		GoalCameraPitch = PivotToTargetRotator.Pitch + CastedPreset->OffsetPitch;

	SetGoalYawPitch( GoalCameraYaw, GoalCameraPitch );
}

bool APlayerCameraLockOnMode::UpdateCameraMode_GetTargetActorList( TArray<const AActor*>& OutTargetActorList ) const
{
	const int32 TargetActorListCount = PlayerCameraPresetInfo.TargetActorKeyList.Num();
	for( int32 TargetActorIndex = 0; TargetActorIndex < TargetActorListCount; ++TargetActorIndex )
	{
		const AActor* TargetActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.TargetActorKeyList[TargetActorIndex] );
		if( TargetActor == nullptr )
			return false;

		OutTargetActorList.Push( TargetActor );
	}

	return true;
}

void APlayerCameraLockOnMode::UpdateCameraMode_UpdateTargetListCenterLocation( const UPlayerCameraLockOnPreset* CastedPreset, const TArray<const AActor*>& TargetActorList )
{
	int32 TargetActorListCount = TargetActorList.Num();

	TargetListCenterLocation = FVector::ZeroVector;
	for( int32 TargetActorIndex = 0; TargetActorIndex < TargetActorListCount; ++TargetActorIndex )
		TargetListCenterLocation += GetActorCameraLocation( TargetActorList[TargetActorIndex], CastedPreset->TargetActorHeightRatio );

	TargetListCenterLocation /= TargetActorListCount;
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
	static float ScreenFormRatio = 0.9f;

	const FRotationMatrix CameraRotationMatrix( FRotator( GetGoalPitch(), GetGoalYaw(), 0.0f));
	const FVector CameraForwardDirection = CameraRotationMatrix.GetUnitAxis( EAxis::X );
	const FVector CameraRightDirection = CameraRotationMatrix.GetUnitAxis( EAxis::Y );
	const FVector CameraUpDirection = CameraRotationMatrix.GetUnitAxis( EAxis::Z );

	const float ProjectedLength = ( Point - LookAtLocation ).Dot( CameraForwardDirection );
	const FVector LookAtLocationInPointSpace = LookAtLocation + ( CameraForwardDirection * ProjectedLength );

	const FVector LookAtToPointVectorInCameraSpace = ( Point - LookAtLocationInPointSpace );
	const float RightComponent = LookAtToPointVectorInCameraSpace.Dot( CameraRightDirection );
	const float UpComponent = LookAtToPointVectorInCameraSpace.Dot( CameraUpDirection );
	const float VectorLength = FMath::Sqrt( FMath::Square( RightComponent ) + FMath::Square( UpComponent ) );
	const float DenominatorLength = FMath::Sqrt( FMath::Square( RightComponent / APlayerCameraModeManager::GetAspectRatio() ) + FMath::Square( UpComponent ) );
	const float ApplyAspectRatio = VectorLength / DenominatorLength;

	const float TanHalfFov = FMath::Tan( FMath::DegreesToRadians( CameraFovDeg * 0.5f ) );
	const float TanHalfFovWithRatio = TanHalfFov * ApplyAspectRatio * ScreenFormRatio;

	const float DistanceA = FVector::Distance( LookAtLocationInPointSpace, Point );
	const float DistanceC = DistanceA / TanHalfFovWithRatio;

	return FMath::Max( DistanceC - ProjectedLength, 0.0f );
}

#ifdef MJ_EDITOR
void APlayerCameraLockOnMode::DebugDraw_PlayerCameraMode()
{
	const UWorld* World = GetWorld();

	const UPlayerCameraLockOnPreset* CastedPreset = Cast<UPlayerCameraLockOnPreset>( PlayerCameraPresetInfo.PlayerCameraPreset );
	MJ_ASSERT_DEV( "김민진", CastedPreset != nullptr, "CastedPreset이 nullptr입니다. 코드 수정이 필요합니다." );

	DrawDebugLine( World, PivotLocation, TargetListCenterLocation, FColor::Cyan, false, -1.0f, 0, 1.5f );

	DrawDebugSphere( World, LookAtLocation, 12.0f, 4, FColor::Cyan );

	if( const AActor* PivotActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.PivotActorKey ) )
	{
		DrawDebugSphere( World, GetActorCameraLocation( PivotActor, CastedPreset->ClampPointHeightRatioRange.X ), 12.0f, 4, FColor::Cyan );
		DrawDebugSphere( World, GetActorCameraLocation( PivotActor, CastedPreset->ClampPointHeightRatioRange.Y ), 12.0f, 4, FColor::Cyan );
	}

	const int32 TargetActorListCount = PlayerCameraPresetInfo.TargetActorKeyList.Num();
	for( int32 TargetActorIndex = 0; TargetActorIndex < TargetActorListCount; ++TargetActorIndex )
	{
		if( const AActor* TargetActor = GetWorld()->GetSubsystem<UClientActorManager>()->GetActor( PlayerCameraPresetInfo.TargetActorKeyList[TargetActorIndex] ) )
		{
			DrawDebugSphere( World, GetActorCameraLocation( TargetActor, CastedPreset->ClampPointHeightRatioRange.X ), 12.0f, 4, FColor::Cyan );
			DrawDebugSphere( World, GetActorCameraLocation( TargetActor, CastedPreset->ClampPointHeightRatioRange.Y ), 12.0f, 4, FColor::Cyan );
		}
	}
}
#endif