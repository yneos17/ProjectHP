#include "Camera/PlayerCameraMode.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "Components/SphereComponent.h"
#include "Components/BoxComponent.h"

APlayerCameraMode::APlayerCameraMode()
{
	__noop;
}

void APlayerCameraMode::Initialize( const FPlayerCameraPresetInfo& InPlayerCameraPresetInfo, const FPlayerCameraResult& PrevPlayerCameraResult )
{
	PlayerCameraPresetInfo = InPlayerCameraPresetInfo;

	CameraYaw = PrevPlayerCameraResult.CameraYaw;
	CameraPitch = PrevPlayerCameraResult.CameraPitch;
}

void APlayerCameraMode::UpdateCameraMode( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	UpdateCameraMode_Internal( PlayerCameraUpdateDesc );
	UpdateCameraMode_Common( PlayerCameraUpdateDesc );
}

void APlayerCameraMode::UpdateCameraMode_Common( const FPlayerCameraUpdateDesc& PlayerCameraUpdateDesc )
{
	CameraPitch = FMath::Clamp( CameraPitch, PlayerCameraPresetInfo.PlayerCameraPreset->PitchMinMax.X, PlayerCameraPresetInfo.PlayerCameraPreset->PitchMinMax.Y );
}

bool APlayerCameraMode::IsSameCamera( const FPlayerCameraPresetInfo& Other ) const
{
	return PlayerCameraPresetInfo == Other;
}

FVector APlayerCameraMode::GetActorCameraLocation( const AActor* Actor, const float HeightRatio ) const
{
	MJ_ASSERT_DEV( "김민진", Actor != nullptr, "GetActorCameraLocation의 인자 Actor가 nullptr입니다. 호출자가 보장해주어야 합니다." );
	return Actor->GetActorLocation() + GetActorHeightOffset( Actor, HeightRatio );
}

FVector APlayerCameraMode::GetActorHeightOffset( const AActor* Actor, const float HeightRatio ) const
{
	MJ_ASSERT_DEV( "김민진", Actor != nullptr, "GetActorCameraLocation의 인자 Actor가 nullptr입니다. 호출자가 보장해주어야 합니다." );

	float HeightDistance = 0.0f;
	if( const ACharacter* CharacterActor = Cast<const ACharacter>( Actor ) )
	{
		HeightDistance = CharacterActor->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	}
	else
	{
		const USceneComponent* Root = Actor->GetRootComponent();
		if( const UCapsuleComponent* Capsule = Cast<const UCapsuleComponent>( Root ) )
			HeightDistance = Capsule->GetScaledCapsuleHalfHeight();
		else if( const USphereComponent* Sphere = Cast<const USphereComponent>( Root ) )
			HeightDistance = Sphere->GetScaledSphereRadius();
		else if( const UBoxComponent* Box = Cast<const UBoxComponent>( Root ) )
			HeightDistance = Box->GetScaledBoxExtent().Z;
		else
			MJ_ASSERT_DEV( "김민진", Actor != nullptr, "Root가 Capsule, Sphere, Box 중에 없습니다. 대응 필요!" );
	}

	return Actor->GetActorUpVector() * ( HeightDistance * HeightRatio );
}