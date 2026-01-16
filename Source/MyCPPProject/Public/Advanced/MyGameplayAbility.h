// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMyGameplayAbility();

	// 스킬을 식별하기 위한 태그 (예: Ability.Skill.Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTag StartupInputTag;

	// '시전 중'인지 체크하기 위한 유틸리티 함수 (나중에 UI 등에 활용)
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsAbilityActive() const;
	
	// SetByCaller를 사용하여 이펙트를 적용하는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	FGameplayEffectSpecHandle MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude);

	// 위에서 만든 Spec을 실제로 자신에게 적용하는 함수
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	void ApplyEffectWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude);
	
	// [설정] 쿨타임 시간 (초 단위)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	float CooldownDuration = 0.0f;

	// [설정] 쿨타임용 태그 (SetByCaller로 시간을 넘겨줄 때 쓸 태그)
	// 기본값: SetByCaller.Cooldown
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	FGameplayTag CooldownTag;

	// [설정] 비용 목록 (태그 : 소모량)
	// 예: SetByCaller.Cost.MP : 20.0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cost")
	TMap<FGameplayTag, float> CostMap;

	// --- 오버라이드 함수 ---

	// 쿨타임 GE에 수치를 주입하여 반환
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// 비용 GE에 수치를 주입하여 반환
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// 비용 검사 (SetByCaller 값을 주입하여 검사)
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
};
