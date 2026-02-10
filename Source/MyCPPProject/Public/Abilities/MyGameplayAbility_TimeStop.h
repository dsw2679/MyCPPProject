// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/MyGameplayAbility_Item.h"
#include "MyGameplayAbility_TimeStop.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility_TimeStop : public UMyGameplayAbility_Item
{
	GENERATED_BODY()
	
public:
	UFUNCTION(BlueprintCallable, Category = "Item")
	void ApplyTimeStop();

protected:
	
	UPROPERTY(EditDefaultsOnly, Category = "Item")
	TSubclassOf<UGameplayEffect> TimeStopEffectClass;
};
