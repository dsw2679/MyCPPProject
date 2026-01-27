// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_FindPlayer.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyBTService_FindPlayer : public UBTService
{
	GENERATED_BODY()
	
public:
	UMyBTService_FindPlayer();

protected:
	// 일정 시간마다 주기적으로 호출되는 함수
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	// 찾은 플레이어를 저장할 블랙보드 키 (TargetActor)
	UPROPERTY(EditAnywhere, Category = "AI")
	struct FBlackboardKeySelector TargetKey;

	// 감지 범위
	UPROPERTY(EditAnywhere, Category = "AI")
	float SearchRadius = 1500.0f;
};
