// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyGameplayAbility.h"

UMyGameplayAbility::UMyGameplayAbility()
{
	// 기본적으로 스킬은 인스턴스화되어 캐릭터마다 독립적으로 실행되도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UMyGameplayAbility::IsAbilityActive() const
{
	// 현재 능력이 활성화(시전 중) 상태인지 확인
	return IsActive();
}