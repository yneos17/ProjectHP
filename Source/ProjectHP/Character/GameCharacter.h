#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Character.h"
#include "GameCharacter.generated.h"

UCLASS()
class PROJECTHP_API AGameCharacter : public ACharacter
{
	GENERATED_BODY()

public:
	AGameCharacter() = default;

	FVector GetActorTopLocation() const;
	FVector GetActorBottomLocation() const;
};
