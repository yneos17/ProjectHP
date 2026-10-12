#include "Character/AnimNotifyState/AnimNotifyState_CameraPreset.h"
#include "Camera/PlayerCameraModeManager.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Components/SkeletalMeshComponent.h"

UAnimNotifyState_CameraPreset::UAnimNotifyState_CameraPreset()
{
	__noop;
}

void UAnimNotifyState_CameraPreset::NotifyBegin( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyBegin( MeshComp, Animation, TotalDuration, EventReference );

	if( MeshComp == nullptr )
		return;

	APawn* OwnerPawn = Cast<APawn>( MeshComp->GetOwner() );
	if( OwnerPawn == nullptr )
		return;

	APlayerController* PC = Cast<APlayerController>( OwnerPawn->GetController() );
	if( PC == nullptr )
		return;

	APlayerCameraModeManager* CameraManager = Cast<APlayerCameraModeManager>( PC->PlayerCameraManager );
	if( CameraManager == nullptr )
		return;

	CameraManager->SetPlayerCameraPreset( EPlayerCameraPriority::AnimNotifyState, PlayerCameraPreset, FObjectKey( MeshComp->GetOwner() ) );
}

void UAnimNotifyState_CameraPreset::NotifyEnd( USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference )
{
	Super::NotifyEnd( MeshComp, Animation, EventReference );

	if( MeshComp == nullptr )
		return;

	APawn* OwnerPawn = Cast<APawn>( MeshComp->GetOwner() );
	if( OwnerPawn == nullptr )
		return;

	APlayerController* PC = Cast<APlayerController>( OwnerPawn->GetController() );
	if( PC == nullptr )
		return;

	APlayerCameraModeManager* CameraManager = Cast<APlayerCameraModeManager>( PC->PlayerCameraManager );
	if( CameraManager == nullptr )
		return;

	CameraManager->ClearPlayerCameraPreset( EPlayerCameraPriority::AnimNotifyState );
}