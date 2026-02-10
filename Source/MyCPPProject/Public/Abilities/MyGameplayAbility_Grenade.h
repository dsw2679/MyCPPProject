// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MyGameplayAbility_Item.h"
#include "MyGameplayAbility_Grenade.generated.h"

class AMyGrenadeProjectile;
/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility_Grenade : public UMyGameplayAbility_Item
{
	GENERATED_BODY()
	
public:
	// 투사체 스폰 (BP에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Ability")
	void SpawnProjectile(TSubclassOf<AMyGrenadeProjectile> ProjectileClass, FVector TargetLocation);

protected:
	// 수류탄 폭발 효과 (블루프린트)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> GrenadeEffectClass;
	
	// 수류탄 데미지 배율 (기본 공격력 대비)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float DamageMultiplier = 1.0f;

	// 수류탄 무력화 피해 수치
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	float StaggerValue = 20.0f;

	// 태그들 (SetByCaller용)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTag DamageTag;

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	FGameplayTag StaggerTag;
	
};
