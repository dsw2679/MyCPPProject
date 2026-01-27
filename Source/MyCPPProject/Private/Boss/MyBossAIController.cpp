// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBossAIController.h"
#include "Boss/MyBossCharacter.h"
#include "AbilitySystemComponent.h"

AMyBossAIController::AMyBossAIController()
{
}

bool AMyBossAIController::AttemptBossSkill(FGameplayTag SkillTag)
{
	// 1. 내가 조종하는 폰이 보스인지 확인
	AMyBossCharacter* BossBot = Cast<AMyBossCharacter>(GetPawn());
	if (!BossBot) return false;

	// 2. 보스의 GAS 컴포넌트 가져오기
	UAbilitySystemComponent* ASC = BossBot->GetAbilitySystemComponent();
	if (!ASC) return false;

	// 3. 해당 태그로 스킬 발동 시도 (태그 컨테이너로 변환하여 전달)
	// TryActivateAbilitiesByTag는 해당 태그를 가진 '부여된 스킬'을 찾아 실행합니다.
	return ASC->TryActivateAbilitiesByTag(FGameplayTagContainer(SkillTag));
}

void AMyBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	if (BossBehaviorTree)
	{
		RunBehaviorTree(BossBehaviorTree);
	}
}
