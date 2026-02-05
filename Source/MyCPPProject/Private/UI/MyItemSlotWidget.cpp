// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyItemSlotWidget.h"

#include "Components/Image.h"
#include "Components/TextBlock.h"
#include "Experience/MyItemDefinition.h"
#include "Advanced/DragDrop/MyDragDropOperation.h"
#include "Component/MyInventoryComponent.h"

void UMyItemSlotWidget::UpdateItemData(const FMyItemSlotInfo& SlotInfo)
{
	const UMyItemDefinition* ItemDef = SlotInfo.ItemDef;

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
		if (ItemOperation->PayloadItem)
		{
			// 인벤토리 컴포넌트 찾아서 장착 호출
			if (APawn* OwningPawn = GetOwningPlayerPawn())
			{
				if (UMyInventoryComponent* InvComp = OwningPawn->FindComponentByClass<UMyInventoryComponent>())
				{
					// MySlotIndex는 이 클래스에 정의된 정수 변수
					InvComp->EquipItemToSlot(ItemOperation->PayloadItem, MySlotIndex);
					return true;
				}
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}
