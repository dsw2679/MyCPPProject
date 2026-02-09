// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Item.h"
#include "Component/MyInventoryComponent.h"
#include "MyGameplayTags.h"
#include "AbilitySystemComponent.h"
#include "GameFramework/PlayerState.h"


UMyGameplayAbility_Item::UMyGameplayAbility_Item()
{
	// 액터당 하나만 만들어서 재사용
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
	
	NetExecutionPolicy = EGameplayAbilityNetExecutionPolicy::LocalPredicted;
}

bool UMyGameplayAbility_Item::ConsumeItem()
{
	int32 SlotIndex = GetSlotIndexFromInputTag(); // (이 함수는 유지)

	if (AActor* Avatar = GetAvatarActorFromActorInfo())
	{
		if (APawn* Pawn = Cast<APawn>(Avatar))
		{
			if (APlayerState* PS = Pawn->GetPlayerState())
			{
				if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
				{
					// 서버 권한 체크 후 소모
					if (HasAuthority(&CurrentActivationInfo))
					{
						return InvComp->ConsumeItem(SlotIndex);
					}
					return true; // 클라이언트는 성공 처리
				}
			}
		}
	}
	return false;
}

const FGameplayTagContainer* UMyGameplayAbility_Item::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset();

	if (CooldownTag.IsValid())
	{
		MutableTags->AddTag(CooldownTag);
		return MutableTags;
	}
	return Super::GetCooldownTags();
}

void UMyGameplayAbility_Item::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		// 1. 쿨타임 이펙트 스펙 생성
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// 2. [SetByCaller] 쿨타임 시간 주입 (태그: SetByCaller.Cooldown)
			FGameplayTag DurationTag = FGameplayTag::RequestGameplayTag(FName("SetByCaller.Cooldown"));
			SpecHandle.Data->SetSetByCallerMagnitude(DurationTag, CooldownDuration);

			// 3. 쿨다운 태그 동적 주입
			if (CooldownTag.IsValid())
			{
				SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
			}

			// 4. 적용
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
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
