#include "PlayerCameraModeManager.h"
#include "PlayerCameraPresetManager.h"
#include "PlayerCameraMode.h"
#include "PlayerCameraTPSMode.h"
#include "PlayerCameraLockOnMode.h"

#include "Engine/World.h"
#include "Engine/GameInstance.h"

#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"

#ifdef MJ_EDITOR
#include "DrawDebugHelpers.h"
#endif

float FPlayerCameraInstance::GetApplyRatio() const
{
	const FPlayerCameraBlendParam& PlayerCameraBlendParam = PlayerCameraMode->GetPlayerCameraPresetInfo().PlayerCameraPreset->PlayerCameraBlendParam;

	if( isBlendOut == false )
	{
		if( ElapsedTime >= PlayerCameraBlendParam.BlendInTime )
		{
			return 1.0f;
		}
		else
		{
			float BlendInRatio = FMath::Clamp( ElapsedTime / PlayerCameraBlendParam.BlendInTime, 0.f, 1.f );
			BlendInRatio = UKismetMathLibrary::Ease( 0.0, 1.0f, BlendInRatio, PlayerCameraBlendParam.BlendInEasingFunction );
			return BlendInRatio;
		}
	}
	else
	{
		float BlendOutRatio = FMath::Clamp( BlendOutElapsedTime / PlayerCameraBlendParam.BlendOutTime, 0.f, 1.f );
		BlendOutRatio = UKismetMathLibrary::Ease( 1.0, 0.0f, BlendOutRatio, PlayerCameraBlendParam.BlendOutEasingFunction );
		return BlendOutRatio;
	}
}

bool FPlayerCameraInstance::IsBlendOutComplete() const
{
	if( isBlendOut == false )
		return false;

	return BlendOutElapsedTime >= PlayerCameraMode->GetPlayerCameraPresetInfo().PlayerCameraPreset->PlayerCameraBlendParam.BlendOutTime;
}

FPlayerCameraResult	FPlayerCameraResult::Lerp( const FPlayerCameraResult& A, const FPlayerCameraResult& B, const float Alpha )
{
	FPlayerCameraResult Result;

	Result.PivotLocation = FMath::Lerp( A.PivotLocation, B.PivotLocation, Alpha );
	Result.CameraLocation = FMath::Lerp( A.CameraLocation, B.CameraLocation, Alpha );
	Result.CameraYaw = MathUtil::LerpDegree( A.CameraYaw, B.CameraYaw, Alpha );
	Result.CameraPitch = MathUtil::LerpDegree( A.CameraPitch, B.CameraPitch, Alpha );
	Result.ZoomDistance = FMath::Lerp( A.ZoomDistance, B.ZoomDistance, Alpha );
	Result.CameraFovDeg = FMath::Lerp( A.CameraFovDeg, B.CameraFovDeg, Alpha );

	return Result;
}

APlayerCameraModeManager::APlayerCameraModeManager()
{
	__noop;
}

void APlayerCameraModeManager::BeginPlay()
{
	Super::BeginPlay();
}

bool APlayerCameraModeManager::SetPlayerCameraPreset( const EPlayerCameraPriority Priority, const TObjectPtr<UPlayerCameraPreset>& PlayerCameraPreset, const FObjectKey PivotActorKey, const TSortedArray<FObjectKey> TargetActorKeyList )
{
	if( Priority == EPlayerCameraPriority::Count || PlayerCameraPreset == nullptr )
		return false;

	FPlayerCameraPresetInfo NewCameraPresetInfo;
	NewCameraPresetInfo.PlayerCameraPreset = PlayerCameraPreset;
	NewCameraPresetInfo.Priority = Priority;
	
	if( PivotActorKey != FObjectKey() )
		NewCameraPresetInfo.PivotActorKey = PivotActorKey;
	else
		NewCameraPresetInfo.PivotActorKey = FObjectKey( GetWorld()->GetFirstPlayerController()->GetPawn() );

	if( NewCameraPresetInfo.PivotActorKey == nullptr )
		return false;

	NewCameraPresetInfo.TargetActorKeyList = TargetActorKeyList;

	bool bInserted = false;
	const int32 CameraPresetCount = CameraPresetList.Num();
	for( int32 CameraPresetIndex = 0; CameraPresetIndex < CameraPresetCount; ++CameraPresetIndex )
	{
		if( CameraPresetList[CameraPresetIndex].Priority == NewCameraPresetInfo.Priority )
		{
			CameraPresetList[CameraPresetIndex] = NewCameraPresetInfo;
			bInserted = true;
			break;
		}
	}

	if( bInserted == false )
	{
		CameraPresetList.Add( NewCameraPresetInfo );
		CameraPresetList.Sort( []( const FPlayerCameraPresetInfo& A, const FPlayerCameraPresetInfo& B )
			{
				return A.Priority < B.Priority;
			}
		);
	}

	return true;
}

