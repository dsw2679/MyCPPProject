// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTService_PickRandomPattern.h"

#include "BehaviorTree/BlackboardComponent.h"

UMyBTService_PickRandomPattern::UMyBTService_PickRandomPattern()
{
	NodeName = "Pick Random Pattern";
	Interval = 0.5f; // 자주 체크하되, 실제 변경은 내부 타이머로 제어
	RandomIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UMyBTService_PickRandomPattern, RandomIndexKey));
}

void UMyBTService_PickRandomPattern::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	int32 RandomVal = FMath::RandRange(0, MaxPatternCount - 1);
	OwnerComp.GetBlackboardComponent()->SetValueAsInt(RandomIndexKey.SelectedKeyName, RandomVal);
}
