#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "Engine/DataAsset.h"
#include "PlayerCameraCommon.h"
#include "PlayerCameraModeManager.h"
#include "PlayerCameraMode.generated.h"

struct FPlayerCameraUpdateDesc;

UENUM( BlueprintType )
enum class EPlayerCameraModeType : uint8
{
	TPS,
	LockOn,
	Count,
};

UCLASS( Abstract, BlueprintType )
class UPlayerCameraPreset : public UDataAsset
{
	GENERATED_BODY()
public:
	UPlayerCameraPreset()
	{
		PivotActorHeightRatio = 0.8f;
		TargetActorHeightRatio = 0.8f;
		CameraFovDeg = 40.0f;
		PitchMinMax = FVector2D( -70.0f, 70.0f );
	};

public:
	UFUNCTION( BlueprintCallable )
	virtual EPlayerCameraModeType GetPlayerCameraModeType() const PURE_VIRTUAL( UPlayerCameraPreset::GetPlayerCameraModeType, return EPlayerCameraModeType::Count; );

public:
	UPROPERTY( EditAnywhere, Category = "Common" )
	FPlayerCameraBlendParam PlayerCameraBlendParam;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float PivotActorHeightRatio;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float TargetActorHeightRatio;

	UPROPERTY( EditAnywhere, Category = "Common" )
	float CameraFovDeg;

	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D PitchMinMax;

	UPROPERTY( EditAnywhere, Category = "Common", meta = ( DisplayName = "회전 속도", ToolTip = "회전 기능을 사용중인 경우, 매 프레임 \"해당 값 * s\" 의 비율로 목표 회전값에 가까워집니다." ) )
	float RotationLerpSpeed = 10.0f;

	UPROPERTY( EditAnywhere, Category = "Common", meta = ( DisplayName = "회전 가속도 (Deg/s)", ToolTip = "회전 기능을 사용중인 경우, 카메라가 목표 회전각에 도달할 때의 회전 가속 속도입니다. (초당 회전 각도: Deg/s)" ) )
	float RotationAccSpeed = 450.0f;
};

UCLASS()
class PROJECTHP_API APlayerCameraMode : public AActor
{
	GENERATED_BODY()
public:
	APlayerCameraMode();

public:
	virtual EPlayerCameraModeType GetPlayerCameraModeType() const PURE_VIRTUAL( APlayerCameraMode::GetPlayerCameraModeType, return EPlayerCameraModeType::Count; );
	virtual FPlayerCameraResult GetPlayerCameraModeResult() const PURE_VIRTUAL( APlayerCameraMode::GetPlayerCameraModeResult, return FPlayerCameraResult(); );

	void		Initialize( const FPlayerCameraPresetInfo& InPlayerCameraPresetInfo, const FPlayerCameraResult& PrevPlayerCameraResult );
	void		UpdateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );

	bool		IsSameCamera( const FPlayerCameraPresetInfo& Other ) const;
	// bool		IsAutoRotate();
	bool		IsValid() const { return bIsValid; }

	const FPlayerCameraPresetInfo& GetPlayerCameraPresetInfo() const { return PlayerCameraPresetInfo; }
	FVector		GetActorCameraLocation( const AActor* Actor, const float HeightRatio ) const;
	FVector		GetActorHeightOffset( const AActor* Actor, const float HeightRatio ) const;

protected:
	virtual void UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc ) PURE_VIRTUAL( APlayerCameraMode::UpdateCameraMode_Internal, );

private:
	void		UpdateCameraMode_Common( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );

protected:
	FPlayerCameraPresetInfo		PlayerCameraPresetInfo;
	float						CameraYaw = 0.0f;
	float						CameraPitch = 0.0f;
	float						ZoomDistance = 0.0f;
	float						CameraFovDeg = 0.0f;

	FVector						RightUpOffsetInWorld = FVector::ZeroVector;
	FVector						PivotLocation = FVector::ZeroVector;
	FVector						LootAtLocation = FVector::ZeroVector;
	FVector						CameraLocation = FVector::ZeroVector;

	bool						bIsValid = true;
};
