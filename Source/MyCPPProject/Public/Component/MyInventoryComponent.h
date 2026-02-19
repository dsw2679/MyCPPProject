// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "Message/MyInventoryMessages.h"
#include "MyInventoryComponent.generated.h"

class UMyItemDefinition;

UCLASS(BlueprintType, meta=(BlueprintSpawnableComponent))
class MYCPPPROJECT_API UMyInventoryComponent : public UActorComponent
{
	GENERATED_BODY()

public:
	UMyInventoryComponent();
	
	virtual void BeginPlay() override;

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool TryBuyItem(const UMyItemDefinition* ItemDef);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void EquipItemToSlot(const UMyItemDefinition* ItemDef, int32 SlotIndex);

	UFUNCTION(BlueprintCallable, Category = "Inventory")
	bool ConsumeItem(int32 SlotIndex);
	
	// 특정 아이템의 전체 보유 수량 확인
	UFUNCTION(BlueprintPure, Category = "MyInventory")
	int32 GetTotalItemCount(const UMyItemDefinition* ItemDef) const;
	
	const TMap<TObjectPtr<const UMyItemDefinition>, int32>& GetOwnedItems() const { return OwnedItems; }
	const TArray<TObjectPtr<const UMyItemDefinition>>& GetOwnedItemOrder() const { return OwnedItemOrder; }
	
	// (퀵슬롯)슬롯 A와 슬롯 B의 아이템을 서로 맞바꿉니다.
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void SwapQuickSlots(int32 SlotIndexA, int32 SlotIndexB);
	
	// 골드 값을 반환
	UFUNCTION(BlueprintPure, Category = "Inventory")
	int32 GetCurrentGold() const { return Gold; }
	
	// 아이템 리필
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	void RefillItems();
	
	void SetInventoryData(int32 InGold, const TMap<TObjectPtr<const UMyItemDefinition>, int32>& InOwnedItems, const TArray<FMyItemSlotInfo>& InSlots);
	
	// 배틀 아이템 슬롯 정보 반환 (Getter)
	const TArray<FMyItemSlotInfo>& GetBattleItemSlots() const { return BattleItemSlots; }
	
protected:
	void BroadcastInventoryMessage();
	
	FGameplayTag GetInputTagForSlot(int32 SlotIndex);

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Gold = 1500;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FMyItemSlotInfo> BattleItemSlots; // 4개 고정 슬롯
	
	// 상점에서 산 모든 아이템 보관함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<TObjectPtr<const UMyItemDefinition>, int32> OwnedItems;
	
	// 아이템이 추가된 순서를 저장하는 배열
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TArray<TObjectPtr<const UMyItemDefinition>> OwnedItemOrder;
		
};