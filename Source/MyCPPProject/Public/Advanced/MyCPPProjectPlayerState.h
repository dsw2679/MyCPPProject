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
class UMyInventoryComponent;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API AMyCPPProjectPlayerState : public AModularPlayerState, public IAbilitySystemInterface
{
	GENERATED_BODY()
	
public:
	AMyCPPProjectPlayerState();
	
	virtual UAbilitySystemComponent* GetAbilitySystemComponent() const override;
	
	// 인벤토리 컴포넌트 반환 함수 (Getter)
	UFUNCTION(BlueprintCallable, Category = "Inventory")
	UMyInventoryComponent* GetInventoryComponent() const { return InventoryComponent; }
	
	UMyAttributeSet* GetAttributeSet() const { return AttributeSet; }

protected:
	UPROPERTY(VisibleAnywhere, Category = "GAS")
	TObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 인벤토리 컴포넌트 추가
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Inventory")
	TObjectPtr<UMyInventoryComponent> InventoryComponent;
	
	UPROPERTY()
	TObjectPtr<UMyAttributeSet> AttributeSet;
	
	// Seamless Travel 시 데이터를 복사하기 위한 함수 오버라이드
	virtual void CopyProperties(APlayerState* PlayerState) override;
};
