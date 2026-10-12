#include "PlayerCameraPresetManager.h"
#include "PlayerCameraMode.h"
#include "PlayerCameraTPSMode.h"
#include "PlayerCameraLockOnMode.h"
#include "Engine/AssetManager.h"

static const FPrimaryAssetType PlayerCameraPresetAssetType = FPrimaryAssetType( "PlayerCameraPreset" );
static const FString PlayerCameraPresetPath = TEXT( "StaticInfo/PlayerCameraPreset/PlayerCameraPreset.xml" );

UPlayerCameraPresetManager* UPlayerCameraPresetManager::Get()
{
	if( GEngine == nullptr )
		return nullptr;

	return GEngine->GetEngineSubsystem<UPlayerCameraPresetManager>();
}

void UPlayerCameraPresetManager::Initialize( FSubsystemCollectionBase& Collection )
{
	PlayerCameraPresetMap.Empty();
	for( UPlayerCameraPreset* PlayerCameraPreset : PlayerCameraPresetArray )
	{
		if( PlayerCameraPreset && PlayerCameraPreset->GetFName().IsNone() == false )
			PlayerCameraPresetMap.Add( PlayerCameraPreset->GetFName(), PlayerCameraPreset );
	}
}

const UPlayerCameraPreset* UPlayerCameraPresetManager::GetPlayerCameraPreset( const FName PresetName ) const
{
	if( PlayerCameraPresetMap.Contains( PresetName ) == false )
		return nullptr;

	return PlayerCameraPresetMap[PresetName].Get();
}