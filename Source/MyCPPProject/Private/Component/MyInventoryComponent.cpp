// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyInventoryComponent.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AbilitySystemGlobals.h"
#include "MyGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Experience/MyItemDefinition.h"
#include "GameplayAbilitySpec.h"
#include "Abilities/MyGameplayAbility_Item.h"
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
	// 유효성 검사
	if (!ItemDef)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] Buy Failed: Item definition is null."));
		return false;
	}

	// 가격 유효성 검사 (0원 이상인지 확인)
	if (ItemDef->Price < 0)
	{
		UE_LOG(LogTemp, Error, TEXT("[Inventory] Buy Failed: Item %s has invalid price (%d)."),
			*ItemDef->ItemName.ToString(), ItemDef->Price);
		return false;
	}

	// 재화 확인
	if (Gold < ItemDef->Price)
	{
		UE_LOG(LogTemp, Warning, TEXT("[Inventory] Buy Failed: Not enough gold for %s. (Required: %d, Current: %d)"),
			*ItemDef->ItemName.ToString(), ItemDef->Price, Gold);

		// 여기서 GameplayMessageRouter를 통해 "골드가 부족합니다" 팝업을 띄우는 신호를 보낼 수 있습니다.
		return false;
	}

	// 구매 처리 (트랜잭션)
	Gold -= ItemDef->Price;

	// 처음 사는 아이템이라면 순서 배열에 등록
	if (!OwnedItems.Contains(ItemDef))
	{
		OwnedItemOrder.Add(ItemDef);
	}
	
	// TMap에 아이템 추가 (이미 있으면 수량 증가, 없으면 1로 초기화)
	int32& OwnedCount = OwnedItems.FindOrAdd(ItemDef);
	OwnedCount++;

	UE_LOG(LogTemp, Log, TEXT("[Inventory] Buy Success: %s. Remaining Gold: %d. Total Owned: %d"),
		*ItemDef->ItemName.ToString(), Gold, OwnedCount);

	// 퀵슬롯 수량 동기화 로직
	for (int32 i = 0; i < BattleItemSlots.Num(); ++i)
	{
		// 현재 장착된 아이템이 방금 구매한 아이템과 같다면
		if (BattleItemSlots[i].ItemDef == ItemDef)
		{
			// 최대 소지 개수 제한 내에서 수량 갱신
			BattleItemSlots[i].CurrentCount = FMath::Min(OwnedCount, ItemDef->MaxStackCount);
		}
	}
	
	// 변경 사항 전파 (메시지 브로드캐스트)
	BroadcastInventoryMessage();

	return true;
}

void UMyInventoryComponent::EquipItemToSlot(const UMyItemDefinition* ItemDef, int32 SlotIndex)
{
	
	if (!ItemDef || !BattleItemSlots.IsValidIndex(SlotIndex)) return;

	int32 OwnedCount = OwnedItems.FindRef(ItemDef);
	if (OwnedCount <= 0)
	{
		if (!TryBuyItem(ItemDef)) return;
		OwnedCount = OwnedItems.FindRef(ItemDef);
	}

	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());

	// 중복 제거 (지금 장착할 슬롯 이외의 칸에 같은 아이템이 있다면 해제)
	for (int32 i = 0; i < BattleItemSlots.Num(); ++i)
	{
		if (i == SlotIndex) continue; 

		if (BattleItemSlots[i].ItemDef == ItemDef)
		{
			if (ASC && BattleItemSlots[i].AbilityHandle.IsValid()) {
				ASC->ClearAbility(BattleItemSlots[i].AbilityHandle);
			}
			BattleItemSlots[i] = FMyItemSlotInfo(); 
		}
	}

	// 현재 슬롯의 기존 어빌리티 제거
	if (ASC && BattleItemSlots[SlotIndex].AbilityHandle.IsValid())
	{
		ASC->ClearAbility(BattleItemSlots[SlotIndex].AbilityHandle);
	}

	// 데이터 및 어빌리티 할당
	FMyItemSlotInfo NewSlotInfo;
	NewSlotInfo.ItemDef = ItemDef;
	NewSlotInfo.CurrentCount = FMath::Min(OwnedCount, ItemDef->MaxStackCount);
	NewSlotInfo.MaxStackCount = ItemDef->MaxStackCount;

	if (ASC && ItemDef->ItemAbility)
	{
		FGameplayAbilitySpec Spec(ItemDef->ItemAbility);
		Spec.GetDynamicSpecSourceTags().AddTag(GetInputTagForSlot(SlotIndex));
		
		Spec.InputID = 100 + SlotIndex;
		NewSlotInfo.AbilityHandle = ASC->GiveAbility(Spec);
	}

	// 최종 저장
	BattleItemSlots[SlotIndex] = NewSlotInfo;

	BroadcastInventoryMessage();
	UE_LOG(LogTemp, Log, TEXT("[Equip] Successfully equipped %s to slot %d"), *ItemDef->ItemName.ToString(), SlotIndex);
}

