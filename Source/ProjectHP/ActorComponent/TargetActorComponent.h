#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "TargetActorComponent.generated.h"

UENUM( BlueprintType )
enum class ETargetSearchType : uint8
{
	Defualt,
	Manual,
	Count,
};

UCLASS( ClassGroup = ( Custom ), meta = ( BlueprintSpawnableComponent ) )
class PROJECTHP_API UTargetActorComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UTargetActorComponent();

	void SetTargetSearchType( const ETargetSearchType TargetSearchType );

private:
	virtual void		TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction ) override;
	virtual void		BeginPlay() override;

	UFUNCTION()
	void				OnOwnerControllerChanged( APawn* Pawn, AController* OldController, AController* NewController );
	void				UpdateTickEnableState();

	void				UpdateTargetForControlActor();
	void				UpdateTargetForControlActor_Default();
	bool				CheckTargetable( AActor* CandidateTargetActor );

private:
	ETargetSearchType			TargetSearchType;
	TWeakObjectPtr<AActor>		TargetActor;
};