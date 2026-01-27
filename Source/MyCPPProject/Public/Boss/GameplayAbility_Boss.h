// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Advanced/MyGameplayAbility.h"
#include "GameplayAbility_Boss.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UGameplayAbility_Boss : public UMyGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 전조(Telegraph) 실행 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ShowTelegraphCircle(FGameplayTag CueTag, FVector CenterLocation, float Radius, float Duration);

	// 광역 공격(Area Attack) 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ApplyDamageToArea(FVector CenterLocation, float Radius, float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass);
	
	// 플레이어의 위치  계산 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	FVector GetPlayerLocation(float ForwardDistance = 0.0f);

protected:
	// 공격 대상(플레이어)을 찾기 위한 설정
	UPROPERTY(EditDefaultsOnly, Category = "Boss Ability")
	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
};
