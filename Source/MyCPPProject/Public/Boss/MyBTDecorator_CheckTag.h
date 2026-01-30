// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTDecorator.h"
#include "MyBTDecorator_CheckTag.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyBTDecorator_CheckTag : public UBTDecorator
{
	GENERATED_BODY()
	
public:
	UMyBTDecorator_CheckTag();

protected:
	// 조건을 검사하는 핵심 함수
	virtual bool CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const override;

	// 검사할 태그
	UPROPERTY(EditAnywhere, Category = "Condition")
	FGameplayTag TagToCheck;
};
