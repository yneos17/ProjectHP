#include "TargetActorComponent.h"
#include "GameFramework/Actor.h"
#include "Kismet/KismetSystemLibrary.h"
#include "GameFramework/Character.h"
#include "Components/CapsuleComponent.h"
#include "GameCharacter.h"

#ifdef MJ_EDITOR
#include "DrawDebugHelpers.h"
#endif

UTargetActorComponent::UTargetActorComponent()
{
	TargetSearchType = ETargetSearchType::Defualt;

	PrimaryComponentTick.bCanEverTick = true;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UTargetActorComponent::BeginPlay()
{
	Super::BeginPlay();

	if( APawn* OwnerPawn = Cast<APawn>( GetOwner() ) )
	{
		OwnerPawn->ReceiveControllerChangedDelegate.AddDynamic( this, &UTargetActorComponent::OnOwnerControllerChanged );
		UpdateTickEnableState();
	}
}

void UTargetActorComponent::OnOwnerControllerChanged( APawn* Pawn, AController* OldController, AController* NewController )
{
	UpdateTickEnableState();
}

void UTargetActorComponent::UpdateTickEnableState()
{
	APawn* OwnerPawn = Cast<APawn>( GetOwner() );
	if( OwnerPawn && OwnerPawn->IsPlayerControlled() )
		SetComponentTickEnabled( true );
}

void UTargetActorComponent::TickComponent( float DeltaTime, ELevelTick TickType, FActorComponentTickFunction* ThisTickFunction )
{
	Super::TickComponent( DeltaTime, TickType, ThisTickFunction );

	const APawn* OwnerPawn = Cast<APawn>( GetOwner() );
	if( OwnerPawn && OwnerPawn->IsPlayerControlled() )
		UpdateTargetForControlActor();
}

void UTargetActorComponent::SetTargetSearchType( const ETargetSearchType NewTargetSearchType )
{
	TargetSearchType = NewTargetSearchType;
}

void UTargetActorComponent::UpdateTargetForControlActor()
{
	switch( TargetSearchType )
	{
		case ETargetSearchType::Defualt:
		{
			UpdateTargetForControlActor_Default();
		}
		break;
		case ETargetSearchType::Manual:
		{
			//MJTODO
		}
		break;
		default:
		{
			MJ_ASSERT_DEV( "김민진", false, "ETargetSearchType[%#]이 비정상입니다. ", static_cast<uint32>( TargetSearchType ) );
		}
		break;
	}
}

void UTargetActorComponent::UpdateTargetForControlActor_Default()
{
	UWorld* World = GetWorld();
	if( World == nullptr )
		return;

	TArray<TEnumAsByte<EObjectTypeQuery>> ObjectTypes;
	ObjectTypes.Add( UEngineTypes::ConvertToObjectType( ECollisionChannel::ECC_Pawn ) );

	TArray<AActor*> IgnoreActorList;
	IgnoreActorList.Add( GetOwner() );

	TArray<AActor*> NearActorList;

	const FVector OwnerLocation = GetOwner()->GetActorLocation();
	const FVector OwnerForwardDirection = GetOwner()->GetActorForwardVector();
	if( UKismetSystemLibrary::SphereOverlapActors( this, OwnerLocation, UGlobalVariable::Get()->Targeting_DistanceCheckScoreDistanceRange.Y, ObjectTypes, nullptr, IgnoreActorList, NearActorList ) == false )
	{
		TargetActor.Reset();
		return;
	}

	AActor* BestTargetActor = nullptr;
	float BestScore = -MAX_FLT;
	for( AActor* CandidateTargetActor : NearActorList )
	{
		if( CheckTargetable( CandidateTargetActor ) == false )
			continue;

		AGameCharacter* GameCharacter = Cast<AGameCharacter>( CandidateTargetActor );

		const FVector CandidateTargetLocation = GameCharacter ? GameCharacter->GetActorBottomLocation() : CandidateTargetActor->GetActorLocation();
		const FVector ToTargetVector = CandidateTargetLocation - OwnerLocation;
		const FVector ToTargetDirection = ToTargetVector.GetSafeNormal();
		const float ToTargetDistance = ToTargetVector.Length();

		const float DistanceScore = FMath::GetMappedRangeValueClamped( UGlobalVariable::Get()->Targeting_DistanceCheckScoreDistanceRange, FVector2D( 0.0f, UGlobalVariable::Get()->Targeting_DistanceMaxScore ), ToTargetDistance );
	
		float AngleScore = UGlobalVariable::Get()->Targeting_AngleMaxScore;
		if( ToTargetDistance > KINDA_SMALL_NUMBER )
		{
			const float ToTargetBetweenAngleDeg = FMath::RadiansToDegrees( FMath::Acos( FVector::DotProduct( OwnerForwardDirection, -ToTargetDirection ) ) );
			AngleScore = FMath::GetMappedRangeValueClamped( UGlobalVariable::Get()->Targeting_AngleCheckScoreAngleDegRange, FVector2D( 0.0f, UGlobalVariable::Get()->Targeting_AngleMaxScore ), ToTargetBetweenAngleDeg );
		}

		const float TotalScore = DistanceScore + AngleScore;
		if( TotalScore > BestScore )
		{
			BestScore = TotalScore;
			BestTargetActor = CandidateTargetActor;
		}

#ifdef MJ_EDITOR
		FVector TextLocation = GameCharacter ? GameCharacter->GetActorTopLocation() : CandidateTargetActor->GetActorLocation();
		TextLocation += FVector::UpVector * 20.0f;

		FString ScoreText = FString::Printf( TEXT( "%.2f" ), TotalScore );
		DrawDebugStringCenter( World, TextLocation, ScoreText );
#endif
	}

	TargetActor = BestTargetActor;

#ifdef MJ_EDITOR
	const AGameCharacter* GameCharacter = Cast<AGameCharacter>( TargetActor );
	FVector ConeLocation = GameCharacter ? GameCharacter->GetActorTopLocation() : TargetActor->GetActorLocation();
	ConeLocation += FVector::UpVector * 50.0f;

	const float ConeAngleRad = FMath::DegreesToRadians( 30.0f );
	DrawDebugCone( World, ConeLocation, FVector::DownVector, 20.0f, ConeAngleRad, ConeAngleRad, 12, FColor::Yellow );
#endif
}

bool UTargetActorComponent::CheckTargetable( AActor* CandidateTargetActor )
{
	return true;
}