// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyInventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "MyGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Experience/MyItemDefinition.h"
#include "GameplayAbilitySpec.h"
#include "Advanced/MyGameplayAbility.h"

UMyInventoryComponent::UMyInventoryComponent()
{
	PrimaryComponentTick.bCanEverTick = false;

	BattleItemSlots.SetNum(4);
}

void UMyInventoryComponent::BeginPlay()
{
	Super::BeginPlay();
	
	BroadcastInventoryMessage();
}

bool UMyInventoryComponent::TryBuyItem(const UMyItemDefinition* ItemDef)
{
	UE_LOG(LogTemp, Warning, TEXT("[TryBuyItem]  is called"));
	if (!ItemDef) return false;

	if (Gold < ItemDef->Price)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] Not enough gold!"));
		return false;
	}
	
	Gold -= ItemDef->Price;
	OwnedItems.FindOrAdd(ItemDef)++;

	BroadcastInventoryMessage();
	return true;
}

void UMyInventoryComponent::EquipItemToSlot(const UMyItemDefinition* ItemDef, int32 SlotIndex)
{
	
	if (!ItemDef || !BattleItemSlots.IsValidIndex(SlotIndex)) return;
	
	int32 OwnedCount = OwnedItems.FindRef(ItemDef);
	if (OwnedCount <= 0)
	{
		// 골드가 충분하다면 구매 성공
		if (TryBuyItem(ItemDef))
		{
			OwnedCount = OwnedItems.FindRef(ItemDef); // 구매 후 수량(1개) 다시 가져옴
			UE_LOG(LogTemp, Log, TEXT("[Inventory] Auto-bought item on drop: %s"), *ItemDef->ItemName.ToString());
		}
		else
		{
			// 골드가 없거나 구매 실패 시 장착 중단
			UE_LOG(LogTemp, Warning, TEXT("[Inventory] FAILED to auto-buy on drop: %s (Not enough gold?)"), *ItemDef->ItemName.ToString());
			return;
		}
	}
	
	// asc를 찾아와서
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (!ASC) return;
	
	// 해당 슬롯에 이미 능력이 있다면 먼저 제거 
	if (BattleItemSlots[SlotIndex].AbilityHandle.IsValid())
	{
		ASC->ClearAbility(BattleItemSlots[SlotIndex].AbilityHandle);
		BattleItemSlots[SlotIndex].AbilityHandle = FGameplayAbilitySpecHandle();
	}
	
	// 이미 다른 슬롯에 같은 아이템이 있다면 해제 (중복 방지)
	for (int32 i = 0; i < BattleItemSlots.Num(); ++i)
	{
		if (BattleItemSlots[i].ItemDef == ItemDef)
		{
			BattleItemSlots[i].ItemDef = nullptr;
			BattleItemSlots[i].CurrentCount = 0;
			BattleItemSlots[i].MaxStackCount = 0;
		}
	}
	
	// 새로운 아이템 데이터 설정
	BattleItemSlots[SlotIndex].ItemDef = ItemDef;
	if (ItemDef)
	{
		BattleItemSlots[SlotIndex].CurrentCount = FMath::Min(OwnedCount, ItemDef->MaxStackCount);
		BattleItemSlots[SlotIndex].MaxStackCount = ItemDef->MaxStackCount;

		// 5. 아이템에 어빌리티가 있다면 새로 부여
		if (ItemDef->ItemAbility)
		{
			FGameplayAbilitySpec Spec(ItemDef->ItemAbility);

			// 입력 태그(1, 2, 3, 4번 키)를 동적으로 할당
			FGameplayTag SlotTag = GetInputTagForSlot(SlotIndex);
			//Spec.DynamicAbilityTags.AddTag(SlotTag);
			Spec.GetDynamicSpecSourceTags().AddTag(SlotTag);

			// 능력 부여 후 핸들 저장 (나중에 지우기 위함)
			BattleItemSlots[SlotIndex].AbilityHandle = ASC->GiveAbility(Spec);
		}
	}
	
	// 슬롯에 등록 및 현재 보유 수량(최대치 제한 적용) 할당
	BattleItemSlots[SlotIndex].ItemDef = ItemDef;
	BattleItemSlots[SlotIndex].CurrentCount = FMath::Min(OwnedCount, ItemDef->MaxStackCount);
	BattleItemSlots[SlotIndex].MaxStackCount = ItemDef->MaxStackCount;

	BroadcastInventoryMessage();
}

bool UMyInventoryComponent::ConsumeItem(int32 SlotIndex)
{
	if (!BattleItemSlots.IsValidIndex(SlotIndex)) return false;

	FMyItemSlotInfo& Slot = BattleItemSlots[SlotIndex];
	if (Slot.ItemDef && Slot.CurrentCount > 0)
	{
		Slot.CurrentCount--;

		// 전체 인벤토리에서도 하나 감소
		if (int32* TotalOwned = OwnedItems.Find(Slot.ItemDef))
		{
			(*TotalOwned) = FMath::Max(0, (*TotalOwned) - 1);
		}

		BroadcastInventoryMessage();
		return true;
	}

	return false;
}

int32 UMyInventoryComponent::GetTotalItemCount(const UMyItemDefinition* ItemDef) const
{
	return OwnedItems.FindRef(ItemDef);
}

void UMyInventoryComponent::BroadcastInventoryMessage()
{
	if (UWorld* World = GetWorld())
	{
		FMyInventoryMessage Message;
		Message.OwnerActor = GetOwner();
		Message.CurrentGold = Gold;
		Message.BattleItemSlots = BattleItemSlots;

		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		MessageSubsystem.BroadcastMessage(MyGameplayTags::Message_Inventory_Updated, Message);
	}
}

FGameplayTag UMyInventoryComponent::GetInputTagForSlot(int32 SlotIndex)
{
	switch (SlotIndex)
	{
	case 0: return MyGameplayTags::InputTag_Item_1;
	case 1: return MyGameplayTags::InputTag_Item_2;
	case 2: return MyGameplayTags::InputTag_Item_3;
	case 3: return MyGameplayTags::InputTag_Item_4;
	}
	return FGameplayTag::EmptyTag;
}