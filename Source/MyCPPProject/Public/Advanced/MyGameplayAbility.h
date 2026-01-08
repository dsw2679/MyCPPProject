// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMyGameplayAbility();

	// 스킬을 식별하기 위한 태그 (예: Ability.Skill.Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTag StartupInputTag;

	// '시전 중'인지 체크하기 위한 유틸리티 함수 (나중에 UI 등에 활용)
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsAbilityActive() const;
};
