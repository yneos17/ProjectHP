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
		ClampPointHeightRatioRange = FVector2D( -0.8f, 0.8f );
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
};

UCLASS()
class PROJECTHP_API APlayerCameraLockOnMode : public APlayerCameraMode
{
	GENERATED_BODY()

public:
	virtual EPlayerCameraModeType GetPlayerCameraModeType() const override { return EPlayerCameraModeType::LockOn; }
	virtual FPlayerCameraResult GetPlayerCameraModeResult() const override;

protected:
	virtual void UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc ) override;

private:
	float UpdateCameraMode_GetNeedMinCameraDistance( const UPlayerCameraLockOnPreset* CastedPreset, const AActor* PivotActor, const TArray<const AActor*>& TargetActorList );
	float UpdateCameraMode_GetNeedMinCameraDistancePerPoint( const FVector& Point );

public:

};