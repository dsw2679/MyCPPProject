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