// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayAbilitySpec.h"
#include "MyInventoryMessages.generated.h"

class UMyItemDefinition;

/**
 * 배틀 아이템 슬롯의 현재 상태를 담는 구조체
 */
USTRUCT(BlueprintType)
struct FMyItemSlotInfo
{
	GENERATED_BODY()

	// 아이템 정의 데이터 (이름, 아이콘 등 참조용)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<const UMyItemDefinition> ItemDef = nullptr;

	// 현재 슬롯에 남은 개수
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 CurrentCount = 0;

	// 최대 소지 가능 개수 (UI 게이지나 텍스트 표시용)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 MaxStackCount = 0;
	
	// 부여된 어빌리티를 나중에 지우기 위해 핸들을 저장
	FGameplayAbilitySpecHandle AbilityHandle;
};


/**
 * 인벤토리 변경시 전송되는 메시지 
 */

USTRUCT(BlueprintType)
struct FMyInventoryMessage
{
	GENERATED_BODY()

	// 메시지를 보낸 주체 (주로 PlayerState)
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<AActor> OwnerActor = nullptr;

	// 현재 보유 중인 골드
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	int32 CurrentGold = 0;

	// 1~4번 배틀 아이템 슬롯의 최신 정보
	UPROPERTY(BlueprintReadOnly, Category = "Inventory")
	TArray<FMyItemSlotInfo> BattleItemSlots;
};