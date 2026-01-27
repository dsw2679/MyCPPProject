// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTTask_ActivateAbilityByTag.h"
#include "Boss/MyBossAIController.h"

UMyBTTask_ActivateAbilityByTag::UMyBTTask_ActivateAbilityByTag()
{
	NodeName = "Activate Ability By Tag";
}

EBTNodeResult::Type UMyBTTask_ActivateAbilityByTag::ExecuteTask(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory)
{
	AMyBossAIController* AICon = Cast<AMyBossAIController>(OwnerComp.GetAIOwner());
	if (!AICon) return EBTNodeResult::Failed;

	// 아까 만든 함수 호출
	bool bSuccess = AICon->AttemptBossSkill(SkillTag);

	return bSuccess ? EBTNodeResult::Succeeded : EBTNodeResult::Failed;
}
