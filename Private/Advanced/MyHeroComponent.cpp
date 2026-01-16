// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyHeroComponent.h"
#include "Experience/MyPawnData.h"
#include "Advanced/MyExperienceManagerComponent.h"
#include "Experience/MyExperienceDefinition.h"
#include "Input/MyInputConfig.h"
#include "Input/MyEnhancedInputComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "EnhancedInputSubsystems.h"
#include "GameFramework/GameStateBase.h"
#include "InputActionValue.h"
#include "Blueprint/AIBlueprintHelperLibrary.h"
#include "NiagaraFunctionLibrary.h"
#include "NiagaraSystem.h"
#include "MyCPPProjectPlayerController.h"
#include "Engine/LocalPlayer.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/PlayerController.h"

UMyHeroComponent::UMyHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMyHeroComponent::InitializeAbilitySystem()
{   
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;
    
    UWorld* World = GetWorld();
    if (!World) return;

    // 1. GameState에서 Experience 매니저를 찾아 PawnData 가져오기
    AGameStateBase* GS = World->GetGameState();
    if (!GS) return;

    UMyExperienceManagerComponent* ExperienceMgr = GS->FindComponentByClass<UMyExperienceManagerComponent>();
    if (!ExperienceMgr) return;
    
    if (ExperienceMgr->IsExperienceLoaded())
    {
        OnExperienceLoaded(ExperienceMgr->GetCurrentExperienceChecked());
    }
    else
    {
        ExperienceMgr->OnExperienceLoaded.AddUObject(this, &ThisClass::OnExperienceLoaded);
    }
}

void UMyHeroComponent::OnExperienceLoaded(const UMyExperienceDefinition* Experience)
{
    // 이미 초기화되었다면 중복 실행 방지
    if (bHasInitialized)
    {
        return;
    }
    bHasInitialized = true;
    
    if (!Experience)
    {
        return;
    }

    PawnData = Experience->DefaultPawnData;
    if (!PawnData)
    {
        return;
    }

    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;
    
    // 1. GAS 초기화 및 스킬 부여
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(Pawn);
    if (ASI)
    {
        UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
        if (ASC)
        {
            if (Pawn->HasAuthority())
            {
                // Ability 스킬 부여 로직 
                for (const FMyAbilitySet_GameplayAbility& AbilitySet : PawnData->Abilities)
                {
                    if (AbilitySet.Ability)
                    {
                        FGameplayAbilitySpec Spec(AbilitySet.Ability);
                        Spec.SourceObject = GetOwner();
                        Spec.Level = 1;

                        if (AbilitySet.InputTag.IsValid())
                        {
                            Spec.GetDynamicSpecSourceTags().AddTag(AbilitySet.InputTag);
                        }

                        ASC->GiveAbility(Spec);
                    }
                }
                // 초기 GameplayEffect (스텟 초기화) 적용
                for (const TSubclassOf<UGameplayEffect>& EffectClass : PawnData->StartupGameplayEffects)
                {
                    if (EffectClass)
                    {
                        FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
                        Context.AddSourceObject(Pawn);
                        ASC->ApplyGameplayEffectToSelf(EffectClass.GetDefaultObject(), 1.0f, Context);
                    }
                }
            }
        }
    }

    // 2. 입력 바인딩 (로딩이 늦게 끝난 경우 여기서 다시 한번 시도)
    if (Pawn->InputComponent)
    {
        InitializePlayerInput(Pawn->InputComponent);
    }
}

void UMyHeroComponent::InitializePlayerInput(UInputComponent* PlayerInputComponent)
{
    // 이미 초기화되었다면 중복 바인딩 방지
    if (bInputInitialized)
    {
        return;
    }
    
    if (!PlayerInputComponent || !PawnData) return;
    
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (PC)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            // PawnData에 설정된 Mapping Context가 있다면 추가
            if (PawnData->DefaultMappingContext)
            {
                Subsystem->ClearAllMappings(); // 기존 것 정리 (선택사항)
                Subsystem->AddMappingContext(PawnData->DefaultMappingContext, 0);
            }
        }
    }

    
    UMyEnhancedInputComponent* MyInputComp = Cast<UMyEnhancedInputComponent>(PlayerInputComponent);
    if (!MyInputComp) return;

    // MyInputConfig에 정의된 스킬 입력들을 바인딩
    const UMyInputConfig* InputConfig = PawnData->InputConfig;
    if (!InputConfig) return;
    
    // "InputTag.Move" 태그가 붙은 액션(우클릭)을 찾아서 연결
    const FGameplayTag InputTag_Move = FGameplayTag::RequestGameplayTag(FName("InputTag.Move"));

    for (const FMyInputAction& Action : InputConfig->NativeInputActions)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            if (Action.InputTag.MatchesTag(InputTag_Move))
            {
                // Triggered: 누르고 있는 동안 계속 호출 (드래그 이동 지원)
                //MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_MoveToCursor);
                MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Triggered, this, &ThisClass::Input_Move_Triggered);
                MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &ThisClass::Input_Move_Released);
            }
        }
    }

    // Ability Input Actions 바인딩
    for (const FMyInputAction& Action : InputConfig->AbilityInputActions)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ThisClass::Input_AbilityInputTagPressed, Action.InputTag);
            MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &ThisClass::Input_AbilityInputTagReleased, Action.InputTag);
        }
    }
    // 마지막에 플래그 설정
    bInputInitialized = true;
}

void UMyHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    // 캐릭터의 GAS에게 입력이 눌렸음을 전달
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetPawn<APawn>());
    if (!ASI) return;

    UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    if (!ASC) return;

    // ASC에 등록된 스킬들 중, 현재 눌린 키의 Tag를 가진 스킬을 찾습니다.
    for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            // 1. 입력이 눌렸음을 스킬 내부로 전달 (WaitInputPress 노드 등에서 사용)
            ASC->AbilityLocalInputPressed(Spec.InputID);

            // 2. 스킬이 활성화 상태가 아니라면 실행 시도
            if (!Spec.IsActive())
            {
                ASC->TryActivateAbility(Spec.Handle);
            }
        }
    }
}

void UMyHeroComponent::Input_AbilityInputTagReleased(FGameplayTag InputTag)
{
    // 캐릭터의 GAS에게 입력이 떼졌음을 전달
    IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(GetPawn<APawn>());
    if (!ASI) return;

    UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
    if (!ASC) return;

    for (FGameplayAbilitySpec& Spec : ASC->GetActivatableAbilities())
    {
        if (Spec.GetDynamicSpecSourceTags().HasTagExact(InputTag))
        {
            // 입력이 떨어졌음을 스킬 내부로 전달 (WaitInputRelease 노드 등에서 사용)
            ASC->AbilityLocalInputReleased(Spec.InputID);
        }
    }
}

void UMyHeroComponent::Input_MoveToCursor(const FInputActionValue& InputActionValue)
{
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC) return;

    // 마우스 커서 위치 찾기
    FHitResult Hit;
    bool bHitSuccessful = PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if (bHitSuccessful)
    {
        // 1. 목표지점 저장
        CachedDestination = Hit.Location;
        
        if (AMyCPPProjectPlayerController* MyPC = Cast<AMyCPPProjectPlayerController>(PC))
        {
            MyPC->MoveToLocation(Hit.Location);
        }
        
        // 3. 이펙트 재생 (PawnData에 설정된 게 있다면)
        if (PawnData && PawnData->MovementClickEffect)
        {
            UNiagaraFunctionLibrary::SpawnSystemAtLocation(
                this,
                PawnData->MovementClickEffect,
                CachedDestination,
                FRotator::ZeroRotator,
                FVector(1.f, 1.f, 1.f),
                true, true, ENCPoolMethod::None, true
            );
        }
    }
}

void UMyHeroComponent::Input_Move_Triggered(const FInputActionValue& InputActionValue)
{
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (!PC) return;

    // 1. 누르고 있는 시간 누적
    FollowTime += GetWorld()->GetDeltaSeconds();

    // 2. 마우스 커서 위치 찾기
    FHitResult Hit;
    bool bHitSuccessful = PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit);

    if (bHitSuccessful)
    {
        CachedDestination = Hit.Location;
    }

    // 3. 직접 이동 (WASD처럼 즉시 이동)
    // 캐릭터가 커서 방향으로 계속 움직이게 함
    FVector WorldDirection = (CachedDestination - Pawn->GetActorLocation()).GetSafeNormal();
    Pawn->AddMovementInput(WorldDirection, 1.0f, false);
}

void UMyHeroComponent::Input_Move_Released(const FInputActionValue& InputActionValue)
{
    // 짧게 눌렀다 뗐을 때만 '길찾기 이동' 실행
    if (FollowTime <= ShortPressThreshold)
    {
        APawn* Pawn = GetPawn<APawn>();
        if (Pawn)
        {
            APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
            if (PC)
            {
                // 네비게이션 시스템을 이용해 길찾기 이동
                UAIBlueprintHelperLibrary::SimpleMoveToLocation(PC, CachedDestination);
            }
        }
    }

    // 시간 초기화
    FollowTime = 0.0f;
}

