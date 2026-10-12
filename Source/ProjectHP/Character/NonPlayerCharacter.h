#pragma once

#include "CoreMinimal.h"
#include "GameCharacter.h"
#include "NonPlayerCharacter.generated.h"

UCLASS()
class PROJECTHP_API ANonPlayerCharacter : public AGameCharacter
{
	GENERATED_BODY()

public:
	ANonPlayerCharacter();

protected:
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaTime) override;
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;

	UFUNCTION( BlueprintCallable, Category = "Patrol Movement" )
	void TestMoveForwardAndBack( const float Distance );

	UFUNCTION( BlueprintCallable, Category = "Patrol Movement" )
	void TestMoveCircle( const float Radius );

protected:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Patrol Movement")
	float MoveSpeed = 300.0f;

private:
	FVector TestMoveStartLocation;
	FVector TestMoveStartForwardVector;
	FVector TestMoveStartRightVector;
};
