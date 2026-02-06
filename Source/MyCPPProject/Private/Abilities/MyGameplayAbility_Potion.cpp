// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Potion.h"

void UMyGameplayAbility_Potion::ActivateItemEffect()
{
	if (HealEffectClass)
	{
		// 자기 자신(Avatar)에게 이펙트 적용
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(HealEffectClass);
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
			UE_LOG(LogTemp, Log, TEXT("[Item] Potion Used! Slot: %d"), SlotIndex);
		}
	}
}
