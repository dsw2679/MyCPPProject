// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Charge.h"

UGameplayAbility_Charge::UGameplayAbility_Charge()
{
	FullChargeTime = 2.0f;
	MinChargeMultiplier = 0.5f;
}

void UGameplayAbility_Charge::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	StartCharge();
}

void UGameplayAbility_Charge::StartCharge()
{
	// 현재 월드 시간 기록
	if (UWorld* World = GetWorld())
	{
		StartTime = World->GetTimeSeconds();
		
		UE_LOG(LogTemp, Warning, TEXT("[Charge] Start Time: %f"), StartTime);
	}
}

float UGameplayAbility_Charge::EndCharge()
{
	if (UWorld* World = GetWorld())
	{
		double CurrentTime = World->GetTimeSeconds();
		double ChargeDuration = CurrentTime - StartTime;

		// 디버그 로그
		UE_LOG(LogTemp, Warning, TEXT("[Charge] End Time: %f / Duration: %f / Required: %f"), CurrentTime, ChargeDuration, FullChargeTime);

		// 완충 시간보다 길게 눌렀다면?
		if (ChargeDuration >= FullChargeTime)
		{
			return 1.0f; // 100% 위력
		}
	}

	// 완충 실패
	return MinChargeMultiplier; // 50% 위력
}

float UGameplayAbility_Charge::GetChargePercent() const
{
	if (UWorld* World = GetWorld())
	{
		double CurrentTime = World->GetTimeSeconds();
		double ChargeDuration = CurrentTime - StartTime;

		// 0.0 ~ 1.0 사이로 클램핑하여 반환
		return FMath::Clamp(ChargeDuration / FullChargeTime, 0.0f, 1.0f);
	}
	return 0.0f;
}
