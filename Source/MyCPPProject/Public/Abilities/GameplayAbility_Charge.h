// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Advanced/MyGameplayAbility.h"
#include "GameplayAbility_Charge.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UGameplayAbility_Charge : public UMyGameplayAbility
{
	GENERATED_BODY()
	
public:
	UGameplayAbility_Charge();

	//~UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	//~End UGameplayAbility Interface

protected:
	// 차징 시작 시간을 기록
	UFUNCTION(BlueprintCallable, Category = "Charge")
	void StartCharge();

	// 차징 종료 및 배율 계산 (2초 넘으면 1.0, 아니면 0.5 등)
	// 반환값: 데미지 배율
	UFUNCTION(BlueprintCallable, Category = "Charge")
	float EndCharge();

	// 현재 차징 진행도 반환 (0.0 ~ 1.0) - UI용
	UFUNCTION(BlueprintCallable, Category = "Charge")
	float GetChargePercent() const;

public:
	// 완충에 필요한 시간 (초)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	float FullChargeTime = 2.0f;

	// 완충 실패 시 데미지 배율 (예: 0.5 = 50%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Charge")
	float MinChargeMultiplier = 0.5f;

private:
	// 차징 시작 시간 (서버 시간 기준)
	double StartTime = 0.0;
	
};
