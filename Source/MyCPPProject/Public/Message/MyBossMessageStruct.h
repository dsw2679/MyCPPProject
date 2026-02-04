// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"
#include "MyBossMessageStruct.generated.h"

class AActor;
class UAbilitySystemComponent;

USTRUCT(BlueprintType)
struct FMyBossMessageStruct
{
	GENERATED_BODY()
	
	// 이벤트 구분
	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	FGameplayTag Verb;

	// 보스 액터
	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	TObjectPtr<AActor> BossActor = nullptr;

	// 보스의 ASC (UI 연결용)
	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	TObjectPtr<UAbilitySystemComponent> BossASC = nullptr;
};

USTRUCT(BlueprintType)
struct FMyBossHealthMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	float CurrentHealth = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	float MaxHealth = 0.0f;
};

USTRUCT(BlueprintType)
struct FMyBossStaggerMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	float CurrentStagger = 0.0f;

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	float MaxStagger = 0.0f;
};

USTRUCT(BlueprintType)
struct FMyBossInfoMessage
{
	GENERATED_BODY()

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	FText BossName;

	UPROPERTY(BlueprintReadWrite, Category = "Boss")
	float EnrageTimeLimit = 0.0f;
	
};

USTRUCT(BlueprintType)
struct FMyDamageMessage
{
	GENERATED_BODY()

	// (위치 파악용)
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	TObjectPtr<AActor> TargetActor = nullptr;

	// 데미지 수치
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	float DamageAmount = 0.0f;

	// 크리티컬 여부 (UI 연출용)
	UPROPERTY(BlueprintReadWrite, Category = "Combat")
	bool bIsCritical = false;
	
};