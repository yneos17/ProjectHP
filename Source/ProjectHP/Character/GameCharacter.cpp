#include "Character/GameCharacter.h"
#include "Components/CapsuleComponent.h"

FVector AGameCharacter::GetActorTopLocation() const
{
	FVector ActorBottomLocation = GetActorLocation();

	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ActorBottomLocation.Z += HalfHeight;

	return ActorBottomLocation;
}

FVector AGameCharacter::GetActorBottomLocation() const
{
	FVector ActorBottomLocation = GetActorLocation();

	const float HalfHeight = GetCapsuleComponent()->GetScaledCapsuleHalfHeight();
	ActorBottomLocation.Z -= HalfHeight;

	return ActorBottomLocation;
}