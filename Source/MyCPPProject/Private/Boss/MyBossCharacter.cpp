// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBossCharacter.h"

#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "NativeGameplayTags.h"
#include "MyGameplayTags.h"

AMyBossCharacter::AMyBossCharacter()
{
	PrimaryActorTick.bCanEverTick = true;

	// GAS 컴포넌트 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Minimal);

	// AttributeSet은 필요에 따라 생성 (보통 ASC 초기화 시점에 함께 처리하거나 여기서 생성)
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AMyBossCharacter::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMyBossCharacter::BeginPlay()
{
	FMyGameplayTags::InitializeNativeTags();
	Super::BeginPlay();

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

	FTimerHandle TimerHandle;
	GetWorld()->GetTimerManager().SetTimer(TimerHandle, [this]()
	{
		if (IsValid(this))
		{
			FMyBossMessageStruct Message;
			Message.Verb = FMyGameplayTags::Get().Message_Boss_Spawned;
			Message.BossActor = this;
			Message.BossASC = this->GetAbilitySystemComponent();

			// 싱글톤 Get()을 안전하게 호출
			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
			MessageSubsystem.BroadcastMessage(Message.Verb, Message);
		}
	}, 0.2f, false);
}
