// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MyGameplayAbility_Item.h"
#include "MyGameplayAbility_Potion.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility_Potion : public UMyGameplayAbility_Item
{
	GENERATED_BODY()
	
protected:
	//virtual void ActivateItemEffect() override;

	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> HealEffectClass;
	
};
