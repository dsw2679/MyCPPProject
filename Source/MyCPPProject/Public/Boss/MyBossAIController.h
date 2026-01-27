// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "GameplayTagContainer.h"
#include "MyBossAIController.generated.h"

class UBehaviorTree;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API AMyBossAIController : public AModularAIController
{
	GENERATED_BODY()
public:
	AMyBossAIController();

	// 블루프린트(비헤이비어 트리)에서 호출할 함수: "특정 태그의 스킬을 써라"
	UFUNCTION(BlueprintCallable, Category = "Boss AI")
	bool AttemptBossSkill(FGameplayTag SkillTag);
	
protected:
	virtual void OnPossess(APawn* InPawn) override;
	
	UPROPERTY(EditDefaultsOnly, Category = "Boss AI")
	TObjectPtr<UBehaviorTree> BossBehaviorTree;
	
};
