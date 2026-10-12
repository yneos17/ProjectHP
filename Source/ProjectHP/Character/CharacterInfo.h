#pragma once

#include "CoreMinimal.h"
#include "Engine/DataTable.h"
#include "Subsystems/GameInstanceSubsystem.h"
#include "CharacterInfo.generated.h"

USTRUCT(BlueprintType)
struct PROJECTHP_API FCharacterInfo : public FTableRowBase
{
	GENERATED_BODY()

public:
	FCharacterInfo();

public:
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	FName StringKey;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
	int32 Key;
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Base")
    TSubclassOf<AActor> CharacterClass;
};

UCLASS()
class PROJECTHP_API UCharacterInfoManager : public UEngineSubsystem
{
	GENERATED_BODY()

public:
	static UCharacterInfoManager* Get();

public:
	virtual void Initialize( FSubsystemCollectionBase& Collection ) override;
	const FCharacterInfo* GetCharacterInfo( const int32 Key ) const;
	const FCharacterInfo* GetCharacterInfo( const FName& StringKey ) const;
	bool LoadCharacterInfoFromXML();

private:
	UPROPERTY()
    TMap<int32, FCharacterInfo> CharacterInfoMapByIndex;

	UPROPERTY()
    TMap<FName, FCharacterInfo> CharacterInfoMapByStringKey;
};