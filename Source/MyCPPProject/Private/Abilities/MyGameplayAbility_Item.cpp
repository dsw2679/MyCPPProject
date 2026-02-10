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
	UE_LOG(LogTemp, Error, TEXT("[Item Ability] Trying to consume Slot: %d from Ability: %s"), SlotIndex, *GetName());

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

bool UMyGameplayAbility_Item::GetHeroCursorHit(FHitResult& OutHit) const
{
	if (APlayerController* PC = GetActorInfo().PlayerController.Get())
	{
		// TraceChannel은 상황에 맞게 변경 가능 (Visibility 등)
		return PC->GetHitResultUnderCursor(ECC_Visibility, true, OutHit);
	}
	return false;
}

const FGameplayTagContainer* UMyGameplayAbility_Item::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset();

	// 내 슬롯 번호 찾기
	int32 SlotIndex = GetSlotIndexFromInputTag();
	FGameplayTag SlotTag;

	switch (SlotIndex)
	{
	case 0: SlotTag = MyGameplayTags::Cooldown_Item_1; break;
	case 1: SlotTag = MyGameplayTags::Cooldown_Item_2; break;
	case 2: SlotTag = MyGameplayTags::Cooldown_Item_3; break;
	case 3: SlotTag = MyGameplayTags::Cooldown_Item_4; break;
	}

	if (SlotTag.IsValid())
	{
		MutableTags->AddTag(SlotTag);
	}

	// 만약 GA 블루프린트에서 설정한 고유 태그(Cooldown.Item.Potion 등)도 같이 쓰고 싶다면 추가
	if (CooldownTag.IsValid())
	{
		MutableTags->AddTag(CooldownTag);
	}

	return MutableTags;
}

void UMyGameplayAbility_Item::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// 시간 설정 (SetByCaller)
			FGameplayTag DurationTag = FGameplayTag::RequestGameplayTag(FName("SetByCaller.Cooldown"));
			SpecHandle.Data->SetSetByCallerMagnitude(DurationTag, CooldownDuration);

			// [핵심] GetCooldownTags()가 뱉는 태그들을 그대로 GE에 주입
			const FGameplayTagContainer* CDTags = GetCooldownTags();
			if (CDTags)
			{
				for (const FGameplayTag& Tag : *CDTags)
				{
					SpecHandle.Data->AddDynamicAssetTag(Tag);
					SpecHandle.Data->DynamicGrantedTags.AddTag(Tag);
				}
			}

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