bool UMyInventoryComponent::ConsumeItem(int32 SlotIndex)
{
	// 서버 권한 확인 (클라이언트는 실행 금지)
	if (!GetOwner()->HasAuthority())
	{
		return false;
	}
	
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

void UMyInventoryComponent::SwapQuickSlots(int32 SlotIndexA, int32 SlotIndexB)
{
	// [1] 데이터 백업 (구조체 복사)
	FMyItemSlotInfo SlotA = BattleItemSlots[SlotIndexA];
	FMyItemSlotInfo SlotB = BattleItemSlots[SlotIndexB];

	// [2] 실제 배열 데이터 교체
	BattleItemSlots[SlotIndexA] = SlotB;
	BattleItemSlots[SlotIndexB] = SlotA;

	// [3] GAS 어빌리티 태그 업데이트 (입력 키 번호를 슬롯 인덱스에 맞게 동기화)
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (ASC)
	{
		auto UpdateTag = [&](int32 Index) {
			FMyItemSlotInfo& Slot = BattleItemSlots[Index];
			if (Slot.AbilityHandle.IsValid())
			{
				if (FGameplayAbilitySpec* Spec = ASC->FindAbilitySpecFromHandle(Slot.AbilityHandle))
				{
					// 기존 아이템 태그(1~4번) 모두 제거
					Spec->GetDynamicSpecSourceTags().RemoveTag(MyGameplayTags::InputTag_Item_1);
					Spec->GetDynamicSpecSourceTags().RemoveTag(MyGameplayTags::InputTag_Item_2);
					Spec->GetDynamicSpecSourceTags().RemoveTag(MyGameplayTags::InputTag_Item_3);
					Spec->GetDynamicSpecSourceTags().RemoveTag(MyGameplayTags::InputTag_Item_4);

					// 새 슬롯 번호에 맞는 태그 부여
					Spec->GetDynamicSpecSourceTags().AddTag(GetInputTagForSlot(Index));
					ASC->MarkAbilitySpecDirty(*Spec);
				}
			}
		};

		UpdateTag(SlotIndexA);
		UpdateTag(SlotIndexB);
	}

	// [4] 결과 보고 및 UI 갱신
	BroadcastInventoryMessage();
	UE_LOG(LogTemp, Log, TEXT("[Inventory] Swap Fixed: Slot %d <-> %d"), SlotIndexA, SlotIndexB);
}

void UMyInventoryComponent::RefillItems()
{
	for (FMyItemSlotInfo& Slot : BattleItemSlots) {
		if (Slot.ItemDef) {
			Slot.CurrentCount = Slot.MaxStackCount;
		}
	}
	BroadcastInventoryMessage(); // UI 갱신 메시지 전송
}

void UMyInventoryComponent::SetInventoryData(int32 InGold,
	const TMap<TObjectPtr<const UMyItemDefinition>, int32>& InOwnedItems, const TArray<FMyItemSlotInfo>& InSlots)
{
	Gold = InGold;
	OwnedItems = InOwnedItems;
	BattleItemSlots = InSlots;
	
	// 중요: 새 레벨의 ASC를 찾아서 어빌리티를 다시 부여합니다.
	UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(GetOwner());
	if (ASC)
	{
		for (int32 i = 0; i < BattleItemSlots.Num(); ++i)
		{
			FMyItemSlotInfo& Slot = BattleItemSlots[i];
			if (Slot.ItemDef && Slot.ItemDef->ItemAbility)
			{
				FGameplayAbilitySpec Spec(Slot.ItemDef->ItemAbility);
				Spec.GetDynamicSpecSourceTags().AddTag(GetInputTagForSlot(i));
				Spec.InputID = 100 + i;
	
				// 새 핸들 부여
				Slot.AbilityHandle = ASC->GiveAbility(Spec);
			}
		}
	}

	// 데이터가 복사되었음을 알림
	BroadcastInventoryMessage();
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