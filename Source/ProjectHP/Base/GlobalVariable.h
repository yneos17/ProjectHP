#pragma once

#include "CoreMinimal.h"
#include "Engine/DeveloperSettingsBackedByCVars.h"
#include "GlobalVariable.generated.h"

UCLASS()
class PROJECTHP_API UGlobalVariable : public UDeveloperSettingsBackedByCVars
{
	GENERATED_BODY()

public:
	static const UGlobalVariable* Get() { return GetDefault<UGlobalVariable>(); }

public:
	UPROPERTY( EditAnywhere, Category = "Targeting" )
	float Targeting_DefaultMaxTargetSearchDistance = 3000.0f;
	UPROPERTY( EditAnywhere, Category = "Targeting" )
	FVector2D Targeting_DistanceCheckScoreDistanceRange = FVector2D( 150.0, 3000.0f );
	UPROPERTY( EditAnywhere, Category = "Targeting" )
	float Targeting_DistanceMaxScore = 30.0f;
	UPROPERTY( EditAnywhere, Category = "Targeting" )
	FVector2D Targeting_AngleCheckScoreAngleDegRange = FVector2D( 30.0f, 150.0f );
	UPROPERTY( EditAnywhere, Category = "Targeting" )
	float Targeting_AngleMaxScore = 30.0f;
};
