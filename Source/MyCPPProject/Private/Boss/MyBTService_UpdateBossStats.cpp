// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTService_UpdateBossStats.h"
#include "BehaviorTree/BlackboardComponent.h"
#include "Boss/MyBossCharacter.h"
#include "AbilitySystemComponent.h"
#include "AIController.h"
#include "Advanced/MyAttributeSet.h"

UMyBTService_UpdateBossStats::UMyBTService_UpdateBossStats()
{
	NodeName = "Update Boss Stats";
	Interval = 0.2f;
}

void UMyBTService_UpdateBossStats::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	AMyBossCharacter* Boss = Cast<AMyBossCharacter>(OwnerComp.GetAIOwner()->GetPawn());
	if (!Boss) return;

	// 체력 업데이트
	if (UAbilitySystemComponent* ASC = Boss->GetAbilitySystemComponent())
	{
		// AttributeSet에서 체력 가져오기
		const UMyAttributeSet* AS = Cast<UMyAttributeSet>(ASC->GetAttributeSet(UMyAttributeSet::StaticClass()));
		if (AS)
		{
			float CurrentHP = AS->GetHealth();
			float MaxHP = AS->GetMaxHealth();
			float HPPercent = (MaxHP > 0.0f) ? (CurrentHP / MaxHP) : 1.0f;

			OwnerComp.GetBlackboardComponent()->SetValueAsFloat(HealthKey.SelectedKeyName, HPPercent);
		}
	}

	// 플레이어와의 거리 업데이트
	UObject* TargetObj = OwnerComp.GetBlackboardComponent()->GetValueAsObject(TEXT("TargetActor"));
	AActor* TargetActor = Cast<AActor>(TargetObj);
	if (TargetActor)
	{
		float Distance = Boss->GetDistanceTo(TargetActor);
		OwnerComp.GetBlackboardComponent()->SetValueAsFloat(DistanceKey.SelectedKeyName, Distance);
	}
}
