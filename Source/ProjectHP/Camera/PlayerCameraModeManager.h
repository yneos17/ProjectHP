#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraManager.h"
#include "Camera/PlayerCameraCommon.h"
#include "PlayerCameraModeManager.generated.h"

class APlayerCameraMode;

struct FPlayerCameraUpdateDesc
{
	float	DeltaTime;
	float	ZoomRatio;
	float	DeltaYaw;
	float	DeltaPitch;
};

USTRUCT()
struct FPlayerCameraInstance
{
	GENERATED_BODY()

	float						GetApplyRatio() const;
	bool						IsBlendOutComplete() const;

	UPROPERTY()
	APlayerCameraMode*			PlayerCameraMode = nullptr;
	float						ElapsedTime = 0.0f;
	bool						isBlendOut = false;
	float						BlendOutElapsedTime = 0.0;

};

struct FPlayerCameraResult
{
	static FPlayerCameraResult	Lerp( const FPlayerCameraResult& A, const FPlayerCameraResult& B, const float Alpha );

	FVector						PivotLocation;
	FVector						CameraLocation;
	float						CameraYaw;
	float						CameraPitch;
	float						ZoomDistance;
	float						CameraFovDeg;
};

UCLASS()
class PROJECTHP_API APlayerCameraModeManager : public APlayerCameraManager
{
	GENERATED_BODY()
public:
	APlayerCameraModeManager();

	virtual void BeginPlay() override;

	bool SetPlayerCameraPreset( const EPlayerCameraPriority Priority, const TObjectPtr<UPlayerCameraPreset>& PlayerCameraPreset, const FObjectKey PivotActorKey = FObjectKey(), const TSortedArray<FObjectKey> TargetActorKeyList = TSortedArray<FObjectKey>() );
	void ClearPlayerCameraPreset( const EPlayerCameraPriority Priority );

	virtual void UpdateCamera( float DeltaTime ) override;
	virtual void UpdateViewTargetInternal( FTViewTarget& OutVT, float DeltaTime ) override;

	void ApplyDeltaRotation( const FRotator& InRotationInput );
	void ApplyZoom( const float WheelDelta );

private:
	void UpdateCamera_ZoomRatio( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );
	void UpdateCamera_UpdatePresetInfo( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );
	void UpdateCamera_CreateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );
	void UpdateCamera_UpdateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );
	void UpdateCamera_BlendCameraResult( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );

#ifdef MJ_EDITOR
public:
	void ToggleDebugDrawSwitch();
	void DebugDraw_PlayerCameraManager();
#endif

private:
	TArray<FPlayerCameraInstance>		CameraInstanceList;
	TArray<FPlayerCameraPresetInfo>		CameraPresetList;

	FPlayerCameraResult					PlayerCameraResult;

	float								ZoomRatio = 1.0f;
	float								PrevGoalZoomRatio = 1.0f;
	float								CurrentGoalZoomRatio = 1.0f;
	float								GoalZoomApplyRatio = 0.0f;

	FRotator							RotationInput;

#ifdef MJ_EDITOR
	bool								IsDebugDraw = false;
#endif
};
