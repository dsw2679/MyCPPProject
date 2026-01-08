// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularPlayerState.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "GameplayEffectTypes.h"
#include "MyCPPProjectPlayerState.generated.h"

class UAbilitySystemComponent;
class UMyAttributeSet;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API AMyCPPProjectPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMyCPPProjectPlayerState();

	//~ Begin IAbilitySystemInterface
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	//~ End IAbilitySystemInterface

	UMyAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	UPROPERTY()
	TObjectPtr<UMyAttributeSet> AttributeSet;
};
