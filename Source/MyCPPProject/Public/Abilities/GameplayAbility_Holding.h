// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Advanced/MyGameplayAbility.h"
#include "GameplayTagContainer.h"
#include "GameplayAbility_Holding.generated.h"

class UAbilityTask_WaitInputRelease;
/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UGameplayAbility_Holding : public UMyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Holding();
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData)override;
virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
	const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled)override;
	
	protected:
	// 최대 홀딩 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float MaxHoldDuration = 3.0f;

	// 데미지 주기 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float DamageInterval = 0.5f;

	// 데미지 범위 (반경)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float DamageRadius = 300.0f;

	// S스킬 데미지 배율 (예: 0.5 = 공격력의 50%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	float DamageMultiplier = 0.5f;
	
	// SetByCaller 태그
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FGameplayTag DamageEventTag;
	
	// 적용할 데미지 GE
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	TSubclassOf<UGameplayEffect> DamageEffectClass;

	// 몽타주
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	UAnimMontage* LoopMontage;
	
	// 시작할 때 재생할 섹션 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FName StartSection = FName("SkillStart");

	// 종료할 때 점프할 섹션 이름
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Settings")
	FName EndSection = FName("End");
	
	FTimerHandle DamageTimerHandle;
	FTimerHandle DurationTimerHandle;

	// 매 틱마다 호출될 함수
	UFUNCTION()
	void OnTickDamage();

	// 키를 뗐을 때
	UFUNCTION()
	void OnInputReleased(float TimeHeld);

	// 태스크
	UPROPERTY()
	TObjectPtr<UAbilityTask_WaitInputRelease> InputReleaseTask;
};
