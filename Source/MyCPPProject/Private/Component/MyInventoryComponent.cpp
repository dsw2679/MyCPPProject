// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyInventoryComponent.h"

#include "MyGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Experience/MyItemDefinition.h"

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
	if (!ItemDef || Gold < ItemDef->Price) return false;

	Gold -= ItemDef->Price;
	OwnedItems.FindOrAdd(ItemDef)++;

	BroadcastInventoryMessage();
	return true;
}

void UMyInventoryComponent::EquipItemToSlot(const UMyItemDefinition* ItemDef, int32 SlotIndex)
{
	if (!ItemDef || !BattleItemSlots.IsValidIndex(SlotIndex)) return;

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

	// 슬롯에 등록 및 현재 보유 수량(최대치 제한 적용) 할당
	int32 OwnedCount = OwnedItems.FindRef(ItemDef);
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
