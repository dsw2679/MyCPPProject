// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MySkillSlotWidget.h"
#include "Message/MyInventoryMessages.h"
#include "MyItemSlotWidget.generated.h"

class UImage;
class UTextBlock;

/**
 * 아이템 위젯
 */
UCLASS()
class MYCPPPROJECT_API UMyItemSlotWidget : public UMySkillSlotWidget
{
	GENERATED_BODY()
	
public:
	/** 이 슬롯의 번호 (0~3) */
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item UI", meta = (ExposeOnSpawn = "true"))
	int32 MySlotIndex = 0;
	
	// 인벤토리 메시지로부터 받은 데이터를 UI에 반영
	UFUNCTION(BlueprintCallable, Category = "Item UI")
	void UpdateItemData(const FMyItemSlotInfo& SlotInfo);
	
	virtual bool NativeOnDrop(const FGeometry& InGeometry, const FDragDropEvent& InDragDropEvent, UDragDropOperation* InOperation) override;
	
	virtual void NativeConstruct() override;
	void OnInventoryUpdated(FGameplayTag Channel, const FMyInventoryMessage& Message);
	
	// 이 위젯이 드래그 중인 비주얼인지 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Item UI", meta = (ExposeOnSpawn = "true"))
	bool bIsDragVisual = false;

protected:
	// 에디터 위젯과 연결 (이름이 일치해야 함) 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UImage> IMG_ItemIcon;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_CountText;
	
	// 현재 슬롯에 장착된 아이템 정보를 보관 (드래그 시 사용)
	UPROPERTY(BlueprintReadOnly, Category = "Item UI")
	TObjectPtr<const UMyItemDefinition> CurrentItemDef;
};
