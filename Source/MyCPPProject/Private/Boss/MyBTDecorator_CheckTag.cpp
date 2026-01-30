// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTDecorator_CheckTag.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

UMyBTDecorator_CheckTag::UMyBTDecorator_CheckTag()
{
	NodeName = "Check Gameplay Tag (Custom)";

	// 중요: 이 데코레이터가 흐름 제어(Abort)를 할 수 있도록 허용
	bAllowAbortLowerPri = true;
	bAllowAbortNone = true;
	bAllowAbortChildNodes = true;
	FlowAbortMode = EBTFlowAbortMode::LowerPriority; // 기본값
}

bool UMyBTDecorator_CheckTag::CalculateRawConditionValue(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory) const
{
	// AI Controller 가져오기
	AAIController* AIController = OwnerComp.GetAIOwner();
	if (!AIController) return false;

	// 관리 중인 Pawn 가져오기
	APawn* ControllingPawn = AIController->GetPawn();
	if (!ControllingPawn) return false;

	// ASC(Ability System Component) 가져오기
	IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(ControllingPawn);
	if (ASI)
	{
		UAbilitySystemComponent* ASC = ASI->GetAbilitySystemComponent();
		if (ASC)
		{
			// 태그 검사 (정확히 매칭되는지)
			return ASC->HasMatchingGameplayTag(TagToCheck);
		}
	}

	return false;
}
