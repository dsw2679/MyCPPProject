// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyShopSlotWidget.h"
#include "Components/Button.h"
#include "Component/MyInventoryComponent.h"
#include "Experience/MyItemDefinition.h"
#include "GameFramework/PlayerState.h"

void UMyShopSlotWidget::UpdateWidget_Implementation(const UMyItemDefinition* ItemDef, int32 Count)
{
	// 상점 리스트 생성 시 전달받은 아이템 데이터를 저장
	CurrentItemDef = ItemDef;
}

void UMyShopSlotWidget::NativeConstruct()
{
	Super::NativeConstruct();
	// if (BTN_Buy)
	// {
	// 	// 중복 바인딩 방지 (이미 바인딩되어 있다면 해제 후 다시 하거나 체크)
	// 	BTN_Buy->OnClicked.RemoveDynamic(this, &UMyShopSlotWidget::OnBuyButtonClicked);
	// 	BTN_Buy->OnClicked.AddDynamic(this, &UMyShopSlotWidget::OnBuyButtonClicked);
	// }
	// else
	// {
	// 	UE_LOG(LogTemp, Error, TEXT("[ShopSlot] BTN_Buy is NULL! Name mismatch in BP?"));
	// }
}

void UMyShopSlotWidget::NativeOnInitialized()
{
	Super::NativeOnInitialized();
	if (BTN_Buy)
	{
		// 중복 바인딩 방지 (이미 바인딩되어 있다면 해제 후 다시 하거나 체크)
		BTN_Buy->OnClicked.RemoveDynamic(this, &UMyShopSlotWidget::OnBuyButtonClicked);
		BTN_Buy->OnClicked.AddDynamic(this, &UMyShopSlotWidget::OnBuyButtonClicked);
	}
	else
	{
		UE_LOG(LogTemp, Error, TEXT("[ShopSlot] BTN_Buy is NULL! Name mismatch in BP?"));
	}
}

void UMyShopSlotWidget::OnBuyButtonClicked()
{
	if (CurrentItemDef)
	{
		// PlayerState에서 인벤토리 컴포넌트 찾기
		if (APlayerState* PS = GetOwningPlayerState())
		{
			if (UMyInventoryComponent* InvComp = PS->FindComponentByClass<UMyInventoryComponent>())
			{
				// 이미 구현된 구매 함수 호출
				InvComp->TryBuyItem(CurrentItemDef);

				UE_LOG(LogTemp, Log, TEXT("[Shop] Clicked Buy for: %s"), *CurrentItemDef->ItemName.ToString());
			}
		}
	}
}