void APlayerCameraModeManager::ClearPlayerCameraPreset( const EPlayerCameraPriority Priority )
{
	const int32 CameraPresetCount = CameraPresetList.Num();
	for( int32 CameraPresetIndex = 0; CameraPresetIndex < CameraPresetCount; ++CameraPresetIndex )
	{
		if( CameraPresetList[CameraPresetIndex].Priority == Priority )
			CameraPresetList.RemoveAt( CameraPresetIndex );
	}
}

void APlayerCameraModeManager::UpdateCamera( float DeltaTime )
{
	Super::UpdateCamera( DeltaTime );

	FPlayerCameraUpdateDesc PlayerCameraUpdateDesc;
	PlayerCameraUpdateDesc.DeltaTime = DeltaTime;
	PlayerCameraUpdateDesc.ZoomRatio = ZoomRatio;
	PlayerCameraUpdateDesc.DeltaYaw = RotationInput.Yaw;
	PlayerCameraUpdateDesc.DeltaPitch = RotationInput.Pitch;

	RotationInput = FRotator::ZeroRotator;

	UpdateCamera_ZoomRatio( PlayerCameraUpdateDesc );
	UpdateCamera_UpdatePresetInfo( PlayerCameraUpdateDesc );
	UpdateCamera_CreateCameraMode( PlayerCameraUpdateDesc );
	UpdateCamera_UpdateCameraMode( PlayerCameraUpdateDesc );
	UpdateCamera_BlendCameraResult( PlayerCameraUpdateDesc );

#ifdef MJ_EDITOR
	DebugDraw_PlayerCameraManager();
#endif
}

void APlayerCameraModeManager::UpdateViewTargetInternal( FTViewTarget& OutVT, float DeltaTime )
{
	FRotator CameraRotator( PlayerCameraResult.CameraPitch, PlayerCameraResult.CameraYaw, 0.0f );
	FVector CameraBackwardVector = -( FRotationMatrix( CameraRotator ).GetUnitAxis( EAxis::X ) );

	OutVT.POV.Rotation = CameraRotator;
	OutVT.POV.Location = PlayerCameraResult.CameraLocation + ( CameraBackwardVector * PlayerCameraResult.ZoomDistance ); // MJTODO 충돌처리
	OutVT.POV.FOV = PlayerCameraResult.CameraFovDeg;
}

void APlayerCameraModeManager::ApplyDeltaRotation( const FRotator& InRotationInput )
{
	RotationInput = InRotationInput;
}

void APlayerCameraModeManager::ApplyZoom( const float WheelDelta )
{
	constexpr float ZoomSpeed = 1.0f / 3.0f;

	PrevGoalZoomRatio = CurrentGoalZoomRatio;
	CurrentGoalZoomRatio += -WheelDelta * ZoomSpeed;
	CurrentGoalZoomRatio = FMath::Clamp( CurrentGoalZoomRatio, 0.0f, 1.0f );

	GoalZoomApplyRatio = 0.0f;
}

void APlayerCameraModeManager::UpdateCamera_ZoomRatio( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	static float ZoomChangeDuration = 0.25f;

	GoalZoomApplyRatio += PlayerCameraUpdateDesc.DeltaTime / ZoomChangeDuration;
	GoalZoomApplyRatio = FMath::Clamp( GoalZoomApplyRatio, 0.0f, 1.0f );

	ZoomRatio = UKismetMathLibrary::Ease( PrevGoalZoomRatio, CurrentGoalZoomRatio, GoalZoomApplyRatio, EEasingFunc::EaseOut );
}

void APlayerCameraModeManager::UpdateCamera_UpdatePresetInfo( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	CameraPresetList.RemoveAll( []( const FPlayerCameraPresetInfo& PlayerCameraPresetInfo )
		{
			return PlayerCameraPresetInfo.PivotActorKey == nullptr;
		} );
}

