// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Adropine.h"

void UMyGameplayAbility_Adropine::ApplyAdropineEffect()
{
	if (!HasAuthority(&CurrentActivationInfo)) return;

	// 버프 적용
	if (BuffEffectClass)
	{
		FGameplayEffectSpecHandle BuffSpec = MakeOutgoingGameplayEffectSpec(BuffEffectClass, GetAbilityLevel());
		if (BuffSpec.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, BuffSpec);
		}
	}

	// 체력 감소 (디메리트)
	if (HPDrainEffectClass)
	{
		FGameplayEffectSpecHandle DrainSpec = MakeOutgoingGameplayEffectSpec(HPDrainEffectClass, GetAbilityLevel());
		if (DrainSpec.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, DrainSpec);
		}
	}
}
