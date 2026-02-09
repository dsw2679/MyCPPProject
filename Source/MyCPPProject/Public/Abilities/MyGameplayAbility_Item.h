// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "MyGameplayAbility_Item.generated.h"

/**
 * GAS 기반 아이템 사용 어빌리티 구현
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility_Item : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMyGameplayAbility_Item();
	
	// 블루프린트에서 호출할 수 있는 아이템 소모 함수
	UFUNCTION(BlueprintCallable, Category = "Item")
	bool ConsumeItem();
	
protected:
	
	// 쿨타임 시간 (초 단위)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	float CooldownDuration = 0.0f;

	// 쿨타임용 태그 (SetByCaller로 시간을 넘겨줄 때 쓸 태그)
	// 기본값: SetByCaller.Cooldown
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	FGameplayTag CooldownTag;

	// 쿨타임 태그를 반환하기 위한 임시 저장소
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
	
	// 쿨타임 태그 반환 로직 재정의
	virtual const FGameplayTagContainer* GetCooldownTags() const override;

	// 실제 쿨타임 이펙트를 적용할 때 시간을 주입
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;

	// 입력 태그를 통해 슬롯 인덱스 계산
	int32 GetSlotIndexFromInputTag() const;
	
};
