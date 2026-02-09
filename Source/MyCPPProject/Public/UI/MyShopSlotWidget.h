// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "UI/MyItemWidgetInterface.h"
#include "MyShopSlotWidget.generated.h"

class UButton;
class UMyItemDefinition;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyShopSlotWidget : public UMyUserWidget , public IMyItemWidgetInterface
{
	GENERATED_BODY()
	
public:
	virtual void UpdateWidget_Implementation(const UMyItemDefinition* ItemDef, int32 Count) override;
	
protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnInitialized() override;

	// 구매 버튼 클릭 시 실행될 함수
	UFUNCTION()
	void OnBuyButtonClicked();

	// 현재 이 슬롯이 팔고 있는 아이템 정보
	UPROPERTY(BlueprintReadOnly, Category = "Shop")
	TObjectPtr<const UMyItemDefinition> CurrentItemDef;

	// [BindWidget] 블루프린트의 버튼 이름과 일치해야 함
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UButton> BTN_Buy;
};
