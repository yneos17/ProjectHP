#include "Character/PlayerCharacterController.h"
#include "Character/PlayerCharacter.h"
#include "Camera/PlayerCameraModeManager.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "InputActionValue.h"

APlayerCharacterController::APlayerCharacterController()
{
	PlayerCameraManagerClass = APlayerCameraModeManager::StaticClass();
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();
}

void APlayerCharacterController::OnPossess( APawn* InPawn )
{
    Super::OnPossess( InPawn );
    ApplyCharacterDefaultCameraPreset( InPawn );
}

void APlayerCharacterController::SetPawn( APawn* InPawn )
{
    Super::SetPawn( InPawn );
    ApplyCharacterDefaultCameraPreset( InPawn );
}

void APlayerCharacterController::UpdateRotation( float DeltaTime )
{
    if( APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager ) )
    {
        PlayerCameraModeManager->ApplyDeltaRotation( RotationInput );
        ControlRotation = PlayerCameraModeManager->GetCameraRotation();
    }

    Super::UpdateRotation( DeltaTime );
}

void APlayerCharacterController::OnMouseWheel( const FInputActionValue& Value )
{
    APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager );
    if( PlayerCameraModeManager == nullptr )
        return;

    const float WheelDelta = Value.Get<float>();
    PlayerCameraModeManager->ApplyZoom( WheelDelta );
}

void APlayerCharacterController::ApplyCharacterDefaultCameraPreset( APawn* InPawn )
{
    APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager );
    if( PlayerCameraModeManager == nullptr )
        return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>( InPawn );
    if( PlayerCharacter == nullptr )
        return;

    PlayerCameraModeManager->SetPlayerCameraPreset( EPlayerCameraPriority::CharacterDefault, PlayerCharacter->GetDefaultCameraPreset(), FObjectKey( InPawn ) );
}