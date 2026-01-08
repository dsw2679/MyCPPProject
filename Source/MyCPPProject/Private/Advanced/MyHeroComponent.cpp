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

UMyHeroComponent::UMyHeroComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
}

void UMyHeroComponent::InitializeAbilitySystem()
{   
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;

    // 1. GameState에서 Experience 매니저를 찾아 PawnData 가져오기
    AGameStateBase* GS = GetWorld()->GetGameState();
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
    if (!PlayerInputComponent || !PawnData /*|| !PawnData->InputConfig*/) return;
    
    APawn* Pawn = GetPawn<APawn>();
    if (!Pawn) return;

    APlayerController* PC = Cast<APlayerController>(Pawn->GetController());
    if (PC)
    {
        if (UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<UEnhancedInputLocalPlayerSubsystem>(PC->GetLocalPlayer()))
        {
            // [추가] PawnData에 설정된 Mapping Context가 있다면 추가
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

    // Ability Input Actions 바인딩
    for (const FMyInputAction& Action : InputConfig->AbilityInputActions)
    {
        if (Action.InputAction && Action.InputTag.IsValid())
        {
            MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Started, this, &ThisClass::Input_AbilityInputTagPressed, Action.InputTag);
            MyInputComp->BindAction(Action.InputAction, ETriggerEvent::Completed, this, &ThisClass::Input_AbilityInputTagReleased, Action.InputTag);
        }
    }
}

void UMyHeroComponent::Input_AbilityInputTagPressed(FGameplayTag InputTag)
{
    UE_LOG(LogTemp, Warning, TEXT("Input Pressed: %s"), *InputTag.ToString());
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