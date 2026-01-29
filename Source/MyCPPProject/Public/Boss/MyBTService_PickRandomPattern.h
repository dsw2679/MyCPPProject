// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_PickRandomPattern.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyBTService_PickRandomPattern : public UBTService
{
	GENERATED_BODY()
	
public:
	UMyBTService_PickRandomPattern();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector RandomIndexKey;

	// 패턴의 개수 (0 ~ Max-1 까지 랜덤 생성)
	UPROPERTY(EditAnywhere, Category = "AI")
	int32 MaxPatternCount = 3;

	// 얼마나 자주 주사위를 굴릴지 (보통 공격 쿨타임과 비슷하게)
	UPROPERTY(EditAnywhere, Category = "AI")
	float PickInterval = 2.0f;

	// 이 태그를 가지고 있으면 랜덤 주사위를 굴리지 않고 특정 패턴을 강제합니다.
	UPROPERTY(EditAnywhere, Category = "AI")
	FGameplayTag ForcePatternTag;

	// 강제할 패턴의 인덱스 번호 (SwingCombo가 4번이라면 4를 입력)
	UPROPERTY(EditAnywhere, Category = "AI")
	int32 ForcePatternIndex = 4;
	
private:
	float TimeSinceLastPick = 0.0f;
};
