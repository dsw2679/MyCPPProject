// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBossCharacter.h"

#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"

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
	Super::BeginPlay();

	if (AbilitySystemComponent)
	{
		// AI 컨트롤러가 빙의된 상태라면 InitAbilityActorInfo 호출
		// 보통 AI는 여기서 자신을 Owner이자 Avatar로 설정합니다.
		AbilitySystemComponent->InitAbilityActorInfo(this, this);
	}
}