void APlayerCameraModeManager::UpdateCamera_CreateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	if( CameraPresetList.IsEmpty() )
		return;

	const FPlayerCameraPresetInfo& LastPlayerCameraPresetInfo = CameraPresetList.Last();
	if( CameraInstanceList.Num() == 0 || CameraInstanceList.Last().PlayerCameraMode->IsSameCamera( LastPlayerCameraPresetInfo ) == false )
	{
		FPlayerCameraInstance PlayerCameraInstance;

		switch( LastPlayerCameraPresetInfo.PlayerCameraPreset->GetPlayerCameraModeType() )
		{
			case EPlayerCameraModeType::TPS:
			{
				PlayerCameraInstance.PlayerCameraMode = GetWorld()->SpawnActor<APlayerCameraTPSMode>( APlayerCameraTPSMode::StaticClass() );
			}
			break;
			case EPlayerCameraModeType::LockOn:
			{
				PlayerCameraInstance.PlayerCameraMode = GetWorld()->SpawnActor<APlayerCameraLockOnMode>( APlayerCameraLockOnMode::StaticClass() );
			}
			break;
			default:
			{
				MJ_ASSERT_DEV( "김민진", false, "사용하면 안 되는 타입입니다. 코드를 확인해주세요." );
				return;
			}
			break;
		}

		PlayerCameraInstance.PlayerCameraMode->Initialize( LastPlayerCameraPresetInfo, PlayerCameraResult );

		CameraInstanceList.Add( MoveTemp( PlayerCameraInstance ) );
	}

	for( int32 Index = CameraInstanceList.Num() - 1; Index >= 0; --Index )
	{
		if( FMath::IsNearlyEqual( CameraInstanceList[Index].GetApplyRatio(), 1.0f ) )
		{
			if( Index > 0 )
				CameraInstanceList.RemoveAt( 0, Index );

			break;
		}
	}
}

void APlayerCameraModeManager::UpdateCamera_UpdateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	const int32 CameraInstanceNum = CameraInstanceList.Num();
	for( int32 CameraInstanceIndex = 0; CameraInstanceIndex < CameraInstanceNum; ++CameraInstanceIndex )
	{
		CameraInstanceList[CameraInstanceIndex].ElapsedTime += PlayerCameraUpdateDesc.DeltaTime;
		if( CameraInstanceList[CameraInstanceIndex].isBlendOut )
			CameraInstanceList[CameraInstanceIndex].BlendOutElapsedTime += PlayerCameraUpdateDesc.DeltaTime;

		CameraInstanceList[CameraInstanceIndex].PlayerCameraMode->UpdateCameraMode( PlayerCameraUpdateDesc );
	}

	CameraInstanceList.RemoveAll( []( const FPlayerCameraInstance& PlayerCameraInstance )
		{
			if( PlayerCameraInstance.IsBlendOutComplete() )
				return true;

			if( PlayerCameraInstance.PlayerCameraMode->IsValid() == false )
				return true;

			return false;
		} );
}

void APlayerCameraModeManager::UpdateCamera_BlendCameraResult( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	FPlayerCameraResult PervPlayerCameraResult = PlayerCameraResult;
	float RemainApplyRatio = 1.0f;

	for( int32 CameraInstanceIndex = CameraInstanceList.Num() - 1; CameraInstanceIndex >= 0; --CameraInstanceIndex )
	{
		const float ApplyRatio = CameraInstanceList[CameraInstanceIndex].GetApplyRatio();

		FPlayerCameraResult PlayerCameraModeResult = CameraInstanceList[CameraInstanceIndex].PlayerCameraMode->GetPlayerCameraModeResult();

		if( CameraInstanceIndex == CameraInstanceList.Num() - 1 )
			PlayerCameraResult = PlayerCameraModeResult;
		else
			PlayerCameraResult = PlayerCameraResult.Lerp( PlayerCameraResult, PlayerCameraModeResult, RemainApplyRatio );

		if( ApplyRatio == 1.0f )
			break;

		RemainApplyRatio *= 1.0f - ApplyRatio;
	}
}

#ifdef MJ_EDITOR
void APlayerCameraModeManager::ToggleDebugDrawSwitch()
{
	IsDebugDraw = !IsDebugDraw;
}

