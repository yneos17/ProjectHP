#pragma once

#include "CoreMinimal.h"
#include "Subsystems/EngineSubsystem.h"
#include "PlayerCameraPresetManager.generated.h"

class FXmlAttribute;
struct FPlayerCameraPreset;
enum class EPlayerCameraModeType : uint8;

UCLASS()
class PROJECTHP_API UPlayerCameraPresetManager : public UEngineSubsystem
{
	GENERATED_BODY()
public:
	static UPlayerCameraPresetManager* Get();

public:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;

	//MJTODO 필요없으면 삭제
	const UPlayerCameraPreset* GetPlayerCameraPreset( const FName PresetName ) const;

private:
	UPROPERTY(EditAnywhere, Category = "Camera Presets")
	TArray<TObjectPtr<UPlayerCameraPreset>> PlayerCameraPresetArray;

	UPROPERTY()
	TMap<FName, TObjectPtr<UPlayerCameraPreset>> PlayerCameraPresetMap;
};
