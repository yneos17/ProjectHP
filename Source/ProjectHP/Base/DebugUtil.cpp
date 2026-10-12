#include "DebugUtil.h"

#ifdef MJ_EDITOR
#include "DrawDebugHelpers.h"
#include "Kismet/GameplayStatics.h"

void DrawDebugStringCenter( const UWorld* InWorld, FVector const& TextLocation, const FString& Text, class AActor* TestBaseActor, FColor const& TextColor, float Duration, bool bDrawShadow, float FontScale )
{
	if( InWorld == nullptr )
		return;

	APlayerController* PlayerController = UGameplayStatics::GetPlayerController( nullptr, 0 );
	if( PlayerController == nullptr || PlayerController->PlayerCameraManager == nullptr )
		return;

	constexpr float BaseCharWidthOffset = 3.333f;
	const float HalfTextWidth = ( Text.Len() * BaseCharWidthOffset * FontScale ) * 0.5f;

	const FVector CameraRight = PlayerController->PlayerCameraManager->GetCameraRotation().RotateVector( FVector::RightVector );

	const FVector AdjustedTextLocation = TextLocation - ( CameraRight * HalfTextWidth );

	DrawDebugString( InWorld, AdjustedTextLocation, Text, TestBaseActor, TextColor, Duration, bDrawShadow, FontScale );
}
#endif