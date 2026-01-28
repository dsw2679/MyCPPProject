// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_BossCollision.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UAnimNotifyState_BossCollision : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	// Notify 시작 시 호출
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;

	// Notify 종료 시 호출
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;

protected:
	// 이 구간 동안 보스에게 부여할 태그 (예: State.Boss.Dashing)
	UPROPERTY(EditAnywhere, Category = "GameplayTags")
	FGameplayTag Tag;
};
