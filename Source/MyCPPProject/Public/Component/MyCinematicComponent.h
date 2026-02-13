// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "MyCinematicComponent.generated.h"

class ALevelSequenceActor;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class MYCPPPROJECT_API UMyCinematicComponent : public UActorComponent
{
	GENERATED_BODY()

public:	
	UMyCinematicComponent();
	
	// 인트로 연출 시작
	void PlayIntroSequence();
protected:
	UFUNCTION()
	void OnSequenceFinished();

	// 카메라 블렌딩 완료 후 조작 복구
	void RestorePlayerControl();

private:
	UPROPERTY()
	TObjectPtr<ALevelSequenceActor> CurrentSequenceActor;

	float BlendTime = 2.0f;
		
};
