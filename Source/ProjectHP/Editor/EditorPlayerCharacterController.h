#pragma once

#include "CoreMinimal.h"
#include "Character/PlayerCharacterController.h"
#include "EditorPlayerCharacterController.generated.h"

UCLASS()
class PROJECTHP_API AEditorPlayerCharacterController : public APlayerCharacterController
{
    GENERATED_BODY()

public:
    virtual void BeginPlay() override;

    UFUNCTION(Exec)
    void Create( const FString& CategoryName, const FString& Key, const int32 Count = 1 );

    UFUNCTION(Exec)
    void Reload( const FString& CategoryName );

    UFUNCTION(Exec)
    void DebugDraw( const FString& CategoryName );

private:
    void CreateCharacter( const FString& Key, const int32 Count = 1 );
};