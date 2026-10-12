#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "AnimNotifyState_TargetSearch.generated.h"

enum class ETargetSearchType : uint8;

UCLASS()
class PROJECTHP_API UAnimNotifyState_TargetSearch : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	virtual void NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference ) override;
	virtual void NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference ) override;

protected:
	UPROPERTY( EditAnywhere, BlueprintReadWrite, Category = "Camera" )
	ETargetSearchType TargetSearchType;
};
