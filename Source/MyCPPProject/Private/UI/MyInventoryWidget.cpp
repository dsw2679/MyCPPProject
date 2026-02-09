// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyInventoryWidget.h"

#include "MyGameplayTags.h"
#include "Advanced/DragDrop/MyDragDropOperation.h"
#include "UI/MyItemWidgetInterface.h"
#include "Components/UniformGridPanel.h"
#include "Components/UniformGridSlot.h"
#include "Blueprint/UserWidget.h"
#include "Component/MyInventoryComponent.h"
#include "Components/TextBlock.h"
#include "Experience/MyItemDefinition.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "GameFramework/PlayerState.h"


void UMyInventoryWidget::RefreshInventory(const FMyInventoryMessage& Message)
{
	if (!InventoryGrid || !SlotWidgetClass) return;

	// 기존 슬롯 싹 비우기
	InventoryGrid->ClearChildren();

	// 골드 갱신
	if (TXT_CurrentGold)
	{
		// 숫자를 텍스트로 변환하여 세팅
		FText GoldText = FText::AsNumber(Message.CurrentGold);
		TXT_CurrentGold->SetText(GoldText);
	}
	
	// 인벤토리 주인으로부터 인벤토리 컴포넌트 가져오기
	AActor* Owner = Message.OwnerActor ? Message.OwnerActor : GetOwningPlayerPawn();
	if (!Owner) return;

	UMyInventoryComponent* InvComp = Owner->FindComponentByClass<UMyInventoryComponent>();
	if (!InvComp) return;

	// 인벤토리 아이템 순회 및 슬롯 생성
	int32 CurrentIndex = 0;

	// MyInventoryComponent에 추가한 GetOwnedItems() 활용
	for (const auto& ItemPair : InvComp->GetOwnedItems())
	{
		const UMyItemDefinition* ItemDef = ItemPair.Key;
		int32 Count = ItemPair.Value;

		// 개수가 0개 이하이거나 데이터가 없으면 패스
		if (Count <= 0 || !ItemDef) continue;

		// 슬롯 위젯 생성
		UUserWidget* NewSlot = CreateWidget<UUserWidget>(this, SlotWidgetClass);
		if (!NewSlot) continue;

		// 인터페이스를 통해 데이터 주입 
		// BlueprintNativeEvent이므로 Execute_ 형식을 사용합니다.
		if (NewSlot->GetClass()->ImplementsInterface(UMyItemWidgetInterface::StaticClass()))
		{
			IMyItemWidgetInterface::Execute_UpdateWidget(NewSlot, ItemDef, Count);
		}

		// 그리드 좌표 계산 (Row: 행, Col: 열)
		int32 Row = CurrentIndex / ColumnCount;
		int32 Col = CurrentIndex % ColumnCount;

		// 그리드 패널에 추가 및 배치
		UUniformGridSlot* GridSlot = InventoryGrid->AddChildToUniformGrid(NewSlot, Row, Col);
		if (GridSlot)
		{
			// 필요 시 여백(Padding)이나 정렬 설정을 여기서 할 수 있습니다.
			GridSlot->SetHorizontalAlignment(HAlign_Center);
			GridSlot->SetVerticalAlignment(VAlign_Center);
		}

		CurrentIndex++;
	}
}

void UMyInventoryWidget::OnInventoryUpdated(FGameplayTag Channel, const FMyInventoryMessage& Message)
{
	RefreshInventory(Message);
}

void UMyInventoryWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 메시지 라우터에 함수등록
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		MessageSubsystem.RegisterListener(MyGameplayTags::Message_Inventory_Updated, this, &UMyInventoryWidget::OnInventoryUpdated);
	}
	
	// 초기 데이터 강제 갱신
	if (APlayerState* PS = GetOwningPlayerState())
	{
		if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
		{
			if (TXT_CurrentGold)
			{
				int32 StartGold = InvComp->GetCurrentGold();
				TXT_CurrentGold->SetText(FText::AsNumber(StartGold));

				UE_LOG(LogTemp, Log, TEXT("[UI] Inventory Initialized with Gold: %d"), StartGold);
			}
		}
	}
}

bool UMyInventoryWidget::NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation)
{
	if (UMyDragDropOperation* ItemOp = Cast<UMyDragDropOperation>(InOperation))
	{
		UE_LOG(LogTemp, Warning, TEXT("[Drop] SourceType: %d, Item: %s"),
	(int)ItemOp->SourceType, ItemOp->PayloadItem ? *ItemOp->PayloadItem->ItemName.ToString() : TEXT("NULL"));
		
		// 상점에서 온 아이템이면 구매 시도
		if (ItemOp->SourceType == EMyDragSource::Shop && ItemOp->PayloadItem)
		{
			if (APlayerState* PS = GetOwningPlayerState())
			{
				if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
				{
					// 구매 함수 호출
					InvComp->TryBuyItem(ItemOp->PayloadItem);
					return true;
				}
			}
		}
	}
	return Super::NativeOnDrop(InGeometry, InDragDropEvent, InOperation);
}

