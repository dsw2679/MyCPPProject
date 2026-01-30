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
	
	virtual void GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const override;
	
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
	// 이동 속도
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MoveSpeed, Category = "Attributes")
	FGameplayAttributeData MoveSpeed;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MoveSpeed)

	UFUNCTION()
	virtual void OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed);
	
	// 체력 관련
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Health, Category = "Attributes")
	FGameplayAttributeData Health;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Health)
	
	UFUNCTION()
	virtual void OnRep_Health(const FGameplayAttributeData& OldHealth);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxHealth, Category = "Attributes")
	FGameplayAttributeData MaxHealth;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxHealth)
	
	UFUNCTION()
	virtual void OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth);

	// 마나 관련
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MP, Category = "Attributes")
	FGameplayAttributeData MP;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MP)
	
	UFUNCTION()
	virtual void OnRep_MP(const FGameplayAttributeData& OldMP);

	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxMP, Category = "Attributes")
	FGameplayAttributeData MaxMP;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxMP)
	
	UFUNCTION()
	virtual void OnRep_MaxMP(const FGameplayAttributeData& OldMaxMP);

	// 아이덴티티 게이지
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_IdentityGage, Category = "Attributes")
	FGameplayAttributeData IdentityGage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, IdentityGage)
	
	UFUNCTION()
	virtual void OnRep_IdentityGage(const FGameplayAttributeData& OldIdentityGage);

	// 무력화 게이지
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_Stagger, Category = "Attributes")
	FGameplayAttributeData Stagger;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Stagger)
	
	UFUNCTION()
	virtual void OnRep_Stagger(const FGameplayAttributeData& OldStagger);
	
	// 최대 무력화 게이지
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_MaxStagger, Category = "Attributes")
	FGameplayAttributeData MaxStagger;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, MaxStagger)
	
	UFUNCTION()
	virtual void OnRep_MaxStagger(const FGameplayAttributeData& OldMaxStagger);
	
	// 피증 시너지
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_DamageScale, Category = "Attributes")
	FGameplayAttributeData DamageScale;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, DamageScale)
	
	UFUNCTION()
	virtual void OnRep_DamageScale(const FGameplayAttributeData& OldDamageScale);
	
	// 플레이어 공격력
	UPROPERTY(BlueprintReadOnly, ReplicatedUsing = OnRep_AttackPower, Category = "Attributes")
	FGameplayAttributeData AttackPower;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, AttackPower)
	
	UFUNCTION()
	virtual void OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower);
	
	// 들어오는 데미지 값을 임시로 저장하는 메타 속성
	UPROPERTY(BlueprintReadOnly, Category = "Lyra|Health", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData Damage;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, Damage);
	
	// 들어오는 무력화 값을 임시로 저장하는 메타 속성
	UPROPERTY(BlueprintReadOnly, Category = "Attributes", Meta = (AllowPrivateAccess = true))
	FGameplayAttributeData IncomingStagger;
	ATTRIBUTE_ACCESSORS(UMyAttributeSet, IncomingStagger);
	
};
