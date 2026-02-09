// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MyDragDropOperation.generated.h"

class UMyItemDefinition;

/**
 * 
 */

// 드래그 시작 위치를 구분하기 위한 Enum
UENUM(BlueprintType)
enum class EMyDragSource : uint8
{
	Shop,           // 상점 (구매용)
	Inventory,      // 인벤토리 (보유 목록)
	QuickSlot       // 전투 퀵슬롯 (HUD)
};

UCLASS()
class MYCPPPROJECT_API UMyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	// 드래그 중인 아이템 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<const UMyItemDefinition> PayloadItem;
	
	// 어디서 왔는가? (Shop / Inventory / QuickSlot)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop", meta = (ExposeOnSpawn = "true"))
	EMyDragSource SourceType = EMyDragSource::Inventory;

	// 몇 번 슬롯에서 왔는가? (퀵슬롯 간 스왑 등에 필요)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop", meta = (ExposeOnSpawn = "true"))
	int32 SourceIndex = -1;
};
