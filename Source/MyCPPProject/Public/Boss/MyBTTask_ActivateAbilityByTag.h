// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTTaskNode.h"
#include "GameplayTagContainer.h"
#include "MyBTTask_ActivateAbilityByTag.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyBTTask_ActivateAbilityByTag : public UBTTaskNode
{
	GENERATED_BODY()
	
public:
	UMyBTTask_ActivateAbilityByTag();

	// Task 실행 시 호출되는 함수
	virtual EBTNodeResult::Type ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) override;

protected:
	// 에디터에서 어떤 태그의 스킬을 쓸지 선택할 변수
	UPROPERTY(EditAnywhere, Category = "GAS")
	FGameplayTag SkillTag;
};
