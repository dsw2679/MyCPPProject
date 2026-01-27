// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "BehaviorTree/BTService.h"
#include "MyBTService_UpdateBossStats.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyBTService_UpdateBossStats : public UBTService
{
	GENERATED_BODY()

public:
	UMyBTService_UpdateBossStats();

protected:
	virtual void TickNode(UBehaviorTreeComponent& OwnerComp, uint8* NodeMemory, float DeltaSeconds) override;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector HealthKey;

	UPROPERTY(EditAnywhere, Category = "AI")
	FBlackboardKeySelector DistanceKey;
};
