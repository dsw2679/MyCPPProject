// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayCueNotify_Actor.h"
#include "MyGameplayCueActor_Looping.generated.h"

/**
 * 캐릭터에 캐스팅해 소켓에 gc를 붙임
 */
UCLASS()
class MYCPPPROJECT_API AMyGameplayCueActor_Looping : public AGameplayCueNotify_Actor
{
	GENERATED_BODY()
	
public:
	// 소켓 이름 설정
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayCue")
	FName SocketName;

protected:
	// GC 활성화 시 자동 호출되는 함수 오버라이드
	virtual void HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType, const FGameplayCueParameters& Parameters) override;
};
