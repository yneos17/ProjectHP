#include "ClientActorManager.h"
#include "Engine/World.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Pawn.h"

void UClientActorManager::Initialize( FSubsystemCollectionBase& Collection )
{
	UWorld* World = GetWorld();
	if( World == nullptr )
	{
		MJ_ASSERT_DEV( "김민진", false, "UClientActorManager의 Initialize에 실패했습니다." );
		return;
	}

	FOnActorSpawned::FDelegate SpawnDelegate = FOnActorSpawned::FDelegate::CreateUObject( this, &UClientActorManager::OnActorSpawned );
	ActorSpawnHandle = World->AddOnActorSpawnedHandler( SpawnDelegate );

	FOnActorSpawned::FDelegate DestroyDelegate = FOnActorSpawned::FDelegate::CreateUObject( this, &UClientActorManager::OnActorDestroyed );
	ActorDestroyHandle = World->AddOnActorDestroyedHandler( DestroyDelegate );
}

void UClientActorManager::Deinitialize()
{
	UWorld* World = GetWorld();
	if( World == nullptr )
	{
		MJ_ASSERT_DEV( "김민진", false, "UClientActorManager의 Initialize에 실패했습니다." );
		return;
	}

	World->RemoveOnActorSpawnedHandler( ActorSpawnHandle );
	World->RemoveOnActorDestroyededHandler( ActorDestroyHandle );

	TArray<AActor*> DestroyActorList;
	CharacterActorMap.GenerateValueArray( DestroyActorList );
	CharacterActorMap.Empty();

	for( AActor* TargetActor : DestroyActorList )
	{
		if( IsValid( TargetActor ) )
		{
			TargetActor->Destroy();
		}
	}

	Super::Deinitialize();
}

AActor* UClientActorManager::CreateActor( const FSpawnInfo& SpawnInfo )
{
	if( SpawnInfo.ActorClass == nullptr )
		return nullptr;

	UWorld* World = GetWorld();
	if( World == nullptr )
		return nullptr;

	FActorSpawnParameters SpawnParams;
	SpawnParams.SpawnCollisionHandlingOverride = SpawnInfo.SpawnCollisionHandlingOverride;

	AActor* NewActor = World->SpawnActor<AActor>( SpawnInfo.ActorClass, SpawnInfo.SpawnLocation, SpawnInfo.SpawnRotation, SpawnParams );
	if( NewActor == nullptr )
		return nullptr;

	return NewActor;
}

AActor* UClientActorManager::GetActor( const FObjectKey& ObjectKey )
{
	if( AActor** FoundActor = CharacterActorMap.Find( ObjectKey ) )
		return *FoundActor;

	return nullptr;
}

AActor* UClientActorManager::GetSelfActor()
{
	return UGameplayStatics::GetPlayerPawn( GetWorld(), 0 );
}

void UClientActorManager::OnActorSpawned( AActor* SpawnedActor )
{
	FObjectKey Key( SpawnedActor );
	CharacterActorMap.Add( Key, SpawnedActor );
}

void UClientActorManager::OnActorDestroyed( AActor* DestroyedActor )
{
	if( DestroyedActor == nullptr )
		return;

	FObjectKey Key( DestroyedActor );
	CharacterActorMap.Remove( Key );
}