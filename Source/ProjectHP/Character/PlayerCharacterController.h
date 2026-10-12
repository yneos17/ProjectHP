#pragma once

#include "CoreMinimal.h"
#include "GameFramework/PlayerController.h"
#include "PlayerCharacterController.generated.h"

struct FInputActionValue;

UCLASS()
class PROJECTHP_API APlayerCharacterController : public APlayerController
{
	GENERATED_BODY()

public:
	APlayerCharacterController();
	virtual void BeginPlay() override;
	virtual void SetupInputComponent() override;

	virtual void OnPossess( APawn* InPawn ) override;
	virtual void SetPawn( APawn* InPawn ) override;

	virtual void UpdateRotation( float DeltaTime ) override;

	void OnMouseWheel( const FInputActionValue& Value );

private:
	void ApplyCharacterDefaultCameraPreset( APawn* InPawn );

	//MJTEST MJTODO
	void OnSpawnAndBlendCamera();
};
