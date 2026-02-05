// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "Message/MyInventoryMessages.h"
#include "MyInventoryWidget.generated.h"

class UUniformGridPanel;
class UMyItemDefinition;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyInventoryWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
public:
	/** 인벤토리 갱신 (메시지 수신 시 호출) */
	UFUNCTION(BlueprintCallable, Category = "Inventory UI")
	void RefreshInventory(const FMyInventoryMessage& Message);
	
	void OnInventoryUpdated(FGameplayTag Channel, const FMyInventoryMessage& Message);
	
	void NativeConstruct() override;
	
protected:
	
	// 인벤토리 슬롯들이 들어갈 그리드 패널
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Inventory UI")
	TObjectPtr<UUniformGridPanel> InventoryGrid;

	// 생성할 슬롯 위젯의 클래스
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI")
	TSubclassOf<UUserWidget> SlotWidgetClass;

	// 한줄에 표시할 슬롯 개수
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Inventory UI")
	int32 ColumnCount = 5;
};
