// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTService_FindPlayer.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Boss/MyBossAIController.h"

UMyBTService_FindPlayer::UMyBTService_FindPlayer()
{
	NodeName = "Find Player";
	Interval = 0.5f; // 0.5초마다 실행
}

void UMyBTService_FindPlayer::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);

	APawn* ControllingPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!ControllingPawn) return;

	// 월드의 PlayerCharacter(0번)를 찾음 (싱글플레이어이므로 유효)
	ACharacter* PlayerCharacter = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerCharacter)
	{
		float Distance = ControllingPawn->GetDistanceTo(PlayerCharacter);
		if (Distance <= SearchRadius)
		{
			// 범위 안에 있으면 블랙보드에 저장
			OwnerComp.GetBlackboardComponent()->SetValueAsObject(TargetKey.SelectedKeyName, PlayerCharacter);
		}
		else
		{
			// 멀어지면 타겟 해제 (선택 사항)
			// OwnerComp.GetBlackboardComponent()->ClearValue(TargetKey.SelectedKeyName);
		}
	}
}
