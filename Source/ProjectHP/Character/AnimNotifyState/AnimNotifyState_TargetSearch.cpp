#include "Character/AnimNotifyState/AnimNotifyState_TargetSearch.h"
#include "Camera/PlayerCameraModeManager.h"
#include "GameFramework/PlayerController.h"
#include "Components/SkeletalMeshComponent.h"
#include "PlayerCharacter.h"
#include "TargetActorComponent.h"

void UAnimNotifyState_TargetSearch::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( MeshComp == nullptr )
		return;

	APlayerCharacter* OwnerPlayerCharacter = Cast<APlayerCharacter>( MeshComp->GetOwner() );
	if( OwnerPlayerCharacter == nullptr )
		return;

	UTargetActorComponent* TargetActorComponent = OwnerPlayerCharacter->FindComponentByClass<UTargetActorComponent>();
	if( TargetActorComponent == nullptr )
		return;

	TargetActorComponent->SetTargetSearchType( TargetSearchType );
}

void UAnimNotifyState_TargetSearch::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( MeshComp == nullptr )
		return;

	APlayerCharacter* OwnerPlayerCharacter = Cast<APlayerCharacter>( MeshComp->GetOwner() );
	if( OwnerPlayerCharacter == nullptr )
		return;

	UTargetActorComponent* TargetActorComponent = OwnerPlayerCharacter->FindComponentByClass<UTargetActorComponent>();
	if( TargetActorComponent == nullptr )
		return;

	TargetActorComponent->SetTargetSearchType( ETargetSearchType::Defualt );
}