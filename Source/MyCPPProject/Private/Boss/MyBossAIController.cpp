// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBossAIController.h"

AMyBossAIController::AMyBossAIController()
{
}

void AMyBossAIController::OnPossess(APawn* InPawn)
{
	Super::OnPossess(InPawn);
	
	// TODO: 보스용 비헤이비어 트리 실행 로직 (나중에 추가)
	// RunBehaviorTree(BTAsset);
}
