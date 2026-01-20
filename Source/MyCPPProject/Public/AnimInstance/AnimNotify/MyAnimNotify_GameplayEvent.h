// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotify.h"
#include "GameplayTagContainer.h"
#include "MyAnimNotify_GameplayEvent.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyAnimNotify_GameplayEvent : public UAnimNotify
{
	GENERATED_BODY()
	
public:
	// 에디터에서 선택할 이벤트 태그 (예: Event.Montage.SpawnThunder)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag EventTag;

	// 노티파이가 실행될 때 호출되는 함수 오버라이드
	virtual void Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
};
