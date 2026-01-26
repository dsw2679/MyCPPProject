// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "GameplayEffectTypes.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "MyBossHealthWidget.generated.h"


class UAbilitySystemComponent;

/**
 * 보스 체력바 위젯
 */
UCLASS(Abstract)
class MYCPPPROJECT_API UMyBossHealthWidget : public UCommonUserWidget
{
	GENERATED_BODY()
	
public:
	// 보스의 ASC를 위젯에 연결하고 이벤트를 바인딩합니다.
	UFUNCTION(BlueprintCallable, Category = "BossUI")
	void SetAbilitySystemComponent(UAbilitySystemComponent* InASC);

protected:
	// 어트리뷰트 변경 시 호출될 콜백
	virtual void OnHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnMaxHealthChanged(const FOnAttributeChangeData& Data);
	virtual void OnStaggerChanged(const FOnAttributeChangeData& Data);
	virtual void OnMaxStaggerChanged(const FOnAttributeChangeData& Data);
	virtual void NativeConstruct() override;
	virtual void NativeDestruct() override;

	void OnBossSpawned(FGameplayTag Channel, const FMyBossMessageStruct& Payload);

	FGameplayMessageListenerHandle BossSpawnListenerHandle;
	
	// 메시지 수신 시 호출될 함수
	void OnHealthMessageReceived(FGameplayTag Channel, const FMyBossHealthMessage& Payload);

	// 리스너 핸들 관리
	FGameplayMessageListenerHandle HealthChangedListenerHandle;
	// 블루프린트에서 실제 UI를 갱신할 때 사용
	UFUNCTION(BlueprintImplementableEvent, Category = "BossUI")
	void UpdateHealth(float CurrentHealth, float MaxHealth);

	UFUNCTION(BlueprintImplementableEvent, Category = "BossUI")
	void UpdateStagger(float CurrentStagger, float MaxStagger);

	// 약참조로 ASC 보관
	UPROPERTY()
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;
};
