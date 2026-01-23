// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "AttributeSet.h"
#include "AbilitySystemComponent.h"
#include "MyAttributeSet.generated.h"

#define ATTRIBUTE_ACCESSORS(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_PROPERTY_GETTER(ClassName, PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_GETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_SETTER(PropertyName) \
GAMEPLAYATTRIBUTE_VALUE_INITTER(PropertyName)
UCLASS()
class MYCPPPROJECT_API UMyAttributeSet : public UAttributeSet
{
	GENERATED_BODY()

public:
	UMyAttributeSet();
	
	virtual void PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue) override;

	// GameplayEffect가 실행된 후 처리 (데미지, 힐링 등 실제 적용 후)
	virtual void PostGameplayEffectExecute(const struct FGameplayEffectModCallbackData& Data) override;
	
protected:
	// Max 수치 변경 시 현재 수치를 비율에 맞게 조정해주는 헬퍼 함수
	void AdjustAttributeForMaxChange(
		FGameplayAttributeData& AffectedAttribute,
		const FGameplayAttributeData& MaxAttribute,
		float NewMaxValue,
		const FGameplayAttribute& AffectedAttributeProperty
	);
	
public:
	// 체력 관련
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth)

	// 마나 관련
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MP;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MP)

	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxMP;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxMP)

	// 아이덴티티 게이지
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData IdentityGage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, IdentityGage)

	// 무력화 게이지
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData Stagger;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Stagger)
	
	// 무력화 게이지
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData MaxStagger;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxStagger)
	
	// 피증 시너지
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData DamageScale;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, DamageScale)
	
	// 플레이어 공격력
	UPROPERTY(BlueprintReadOnly, Category = "Attributes")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, AttackPower)
	
	// 들어오는 데미지 값을 임시로 저장하는 메타 속성
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Damage);
	
};
