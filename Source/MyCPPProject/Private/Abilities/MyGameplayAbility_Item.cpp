// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Item.h"
#include "Component/MyInventoryComponent.h"
#include "MyGameplayTags.h"
#include "AbilitySystemComponent.h"

void UMyGameplayAbility_Item::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	if (!CommitAbility(Handle, ActorInfo, ActivationInfo))
	{
		EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
		return;
	}

	// 입력 태그를 통해 동적으로 슬롯 인덱스 결정
	int32 DynamicSlotIndex = GetSlotIndexFromInputTag();
	if (DynamicSlotIndex != -1)
	{
		SlotIndex = DynamicSlotIndex;
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
				ActivateItemEffect();
			}
		}
	}

	EndAbility(Handle, ActorInfo, ActivationInfo, true, false);
}

void UMyGameplayAbility_Item::ActivateItemEffect()
{
	// 기본적으로 블루프린트 이벤트 호출
	OnItemEffectTriggered();
}

int32 UMyGameplayAbility_Item::GetSlotIndexFromInputTag() const
{
	if (CurrentActorInfo && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		// 현재 활성화된 스펙의 Dynamic Tags(Input Tags 포함) 확인
		if (FGameplayAbilitySpec* Spec = CurrentActorInfo->AbilitySystemComponent->FindAbilitySpecFromHandle(CurrentSpecHandle))
		{
			if (Spec->GetDynamicSpecSourceTags().HasTag(MyGameplayTags::InputTag_Item_1)) return 0;
			if (Spec->GetDynamicSpecSourceTags().HasTag(MyGameplayTags::InputTag_Item_2)) return 1;
			if (Spec->GetDynamicSpecSourceTags().HasTag(MyGameplayTags::InputTag_Item_3)) return 2;
			if (Spec->GetDynamicSpecSourceTags().HasTag(MyGameplayTags::InputTag_Item_4)) return 3;
		}
	}
	
	return -1;
}