#include "PlayerCharacter.h"
#include "ActorComponent/TargetActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerCharacterController.h"
#include "Camera/PlayerCameraModeManager.h"

#include "Animation/AnimInstance.h"
#include "Animation/AnimMontage.h"
#include "Animation/BlendSpace.h"
#include "Animation/AnimClassInterface.h"
#include "Animation/AnimSequence.h"

APlayerCharacter::APlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	{
		TargetComponent = CreateDefaultSubobject<UTargetActorComponent>( TEXT( "TargetComponent" ) );
		if( TargetComponent )
		{
			TargetComponent->PrimaryComponentTick.bCanEverTick = true;
			TargetComponent->PrimaryComponentTick.bStartWithTickEnabled = true;
		}
	}
}

void APlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick( DeltaTime );
}

void APlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

	if( UEnhancedInputComponent* EnhancedInputComponent = Cast<UEnhancedInputComponent>( PlayerInputComponent ) )
	{
		if( APlayerCharacterController* PC = Cast<APlayerCharacterController>( GetController() ) )
		{
			if( IA_Zoom )
				EnhancedInputComponent->BindAction( IA_Zoom, ETriggerEvent::Triggered, PC, &APlayerCharacterController::OnMouseWheel );

			if( IA_LockOn )
				EnhancedInputComponent->BindAction( IA_LockOn, ETriggerEvent::Started, this, &APlayerCharacter::ToggleLockOn );
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::ToggleLockOn()
{
	APlayerCharacterController* PlayerCharacterController = Cast<APlayerCharacterController>( GetController() );
	if( PlayerCharacterController == nullptr )
		return;

	APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCharacterController->PlayerCameraManager );
	if( PlayerCharacterController == nullptr )
		return;

	bTargetLockOn = !bTargetLockOn;

	if( bTargetLockOn )
	{
		TSortedArray<FObjectKey> TargetActorKeyList;
		TargetActorKeyList.Push( TargetComponent->GetTargetActorKey() );
		PlayerCameraModeManager->SetPlayerCameraPreset( EPlayerCameraPriority::System, LockOnCameraPreset, FObjectKey( this ), TargetActorKeyList );
	}
	else
	{
		PlayerCameraModeManager->ClearPlayerCameraPreset( EPlayerCameraPriority::System );
	}
}