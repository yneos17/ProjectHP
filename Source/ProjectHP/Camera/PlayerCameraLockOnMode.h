#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraMode.h"
#include "PlayerCameraLockOnMode.generated.h"

UCLASS( BlueprintType )
class UPlayerCameraLockOnPreset : public UPlayerCameraPreset
{
	GENERATED_BODY()
public:
	UPlayerCameraLockOnPreset()
	{
		ClampPointHeightRatioRange = FVector2D( -0.5f, 0.8f );
		ClampCameraDistanceRange = FVector2D( 300.0f, 10000.0f );
	}

	virtual EPlayerCameraModeType GetPlayerCameraModeType() const override { return EPlayerCameraModeType::LockOn; }

public:
	UPROPERTY( EditAnywhere, Category = "Common" )
	float CameraCenterRatio = 0.5f;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float ScreenClampRatio = 0.9f;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float DistanceLerpSpeed = 10.0f;

	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D ClampPointHeightRatioRange;

	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D ClampCameraDistanceRange;

	UPROPERTY( EditAnywhere, Category = "Common" )
	bool AutoRotate = true;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float OffsetYaw = 0.0f;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float OffsetPitch = 0.0f;

	UPROPERTY( EditAnywhere, Category = "Common" )
	bool IsClampPitch = true;

	UPROPERTY( EditAnywhere, Category = "Common" )
	bool FixedTarget = false;
};

UCLASS()
class PROJECTHP_API APlayerCameraLockOnMode : public APlayerCameraMode
{
	GENERATED_BODY()

public:
	virtual EPlayerCameraModeType GetPlayerCameraModeType() const override { return EPlayerCameraModeType::LockOn; }
	virtual FPlayerCameraResult GetPlayerCameraModeResult() const override;
	void Initialize( const FPlayerCameraPresetInfo& InPlayerCameraPresetInfo, const FPlayerCameraResult& PrevPlayerCameraResult );

#ifdef MJ_EDITOR
	virtual void DebugDraw_PlayerCameraMode() override;
#endif

protected:
	virtual void UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc ) override;

private:
	bool UpdateCameraMode_GetTargetActorList( TArray<const AActor*>& OutTargetActorList ) const;
	void UpdateCameraMode_UpdateTargetListCenterLocation( const UPlayerCameraLockOnPreset* CastedPreset, const TArray<const AActor*>& OutTargetActorList );
	void UpdateCameraMode_Rotate( const UPlayerCameraLockOnPreset* CastedPreset, const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );
	void UpdateCameraMode_AutoRotate( const UPlayerCameraLockOnPreset* CastedPreset );
	float UpdateCameraMode_GetNeedMinCameraDistance( const UPlayerCameraLockOnPreset* CastedPreset, const AActor* PivotActor, const TArray<const AActor*>& TargetActorList );
	float UpdateCameraMode_GetNeedMinCameraDistancePerPoint( const FVector& Point );

public:
	FVector TargetListCenterLocation = FVector::ZeroVector;
};