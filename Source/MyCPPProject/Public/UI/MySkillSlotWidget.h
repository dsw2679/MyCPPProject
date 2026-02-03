// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "MySkillSlotWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMySkillSlotWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
protected:
	// 현재 남은 시간
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	float CurrentRemainingTime = 0.0f;

	// 총 쿨타임 시간 (최대값)
	UPROPERTY(BlueprintReadOnly, Category = "Skill")
	float TotalCooldownTime = 1.0f;
	
public:
	UFUNCTION(BlueprintCallable)
	void UpdateCooldown(bool bIsCooldown, float RemainingTime, float TotalDuration); // BP에서 구현하거나 C++ 로직 추가

	// 이 슬롯이 담당하는 태그 (에디터에서 설정)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Skill")
	FGameplayTag SkillTag;

	// BP 구현 이벤트
	UFUNCTION(BlueprintImplementableEvent)
	void BP_UpdateCooldown(bool bIsCooldown, float RemainingTime);
	
};
