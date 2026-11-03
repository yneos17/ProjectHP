#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "PlayerCharacter.generated.h"

class UTargetActorComponent;
class UPlayerCameraPreset;
class UInputAction;

UCLASS()
class PROJECTHP_API APlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	APlayerCharacter();

public:	
	virtual void Tick( float DeltaTime ) override;
	virtual void SetupPlayerInputComponent( class UInputComponent* PlayerInputComponent ) override;

	UTargetActorComponent* GetTargetActorComponent() { return TargetComponent; }
	const UTargetActorComponent* GetTargetActorComponent() const { return TargetComponent; }

	void ToggleLockOn();

	UFUNCTION(BlueprintCallable)
	UPlayerCameraPreset* GetDefaultCameraPreset() const { return DefaultCameraPreset; }

protected:
	virtual void BeginPlay() override;

protected:
	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite )
	TObjectPtr<UPlayerCameraPreset> DefaultCameraPreset;

	UPROPERTY( EditDefaultsOnly, BlueprintReadWrite )
	TObjectPtr<UPlayerCameraPreset> LockOnCameraPreset;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_Zoom;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Input")
	TObjectPtr<UInputAction> IA_LockOn;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Components", meta = (AllowPrivateAccess = "true"))
	TObjectPtr<UTargetActorComponent> TargetComponent;

private:
	bool					bTargetLockOn = false;
};
