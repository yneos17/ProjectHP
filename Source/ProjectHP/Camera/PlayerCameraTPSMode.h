#pragma once

#include "CoreMinimal.h"
#include "Camera/PlayerCameraMode.h"
#include "PlayerCameraTPSMode.generated.h"

UCLASS( BlueprintType )
class UPlayerCameraTPSPreset : public UPlayerCameraPreset
{
	GENERATED_BODY()
public:
	UPlayerCameraTPSPreset()
	{
		ZoomDistanceRange = FVector2D( 300.0f, 600.0f );
	}

	virtual EPlayerCameraModeType GetPlayerCameraModeType() const override { return EPlayerCameraModeType::TPS; }

public:
	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D RightOffsetRange = FVector2D::Zero();

	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D UpOffsetRange = FVector2D::Zero();

	UPROPERTY( EditAnywhere, Category = "Common" )
	FVector2D ZoomDistanceRange;
};

UCLASS()
class PROJECTHP_API APlayerCameraTPSMode : public APlayerCameraMode
{
	GENERATED_BODY()
public:
	APlayerCameraTPSMode();

public:
	virtual EPlayerCameraModeType GetPlayerCameraModeType() const override { return EPlayerCameraModeType::TPS; }
	virtual FPlayerCameraResult GetPlayerCameraModeResult() const override;

protected:
	virtual void UpdateCameraMode_Internal( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc ) override;

private:
	void UpdateCameraMode_CalcRightUpOffsetInWorld( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc );

public:
};
