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
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Item")
	int32 SlotIndex = 0;
	
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
protected:
	// 아이템 효과 구현 (블루프린트 작성)
	UFUNCTION(BlueprintImplementableEvent, Category = "Item")
	void OnItemEffectTriggered();
};
