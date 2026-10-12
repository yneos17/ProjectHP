#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_CameraPreset.generated.h"

class UPlayerCameraPreset;

UCLASS(meta = (DisplayName = "Player Camera Preset Track"))
class PROJECTHP_API UAnimNotifyState_CameraPreset : public UAnimNotifyState
{
	GENERATED_BODY()
public:
	UAnimNotifyState_CameraPreset();

	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Camera")
	TObjectPtr<UPlayerCameraPreset> PlayerCameraPreset;
};
