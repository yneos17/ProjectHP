#include "Character/NonPlayerCharacter.h"
#include "GameFramework/CharacterMovementComponent.h"

ANonPlayerCharacter::ANonPlayerCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	bUseControllerRotationYaw = false;
	if( UCharacterMovementComponent* MoveComp = GetCharacterMovement() )
	{
		MoveComp->bOrientRotationToMovement = true;
		MoveComp->RotationRate = FRotator( 0.0f, 540.0f, 0.0f );
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}
}

void ANonPlayerCharacter::BeginPlay()
{
	Super::BeginPlay();

	TestMoveStartLocation = GetActorLocation();
	TestMoveStartForwardVector = GetActorForwardVector().GetSafeNormal2D();
	TestMoveStartRightVector = GetActorRightVector().GetSafeNormal2D();

	if( UCharacterMovementComponent* MoveComp = GetCharacterMovement() )
	{
		MoveComp->MaxWalkSpeed = MoveSpeed;
	}
}

void ANonPlayerCharacter::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);
}

void ANonPlayerCharacter::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);
}

void ANonPlayerCharacter::TestMoveForwardAndBack( const float Distance )
{
	UWorld* World = GetWorld();
	if( World == nullptr )
		return;

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	const float Frequency = ( Distance > 0.0f ) ? ( MoveSpeed / Distance ) : 1.0f;
	const float RunningTime = World->GetTimeSeconds();
	const float DeltaOffset = FMath::Sin( RunningTime * Frequency ) * Distance;

	const FVector TargetLocation = TestMoveStartLocation + ( TestMoveStartForwardVector * DeltaOffset );

	const FVector MoveDirection = ( TargetLocation - GetActorLocation() ).GetSafeNormal2D();
	AddMovementInput( MoveDirection, 1.0f );
}

void ANonPlayerCharacter::TestMoveCircle( const float Radius )
{
	UWorld* World = GetWorld();
	if( World == nullptr || Radius <= 0.0f )
		return;

	GetCharacterMovement()->MaxWalkSpeed = MoveSpeed;

	const FVector CenterLocation = TestMoveStartLocation + ( TestMoveStartRightVector * Radius );

	const float AngularSpeed = MoveSpeed / Radius;
	const float RunningTime = World->GetTimeSeconds();
	const float CurrentAngle = RunningTime * AngularSpeed;

	const FVector TargetOffset = ( -TestMoveStartRightVector * FMath::Cos( CurrentAngle ) ) + ( TestMoveStartForwardVector * FMath::Sin( CurrentAngle ) );
	const FVector TargetLocation = CenterLocation + ( TargetOffset * Radius );

	const FVector MoveDirection = ( TargetLocation - GetActorLocation() ).GetSafeNormal2D();
	AddMovementInput( MoveDirection, 1.0f );
}
