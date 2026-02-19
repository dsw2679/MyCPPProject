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
	
	// 보스 소환
	UPROPERTY(EditAnywhere, Category = "Boss")
	TSubclassOf<APawn> BossClass;

	// 보스 소환 위치
	UPROPERTY(EditAnywhere, Category = "Boss", Meta = (MakeEditWidget = true))
	FTransform BossSpawnTransform;
	
	UPROPERTY(VisibleAnywhere, Category = "Boss")
	TObjectPtr<USceneComponent> BossSpawnLocationComponent;

	// 재생할 보스 등장 시퀀스
	UPROPERTY(EditAnywhere, Category = "Boss")
	TObjectPtr<ALevelSequenceActor> BossSequenceActor;
	
	// 타이머 핸들
	FTimerHandle CutsceneTimerHandle;
	
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
	
	// 구역을 나갔을 때 타이머를 취소하기 위함
	UFUNCTION()
	void OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
		UPrimitiveComponent* OtherComp, int32 OtherBodyIndex);
	
	// 실제 컷신과 소환을 시작하는 함수
	void StartBossCutscene();
	
	UFUNCTION()
	void OnSequenceFinished();
	
	// 보스전에서 재생할 새로운 BGM
	UPROPERTY(EditAnywhere, Category = "Boss|Audio")
	TObjectPtr<USoundBase> BossBGM;

	// BGM 제어를 위한 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Boss|Audio")
	TObjectPtr<UAudioComponent> BossBGMComponent;

};
