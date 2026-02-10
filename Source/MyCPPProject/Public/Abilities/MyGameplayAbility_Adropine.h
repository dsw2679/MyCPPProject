// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MyGameplayAbility_Item.h"
#include "MyGameplayAbility_Adropine.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility_Adropine : public UMyGameplayAbility_Item
{
	GENERATED_BODY()
	
protected:
	// 공격력/이속 버프 GE
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> BuffEffectClass;

	// 체력 감소 GE (Cost로 처리해도 되지만, 즉발 효과로 구현 가능)
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> HPDrainEffectClass;

	// 블루프린트에서 호출할 실행 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	void ApplyAdropineEffect();
};
