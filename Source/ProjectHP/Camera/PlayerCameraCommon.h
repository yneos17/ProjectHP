#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectKey.h"
#include "PlayerCameraCommon.generated.h"

class UPlayerCameraPreset;

enum class EPlayerCameraPriority : uint8
{
	CharacterDefault,
	AnimNotifyState,
	System,
	Count,
};

struct FPlayerCameraPresetInfo
{
	bool operator==( const FPlayerCameraPresetInfo& Other ) const
	{
		return PlayerCameraPreset == Other.PlayerCameraPreset &&
			Priority == Other.Priority &&
			PivotActorKey == Other.PivotActorKey &&
			TargetActorKeyList == Other.TargetActorKeyList;
	}

	const UPlayerCameraPreset*	PlayerCameraPreset = nullptr;
	EPlayerCameraPriority		Priority = EPlayerCameraPriority::Count;
	FObjectKey					PivotActorKey;
	TSortedArray<FObjectKey>	TargetActorKeyList;
};

USTRUCT( BlueprintType )
struct FPlayerCameraBlendParam
{
	GENERATED_BODY()

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Blend", meta = ( DisplayName = "블렌드 인 시간(초)", ClampMin = "0.0" ) )
	float BlendInTime = 0.5f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Blend", meta = ( DisplayName = "블렌드 인 이징 함수" ) )
	TEnumAsByte<EEasingFunc::Type> BlendInEasingFunction = EEasingFunc::EaseInOut;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Blend", meta = ( DisplayName = "블렌드 아웃 시간(초)", ClampMin = "0.0" ) )
	float BlendOutTime = 0.5f;

	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera Blend", meta = ( DisplayName = "블렌드 아웃 이징 함수" ) )
	TEnumAsByte<EEasingFunc::Type> BlendOutEasingFunction = EEasingFunc::EaseInOut;
};
