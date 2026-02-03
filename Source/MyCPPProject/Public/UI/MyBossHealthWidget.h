// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "MyBossHealthWidget.generated.h"


class UAbilitySystemComponent;

/**
 * 보스 체력바 위젯
 */
UCLASS(Abstract)
class MYCPPPROJECT_API UMyBossHealthWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
public:
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

protected:
	// 메시지 수신 리스너 핸들 (위젯 파괴 시 등록 해제용)
	FGameplayMessageListenerHandle HealthMessageHandle;

	// 체력 변경 메시지 수신 시 호출되는 함수
	void OnHealthMessageReceived(FGameplayTag Channel, const FMyBossHealthMessage& Message);

	// BP에서 실제 프로그레스 바(Progress Bar) 등을 갱신할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Boss UI")
	void BP_UpdateBossHealth(float CurrentHealth, float MaxHealth);

	// (확장용) 보스 이름 설정 등을 위한 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Boss UI")
	void BP_SetBossInfo(const FString& BossName);
};
