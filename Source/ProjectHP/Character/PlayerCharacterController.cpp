#include "Character/PlayerCharacterController.h"
#include "Character/PlayerCharacter.h"
#include "Camera/PlayerCameraModeManager.h"
#include "GameFramework/Character.h"
#include "Components/InputComponent.h"
#include "TimerManager.h"
#include "CollisionQueryParams.h"
#include "Engine/OverlapResult.h"
#include "InputActionValue.h"

APlayerCharacterController::APlayerCharacterController()
{
	PlayerCameraManagerClass = APlayerCameraModeManager::StaticClass();
}

void APlayerCharacterController::BeginPlay()
{
	Super::BeginPlay();
}

void APlayerCharacterController::SetupInputComponent()
{
    Super::SetupInputComponent();

    // 키보드 1번 입력을 함수와 매칭 (구형 입력 시스템 방식)
    if( InputComponent )
        InputComponent->BindAction( "TestKey1", IE_Pressed, this, &APlayerCharacterController::OnSpawnAndBlendCamera );
}

void APlayerCharacterController::OnPossess( APawn* InPawn )
{
    Super::OnPossess( InPawn );
    ApplyCharacterDefaultCameraPreset( InPawn );
}

void APlayerCharacterController::SetPawn( APawn* InPawn )
{
    Super::SetPawn( InPawn );
    ApplyCharacterDefaultCameraPreset( InPawn );
}

void APlayerCharacterController::UpdateRotation( float DeltaTime )
{
    if( APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager ) )
        PlayerCameraModeManager->ApplyDeltaRotation( RotationInput );

    Super::UpdateRotation( DeltaTime );
}

void APlayerCharacterController::OnMouseWheel( const FInputActionValue& Value )
{
    APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager );
    if( PlayerCameraModeManager == nullptr )
        return;

    const float WheelDelta = Value.Get<float>();
    PlayerCameraModeManager->ApplyZoom( WheelDelta );
}

void APlayerCharacterController::ApplyCharacterDefaultCameraPreset( APawn* InPawn )
{
    APlayerCameraModeManager* PlayerCameraModeManager = Cast<APlayerCameraModeManager>( PlayerCameraManager );
    if( PlayerCameraModeManager == nullptr )
        return;

    APlayerCharacter* PlayerCharacter = Cast<APlayerCharacter>( InPawn );
    if( PlayerCharacter == nullptr )
        return;

    PlayerCameraModeManager->SetPlayerCameraPreset( EPlayerCameraPriority::CharacterDefault, PlayerCharacter->GetDefaultCameraPreset(), FObjectKey( InPawn ) );
}

//MJTEST
void APlayerCharacterController::OnSpawnAndBlendCamera()
{
    FString AssetPath = TEXT( "/Script/Engine.Blueprint'/Game/ThirdPerson/Blueprints/BP_ThirdPersonCharacter.BP_ThirdPersonCharacter_C'" );

    UClass* LoadedClass = StaticLoadClass( ACharacter::StaticClass(), nullptr, *AssetPath );

    if( !LoadedClass ) return;

    UWorld* World = GetWorld();
    if( World && GetPawn() )
    {
        APawn* CurrentActivePawn = GetPawn();

        FVector TargetLocation = CurrentActivePawn->GetActorLocation() + FVector( FMath::RandPointInCircle( 500.0f ), 0.0f );

        // ----------------------------------------------------------------------
        // [해결책 Step 1] 현재 월드 기준 카메라가 바라보던 '진짜 회전값'을 백업합니다.
        // ----------------------------------------------------------------------
        FRotator FixedWorldRotation = GetControlRotation();

        ACharacter* TargetCharacter = nullptr;

        // [주변 탐색 로직 생략 - 기존과 동일]
        float SearchRadius = 5000.0f;
        TArray<FOverlapResult> OverlapResults;
        FCollisionShape SearchSphere = FCollisionShape::MakeSphere( SearchRadius );
        FCollisionQueryParams QueryParams;
        QueryParams.AddIgnoredActor( CurrentActivePawn );

        bool bHasOverlap = World->OverlapMultiByChannel( OverlapResults, TargetLocation, FQuat::Identity, ECC_Pawn, SearchSphere, QueryParams );
        if( bHasOverlap )
        {
            for( const FOverlapResult& Result : OverlapResults )
            {
                AActor* OverlappedActor = Result.GetActor();
                if( OverlappedActor && OverlappedActor->GetClass() == LoadedClass )
                {
                    TargetCharacter = Cast<ACharacter>( OverlappedActor );
                    if( TargetCharacter ) break;
                }
            }
        }

        // 3. 주변에 없다면 기존 로직대로 새로 스폰 수행
        if( !TargetCharacter )
        {
            FActorSpawnParameters SpawnParams;
            SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AlwaysSpawn;

            // 새로 스폰할 때는 생성과 동시에 내 회전값을 먹임
            TargetCharacter = World->SpawnActor<ACharacter>( LoadedClass, TargetLocation, FixedWorldRotation, SpawnParams );
        }
        else
        {
            // ----------------------------------------------------------------------
            // [★ 이미 존재하는 캐릭터로 갈 때의 핵심 해결책]
            // 이미 배치되어 있던 캐릭터의 몸통과 제어 방향을 현재 카메라 월드 회전값으로 
            // '블렌딩 직전'에 강제로 돌려버립니다. 이렇게 해야 카메라가 돌지 않고 그대로 유지됩니다.
            // ----------------------------------------------------------------------
            TargetCharacter->SetActorRotation( FixedWorldRotation );
            if( AController* TargetAttachedController = TargetCharacter->GetController() )
            {
                // 혹시나 AIController 등이 붙어있다면 그것의 회전도 싱크를 맞춥니다.
                TargetAttachedController->SetControlRotation( FixedWorldRotation );
            }
        }

        // 4. [공통 연출 및 조종권 전환]
        if( TargetCharacter )
        {
            // 블렌딩 시작 전 내 컨트롤러와 대상 캐릭터의 상태를 철저히 고정
            this->SetControlRotation( FixedWorldRotation );
            TargetCharacter->FaceRotation( FixedWorldRotation, 0.0f );

            // 1) 카메라 뷰 블렌딩 시작 (이제 대상 캐릭터가 나랑 똑같은 곳을 보고 있으므로 튀지 않음!)
            this->SetViewTargetWithBlend( TargetCharacter, 2.0f, EViewTargetBlendFunction::VTBlend_Cubic );

            // 2) 인풋 교체
            CurrentActivePawn->DisableInput( this );
            TargetCharacter->EnableInput( this );

            // 3) 2초 뒤 안전하게 Possess 처리
            FTimerHandle SwitchTimerHandle;
            World->GetTimerManager().SetTimer( SwitchTimerHandle, [this, TargetCharacter, FixedWorldRotation]()
                {
                    if( TargetCharacter )
                    {
                        this->bAutoManageActiveCameraTarget = false;

                        // 빙의 시 일어나는 내부 덮어쓰기 무력화
                        this->Possess( TargetCharacter );

                        // 빙의 직후 다시 한 번 월드 회전값 고정 수비
                        this->SetControlRotation( FixedWorldRotation );
                    }
                }, 2.0f, false );
        }
    }
}