// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "MyBossEncounterTrigger.generated.h"

class UBoxComponent;
class ALevelSequenceActor;

UCLASS()
class MYCPPPROJECT_API AMyBossEncounterTrigger : public AActor
{
	GENERATED_BODY()
	
public:	
	AMyBossEncounterTrigger();

protected:
	UPROPERTY(VisibleAnywhere, Category = "Components")
	TObjectPtr<UBoxComponent> TriggerBox;

	// 재생할 보스 등장 시퀀스
	UPROPERTY(EditAnywhere, Category = "Boss")
	TObjectPtr<ALevelSequenceActor> BossSequenceActor;
	
	// 시퀀스 종료 후 플레이어가 이동할 위치 (건물 내부)
	UPROPERTY(EditAnywhere, Category = "Boss")
	FVector AfterCutsceneLocation;

	// 추가할 보스 UI 클래스 (WBP_BossInfo)
	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<UUserWidget> BossInfoWidgetClass;

	// 중복 실행 방지용 플래그
	bool bHasTriggered = false;

	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);
	
	UFUNCTION()
	void OnSequenceFinished();

};
