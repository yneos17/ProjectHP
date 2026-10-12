#include "PlayerCharacter.h"
#include "ActorComponent/TargetActorComponent.h"
#include "EnhancedInputComponent.h"
#include "EnhancedInputSubsystems.h"
#include "InputActionValue.h"
#include "PlayerCharacterController.h"

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

			//MJTODO
			if( IA_LockOn )
				EnhancedInputComponent->BindAction( IA_Zoom, ETriggerEvent::Triggered, PC, &APlayerCharacterController::OnMouseWheel );
		}
	}
}

void APlayerCharacter::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacter::ToggleLockOn( AActor* EnemyActor )
{
	// MJTODO

	//UGameplayCameraComponent* CameraComp = GetGameplayCameraComponent();
	//if( !CameraComp ) return;
	//
	//if( EnemyActor )
	//{
	//    // 1. VariableTable에 Pivot과 Target Actor 전달
	//    FCameraVariableTable& VarTable = CameraComp->GetVariableTable();
	//    VarTable.SetObjectVariable( FCameraVariableName( "PivotActor" ), this );
	//    VarTable.SetObjectVariable( FCameraVariableName( "TargetActor" ), EnemyActor );
	//
	//    // 2. Lock-On Camera Asset 활성화 (CA_LockOnTest 에셋 할당 필요)
	//    if( LockOnCameraAsset )
	//    {
	//        CameraComp->ActivateCameraAsset( LockOnCameraAsset );
	//    }
	//}
	//else
	//{
	//    // 락온 해제 시 기본 카메라로 복귀
	//    CameraComp->DeactivateCameraAsset( LockOnCameraAsset );
	//}
}