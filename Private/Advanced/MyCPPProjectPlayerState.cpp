// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyCPPProjectPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"

AMyCPPProjectPlayerState::AMyCPPProjectPlayerState()
{
	
	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
}

UAbilitySystemComponent* AMyCPPProjectPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}
