// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBossCharacter.h"

#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "NativeGameplayTags.h"
#include "MyGameplayTags.h"
#include "NiagaraSystem.h"
#include "Experience/MyPawnData.h"
#include "Boss/MyBossAIController.h"

AMyBossCharacter::AMyBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;
	bReplicates = true;

	// GAS 컴포넌트 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// AttributeSet
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AMyBossCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMyBossCharacter::BeginPlay()
{
	Super::BeginPlay();
	PreloadAssets();
	
	if (AbilitySystemComponent)
	{
		// AI 컨트롤러가 빙의된 상태라면 InitAbilityActorInfo 호출
		// 보통 AI는 여기서 자신을 Owner이자 Avatar로 설정합니다.
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	if (HasAuthority() && InitStatEffect) // 서버(싱글에선 나)만 방송
	{
		// GE 적용 헬퍼 함수 사용
		FGameplayEffectContextHandle ContextHandle = AbilitySystemComponent->MakeEffectContext();
		ContextHandle.AddSourceObject(this);

		FGameplayEffectSpecHandle SpecHandle = AbilitySystemComponent->MakeOutgoingSpec(
			InitStatEffect, 1.0f, ContextHandle);
		if (SpecHandle.IsValid())
		{
			AbilitySystemComponent->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
		}
	}
}

void AMyBossCharacter::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	//구독해제
	UGameplayMessageSubsystem::Get(this).UnregisterListener(BossInfoRequestListenerHandle);
	Super::EndPlay(EndPlayReason);
}

void AMyBossCharacter::PossessedBy(AController* NewController)
{
	Super::PossessedBy(NewController);

	// GAS 초기화 확인
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);

		// PawnData가 있다면 스킬(Ability) 부여
		if (PawnData && HasAuthority())
		{

			// (만약 아까 제가 제안드린) 'BossAbilities' 배열을 따로 만드셨다면:
			for (const TSubclassOf<UGameplayAbility>& AbilityClass : PawnData->BossAbilities)
			{
				if (AbilityClass)
				{
					FGameplayAbilitySpec Spec(AbilityClass);
					Spec.SourceObject = this;
					Spec.Level = 1;
					AbilitySystemComponent->GiveAbility(Spec);
				}
			} 
		}
	}

	// 메시지 방송 (기존 코드 유지)
	FMyBossMessageStruct Message;
	Message.BossActor = this;
	Message.BossASC = this->GetAbilitySystemComponent();

	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(MyGameplayTags::Message_Boss_Spawned, Message);
}

void AMyBossCharacter::PreloadAssets()
{
	if (!PawnData) return;

	UE_LOG(LogTemp, Warning, TEXT("[Boss] Starting Preload for %s"), *GetName());

	// 1. VFX(Niagara) 프리로드
	// HitEffect 프리로드 (가장 중요)
	for (auto HitFX : PawnData->OnlyBoss_HitEffect)
	{
		UE_LOG(LogTemp, Log, TEXT("[Boss] Preloaded HitEffect: %s"), *HitFX->GetName());
	}

	// 목록에 있는 VFX 프리로드
	for (auto VFX : PawnData->PreloadVFX)
	{
		if (VFX) {
			UE_LOG(LogTemp, Log, TEXT("[Boss] Preloaded VFX: %s"), *VFX->GetName());
		}
	}

	// 2. SFX(Sound) 프리로드
	for (auto SFX : PawnData->PreloadSFX)
	{
		if (SFX) {
			UE_LOG(LogTemp, Log, TEXT("[Boss] Preloaded SFX: %s"), *SFX->GetName());
		}
	}

	// 3. GameplayCues 프리로드 (중요: LoadSynchronous 사용)
	for (const TSoftClassPtr<UObject>& CueSoftClass : PawnData->PreloadGameplayCues)
	{
		if (!CueSoftClass.IsNull())
		{
			// SoftClassPtr을 즉시 로드하여 메모리에 올림
			if (UClass* LoadedClass = CueSoftClass.LoadSynchronous())
			{
				// CDO를 참조하여 GC 방지 및 초기화 보장
				LoadedClass->GetDefaultObject();
				UE_LOG(LogTemp, Log, TEXT("[Boss] Preloaded Cue: %s"), *LoadedClass->GetName());
			}
		}
	}
	
	for (auto Montage : PawnData->PreloadMontages)
	{
		if (Montage)
		{
			// UE_LOG(LogTemp, Log, TEXT("Preloaded Montage: %s"), *Montage->GetName());
		}
	}

	for (const TSubclassOf<UGameplayEffect>& GEClass : PawnData->PreloadGameplayEffects)
	{
		if (GEClass)
		{
			// CDO(Class Default Object)를 가져와서 확실하게 초기화
			GEClass->GetDefaultObject();
			UE_LOG(LogTemp, Log, TEXT("[Hero] Preloaded GE: %s"), *GEClass->GetName());
		}
	}
}

void AMyBossCharacter::OnBossInfoRequested(FGameplayTag Channel, const struct FMyBossMessageStruct& Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("[Boss] RECEIVED REQUEST from UI! Sending Response..."));
	
	FMyBossMessageStruct Message;
	Message.BossActor = this;
	Message.BossASC = this->GetAbilitySystemComponent();

	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(MyGameplayTags::Message_Request_BossInfo, Message);
}
