#pragma once

#include "CoreMinimal.h"
#include "Subsystems/WorldSubsystem.h"
#include "UObject/ObjectKey.h"
#include "Engine/EngineTypes.h"
#include "ClientActorManager.generated.h"

class PlayerCharacter;
enum class ESpawnActorCollisionHandlingMethod : uint8;

struct FSpawnInfo
{
	TSubclassOf<AActor> ActorClass;
	FVector SpawnLocation = FVector::ZeroVector;
	FRotator SpawnRotation = FRotator::ZeroRotator;
	ESpawnActorCollisionHandlingMethod SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;
};

UCLASS()
class PROJECTHP_API UClientActorManager : public UWorldSubsystem
{
	GENERATED_BODY()

public:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	virtual void Deinitialize() override;

	AActor* CreateActor( const FSpawnInfo& SpawnInfo );
	AActor* GetActor( const FObjectKey& ObjectKey );
	AActor* GetSelfActor();

	void OnActorSpawned( AActor* SpawnedActor );
	void OnActorDestroyed( AActor* DestroyedActor );

private:
	TMap<FObjectKey, AActor*>	CharacterActorMap;

	FDelegateHandle ActorSpawnHandle;
	FDelegateHandle ActorDestroyHandle;
};