void APlayerCameraModeManager::DebugDraw_PlayerCameraManager()
{
	if( IsDebugDraw == false )
		return;

	// Draw Element
	{
		constexpr float AspectRatio = 16.0f / 9.0f;
		const UWorld* World = GetWorld();

		const FRotator CameraRotation( PlayerCameraResult.CameraPitch, PlayerCameraResult.CameraYaw, 0.0f );
		const FVector CameraBackwardVector = -CameraRotation.Vector();

		const FVector CameraLocation = PlayerCameraResult.CameraLocation + ( CameraBackwardVector * PlayerCameraResult.ZoomDistance );

		DrawDebugLine( World, PlayerCameraResult.PivotLocation, CameraLocation, FColor::Yellow, false, -1.0f, 0, 2.0f );

		DrawDebugLine( World, PlayerCameraResult.CameraLocation, CameraLocation, FColor::Orange, false, -1.0f, 0, 1.0f );

		DrawDebugPoint( World, PlayerCameraResult.PivotLocation, 12.0f, FColor::Red, false, -1.0f );
		DrawDebugPoint( World, PlayerCameraResult.CameraLocation, 12.0f, FColor::Green, false, -1.0f );

		const float HalfHFOVRad = FMath::DegreesToRadians( PlayerCameraResult.CameraFovDeg * 0.5f );

		const float HalfWidth = PlayerCameraResult.ZoomDistance * FMath::Tan( HalfHFOVRad );
		const float HalfHeight = HalfWidth / AspectRatio;

		const FMatrix CamMatrix = FRotationMatrix( CameraRotation );
		const FVector Forward = CamMatrix.GetScaledAxis( EAxis::X );
		const FVector Right = CamMatrix.GetScaledAxis( EAxis::Y );
		const FVector Up = CamMatrix.GetScaledAxis( EAxis::Z );

		const FVector PlaneCenter = PlayerCameraResult.CameraLocation + ( Forward * PlayerCameraResult.ZoomDistance );

		const FVector TopLeft = PlaneCenter - ( Right * HalfWidth ) + ( Up * HalfHeight );
		const FVector TopRight = PlaneCenter + ( Right * HalfWidth ) + ( Up * HalfHeight );
		const FVector BottomLeft = PlaneCenter - ( Right * HalfWidth ) - ( Up * HalfHeight );
		const FVector BottomRight = PlaneCenter + ( Right * HalfWidth ) - ( Up * HalfHeight );

		DrawDebugLine( World, TopLeft, TopRight, FColor::Cyan, false, -1.0f, 0, 1.5f );
		DrawDebugLine( World, TopRight, BottomRight, FColor::Cyan, false, -1.0f, 0, 1.5f );
		DrawDebugLine( World, BottomRight, BottomLeft, FColor::Cyan, false, -1.0f, 0, 1.5f );
		DrawDebugLine( World, BottomLeft, TopLeft, FColor::Cyan, false, -1.0f, 0, 1.5f );

		DrawDebugLine( World, PlayerCameraResult.CameraLocation, TopLeft, FColor::Cyan, false, -1.0f, 0, 1.0f );
		DrawDebugLine( World, PlayerCameraResult.CameraLocation, TopRight, FColor::Cyan, false, -1.0f, 0, 1.0f );
		DrawDebugLine( World, PlayerCameraResult.CameraLocation, BottomLeft, FColor::Cyan, false, -1.0f, 0, 1.0f );
		DrawDebugLine( World, PlayerCameraResult.CameraLocation, BottomRight, FColor::Cyan, false, -1.0f, 0, 1.0f );
	}

	// Draw Text
	{
		int32 KeyIndex = 100;

		GEngine->AddOnScreenDebugMessage( KeyIndex--, 0.0f, FColor::Cyan, FString::Printf( TEXT( "[PlayerCameraManager]" ) ) );

		for( int32 i = 0; i < CameraInstanceList.Num(); ++i )
		{
			const FPlayerCameraInstance& Inst = CameraInstanceList[i];
			const FPlayerCameraResult ModeResult = Inst.PlayerCameraMode->GetPlayerCameraModeResult();

			FString DebugString = FString::Printf( TEXT( "-[%s] ApplyRatio: %.2f Loc[%.2f %.2f %.2f] YP[%.2f %.2f] Piv[%.2f %.2f %.2f] D[%.2f]" ),
				*Inst.PlayerCameraMode->GetName(),
				Inst.GetApplyRatio(),
				ModeResult.CameraLocation.X,
				ModeResult.CameraLocation.Y,
				ModeResult.CameraLocation.Z,
				ModeResult.CameraYaw,
				ModeResult.CameraPitch,
				ModeResult.PivotLocation.X,
				ModeResult.PivotLocation.Y,
				ModeResult.PivotLocation.Z,
				ModeResult.ZoomDistance
			);
			GEngine->AddOnScreenDebugMessage( KeyIndex--, 0.0f, FColor::White, DebugString );
		}

		FString DebugString = FString::Printf( TEXT( "Result Loc[% .2f % .2f % .2f] YP[% .2f % .2f] Piv[% .2f % .2f % .2f] D[% .2f]" ),
			PlayerCameraResult.CameraLocation.X,
			PlayerCameraResult.CameraLocation.Y,
			PlayerCameraResult.CameraLocation.Z,
			PlayerCameraResult.CameraYaw,
			PlayerCameraResult.CameraPitch,
			PlayerCameraResult.PivotLocation.X,
			PlayerCameraResult.PivotLocation.Y,
			PlayerCameraResult.PivotLocation.Z,
			PlayerCameraResult.ZoomDistance
		);

		GEngine->AddOnScreenDebugMessage( KeyIndex--, 0.0f, FColor::White, DebugString );
	}
}
#endif