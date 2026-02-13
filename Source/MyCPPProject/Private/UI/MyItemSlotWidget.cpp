// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyItemSlotWidget.h"

#include "MyGameplayTags.h"
#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Experience/MyItemDefinition.h"
#include "Advanced/DragDrop/MyDragDropOperation.h"
#include "Component/MyInventoryComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"

void UMyItemSlotWidget::UpdateItemData(const FMyItemSlotInfo& SlotInfo)
{
	const UMyItemDefinition* ItemDef = SlotInfo.ItemDef;
	CurrentItemDef = ItemDef;
	
	if (ItemDef)
	{
		// 아이콘 설정 (Soft Object Pointer 로드)
		if (IMG_ItemIcon)
		{
			UTexture2D* IconTexture = ItemDef->ItemIcon.LoadSynchronous();
			IMG_ItemIcon->SetBrushFromTexture(IconTexture);
			IMG_ItemIcon->SetVisibility(ESlateVisibility::Visible);
		}

		// 개수 텍스트 설정 (현재 / 최대)
		if (TXT_CountText)
		{
			FString CountStr = FString::Printf(TEXT("%d / %d"), SlotInfo.CurrentCount, SlotInfo.MaxStackCount);
			TXT_CountText->SetText(FText::FromString(CountStr));
			TXT_CountText->SetVisibility(ESlateVisibility::Visible);
		}
	}
	else
	{
		// 아이템이 없는 경우 (비어있는 슬롯 처리)
		if (IMG_ItemIcon) IMG_ItemIcon->SetVisibility(ESlateVisibility::Hidden);
		if (TXT_CountText) TXT_CountText->SetVisibility(ESlateVisibility::Hidden);
	}
}

bool UMyItemSlotWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent,
	UDragDropOperation* InOperation)
{
	// C++ 클래스로 캐스팅
	if (UMyDragDropOperation* ItemOperation = Cast<UMyDragDropOperation>(InOperation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Drop] SourceType: %d, Item: %s"),
	(int)ItemOperation->SourceType, ItemOperation->PayloadItem ? *ItemOperation->PayloadItem->ItemName.ToString() : TEXT("NULL"));
		
		// OwningPlayerState에서 컴포넌트를 찾습니다.
		if (APlayerState* PS = GetOwningPlayerState())
		{
			
			if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
			{
				
				switch (ItemOperation->SourceType)
				{
				case EMyDragSource::Inventory:
					if (ItemOperation->PayloadItem)
					{
						InvComp->EquipItemToSlot(ItemOperation->PayloadItem, MySlotIndex);
						return true;
					}
					break;

				case EMyDragSource::QuickSlot:
					// [디버깅 로그 추가]
					UE_LOG(LogTemp, Warning, TEXT("[Swap] Attempting swap. Source: %d, Destination: %d"),
						ItemOperation->SourceIndex, MySlotIndex);

					// 자기 자신에게 드롭한 게 아닐 때만 스왑
					if (ItemOperation->SourceIndex != MySlotIndex && ItemOperation->SourceIndex != -1)
					{
						InvComp->SwapQuickSlots(ItemOperation->SourceIndex, MySlotIndex);
						return true;
					}
					else
					{
						UE_LOG(LogTemp, Error, TEXT("[Swap] Swap condition failed!"));
					}
					break;
				case EMyDragSource::Shop:
					// 상점 -> 퀵슬롯: 구매 후 성공하면 즉시 장착
					if (ItemOperation->PayloadItem)
					{
						// 먼저 구매 시도
						if (InvComp->TryBuyItem(ItemOperation->PayloadItem))
						{
							// 구매 성공 시 해당 슬롯에 장착
							InvComp->EquipItemToSlot(ItemOperation->PayloadItem, MySlotIndex);
							return true;
						}
					}
					break;
				}
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

void UMyItemSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (!bIsDragVisual)
	{
		if (UWorld* World = GetWorld())
		{
			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
			MessageSubsystem.RegisterListener(MyGameplayTags::Message_Inventory_Updated, this, &UMyItemSlotWidget::OnInventoryUpdated);
			
			// 초기 데이터 동기화 
			if (APlayerState* PS = GetOwningPlayerState())
			{
				// PlayerState에서 인벤토리 컴포넌트를 찾습니다.
				if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
				{
					// 컴포넌트가 가진 현재 슬롯 정보 배열을 가져옵니다.
					const TArray<FMyItemSlotInfo>& Slots = InvComp->GetBattleItemSlots();

					// 내 슬롯 번호(MySlotIndex)가 유효한지 확인하고 데이터를 주입합니다.
					if (Slots.IsValidIndex(MySlotIndex))
					{
						UpdateItemData(Slots[MySlotIndex]);
						UE_LOG(LogTemp, Log, TEXT("[UI] Slot %d Initialized with current data."), MySlotIndex);
					}
				}
			}
		}
	}
	else
	{
		UE_LOG(LogTemp, Log, TEXT("[UI] Drag Visual Created - Skipping Listener Registration"));
	}
}

void UMyItemSlotWidget::OnInventoryUpdated(FGameplayTag Channel, const FMyInventoryMessage& Message)
{
	UE_LOG(LogTemp, Warning, TEXT("[UI] Slot %d Received Update Message"), MySlotIndex);
	// 메시지에 담긴 BattleItemSlots 배열에서 내 인덱스(MySlotIndex) 정보를 찾아 갱신
	if (Message.BattleItemSlots.IsValidIndex(MySlotIndex))
	{
		UpdateItemData(Message.BattleItemSlots[MySlotIndex]);
	}
}