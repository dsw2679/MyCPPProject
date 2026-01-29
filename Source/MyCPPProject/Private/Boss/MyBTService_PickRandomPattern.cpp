// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/MyBTService_PickRandomPattern.h"

#include "BehaviorTree/BlackboardComponent.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "AIController.h"

UMyBTService_PickRandomPattern::UMyBTService_PickRandomPattern()
{
	NodeName = "Pick Random Pattern";
	Interval = 0.5f; // 자주 체크하되, 실제 변경은 내부 타이머로 제어
	RandomIndexKey.AddIntFilter(this, GET_MEMBER_NAME_CHECKED(UMyBTService_PickRandomPattern, RandomIndexKey));
}

void UMyBTService_PickRandomPattern::TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds)
{
	Super::TickNode(OwnerComp, NodeMemory, DeltaSeconds);
	
	APawn* OwnerPawn = OwnerComp.GetAIOwner()->GetPawn();
	if (!OwnerPawn) return;

	// 보스의 GAS 컴포넌트 가져오기
	UAbilitySystemComponent* ASC = nullptr;
	if (IAbilitySystemInterface* ASIPawn = Cast<IAbilitySystemInterface>(OwnerPawn))
	{
		ASC = ASIPawn->GetAbilitySystemComponent();
	}

	// 강제 패턴 태그가 있는지 확인
	if (ASC && ForcePatternTag.IsValid() && ASC->HasMatchingGameplayTag(ForcePatternTag))
	{
		// 태그가 있으면 무조건 강제 인덱스로 설정
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(RandomIndexKey.SelectedKeyName, ForcePatternIndex);
		return; // 아래의 랜덤 로직을 타지 않고 종료
	}

	// 기존 랜덤 로직 (태그가 없을 때만 실행됨)
	TimeSinceLastPick += DeltaSeconds;
	if (TimeSinceLastPick >= PickInterval)
	{
		TimeSinceLastPick = 0.0f;
		int32 RandomVal = FMath::RandRange(0, MaxPatternCount - 1);
		OwnerComp.GetBlackboardComponent()->SetValueAsInt(RandomIndexKey.SelectedKeyName, RandomVal);
	}
}
