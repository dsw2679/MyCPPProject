// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_TimeStop.h"


void UMyGameplayAbility_TimeStop::ApplyTimeStop()
{
	if (TimeStopEffectClass)
	{
		// 자신에게 이펙트 적용 (BP_ApplyGameplayEffectToOwner와 동일한 역할)
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(TimeStopEffectClass, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
		}
	}
}
