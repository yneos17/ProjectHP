#include "Editor/EditorPlayerCharacterController.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Pawn.h"
#include "Character/CharacterInfo.h"
#include "Camera/PlayerCameraPresetManager.h"
#include "Camera/PlayerCameraModeManager.h"
#include "Engine/HitResult.h"
#include "CollisionQueryParams.h"

void AEditorPlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void AEditorPlayerCharacterController::Create( const FString& CategoryName, const FString& Key, const int32 Count )
{
	if( CategoryName == TEXT( "Character" ) )
		CreateCharacter( Key, Count );
}

void AEditorPlayerCharacterController::CreateCharacter( const FString& Key, const int32 Count )
{
	UCharacterInfoManager* InfoMgr = UCharacterInfoManager::Get();
	if( InfoMgr == nullptr )
		return;

	const FCharacterInfo* CharacterInfo = nullptr;
	if( Key.IsNumeric() )
	{
		int32 KeyIndex = FCString::Atoi( *Key );
		CharacterInfo = InfoMgr->GetCharacterInfo( KeyIndex );
	}
	else
	{
		CharacterInfo = InfoMgr->GetCharacterInfo( FName( Key ) );
	}

	if( CharacterInfo == nullptr )
		return;

	UWorld* World = GetWorld();

	for( int32 index = 0; index < Count; ++index )
	{
		FVector CreateLocation;
		if( index == 0 )
		{
			CreateLocation = GetPawn()->GetActorLocation() + ( GetPawn()->GetActorForwardVector() * 300.f );
		}
		else
		{
			FVector RandomOffset = FMath::VRand() * FMath::FRandRange( 300.f, 1000.f );
			RandomOffset.Z = 0.f;

			CreateLocation = GetPawn()->GetActorLocation() + RandomOffset;
		}

		FVector TraceStart = CreateLocation + FVector( 0.f, 0.f, 1000.f );
		FVector TraceEnd = TraceStart - FVector( 0.f, 0.f, 2000.f );

		FHitResult HitResult;
		FCollisionQueryParams Params;
		Params.AddIgnoredActor( GetPawn() );

		if( World->LineTraceSingleByChannel( HitResult, TraceStart, TraceEnd, ECC_Visibility, Params ) )
		{
			CreateLocation = HitResult.Location;
		}

		FRotator SpawnRotation = FRotator::ZeroRotator;

		if( CharacterInfo->CharacterClass != nullptr )
		{
			World->SpawnActor<AActor>( CharacterInfo->CharacterClass, CreateLocation, SpawnRotation );
		}
	}
}

void AEditorPlayerCharacterController::Reload( const FString& CategoryName )
{
	if( CategoryName == TEXT( "CharacterInfo" ) )
	{
		UCharacterInfoManager::Get()->LoadCharacterInfoFromXML();
	}
}

void AEditorPlayerCharacterController::DebugDraw( const FString& CategoryName )
{
	if( CategoryName == TEXT( "PlayerCamera" ) )
	{
		APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager );
		PlayerCameraModeManager->ToggleDebugDrawSwitch();
	}
}