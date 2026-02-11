// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyLevelTransitionTrigger.generated.h"

class UBoxComponent;

/**
 * 상점 레벨에서 전투 레벨로 이동하기 위한 트리거 액터
 */
UCLASS()
class MYCPPPROJECT_API AMyLevelTransitionTrigger : public AActor
{
	GENERATED_BODY()
public:	
	AMyLevelTransitionTrigger();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 이동할 타겟 레벨 (Soft Object Path로 메모리 효율성 확보)
	UPROPERTY(EditAnywhere, Category = "Level")
	TSoftObjectPtr<UWorld> TargetLevel;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp, 
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

};
