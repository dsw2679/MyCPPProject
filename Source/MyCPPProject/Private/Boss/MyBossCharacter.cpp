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
	// GAS 초기화
	if (AbilitySystemComponent)
	{
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}

	// 메시지 방송
	FMyBossMessageStruct Message;
	Message.BossActor = this;
	Message.BossASC = this->GetAbilitySystemComponent();
	
	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(MyGameplayTags::Message_Boss_Spawned, Message);

	UE_LOG(LogTemp, Warning, TEXT("[Boss] PossessedBy: Broadcasted Spawn Message!"));
}

void AMyBossCharacter::PreloadAssets()
{
	if (!PawnData) return;

	UE_LOG(LogTemp, Warning, TEXT("[Boss] Starting Preload for %s"), *GetName());

	// 1. VFX(Niagara) 프리로드
	for (auto VFX : PawnData->PreloadVFX)
	{
		if (VFX) {
			// 실제 사용은 안 하지만 포인터를 참조함으로써 메모리에 로드 상태 유지
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

	// 3. GameplayCues 프리로드 (클래스 로딩)
	for (auto CueClass : PawnData->PreloadGameplayCues)
	{
		if (CueClass) {
			// 클래스 디폴트 객체(CDO)를 참조하여 로드 보장
			CueClass->GetDefaultObject();
			UE_LOG(LogTemp, Log, TEXT("[Boss] Preloaded Cue: %s"), *CueClass->GetName());
		}
	}

	UE_LOG(LogTemp, Warning, TEXT("[Boss] Preload Complete!"));
}

void AMyBossCharacter::OnBossInfoRequested(FGameplayTag Channel, const struct FMyBossMessageStruct& Payload)
{
	UE_LOG(LogTemp, Warning, TEXT("[Boss] RECEIVED REQUEST from UI! Sending Response..."));
	
	FMyBossMessageStruct Message;
	Message.BossActor = this;
	Message.BossASC = this->GetAbilitySystemComponent();

	UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(MyGameplayTags::Message_Request_BossInfo, Message);
}
