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
	
protected:
	void BroadcastInventoryMessage();
	
	FGameplayTag GetInputTagForSlot(int32 SlotIndex);

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 Gold = 1000;

	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FMyItemSlotInfo> BattleItemSlots; // 4개 고정 슬롯
	
	// 상점에서 산 모든 아이템 보관함
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TMap<TObjectPtr<const UMyItemDefinition>, int32> OwnedItems;
		
};