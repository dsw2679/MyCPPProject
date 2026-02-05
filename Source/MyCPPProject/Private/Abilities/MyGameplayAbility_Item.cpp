// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Item.h"
#include "Component/MyInventoryComponent.h"

void UMyGameplayAbility_Item::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 인벤토리 컴포넌트 찾아오기
	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (UMyInventoryComponent* InvComp = Avatar->FindComponentByClass<UMyInventoryComponent>())
		{
			// 아이템 소모 시도
			if (InvComp->ConsumeItem(SlotIndex))
			{
				// 성공 시 이펙트 실행
				OnItemEffectTriggered();
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}
