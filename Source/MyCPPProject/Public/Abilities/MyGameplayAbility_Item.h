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
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;

protected:
	UPROPERTY(EditDefaultsOnly, BlueprintReadWrite, Category = "Item")
	int32 SlotIndex = 0;

	// 아이템 실제 효과 구현 (C++ 오버라이드용)
	virtual void ActivateItemEffect();

	// 입력 태그를 통해 슬롯 인덱스 계산
	int32 GetSlotIndexFromInputTag() const;

	// 아이템 효과 구현 (블루프린트 작성)
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnItemEffectTriggered();
};
