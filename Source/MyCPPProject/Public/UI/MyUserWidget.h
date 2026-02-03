// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonUserWidget.h"
#include "AbilitySystemInterface.h"
#include "AttributeSet.h"
#include "GameplayEffectTypes.h"
#include "MyUserWidget.generated.h"

class UAbilitySystemComponent;

/**
 * GAS 연동을 위한 기본 위젯 클래스
 * AbilitySystemComponent를 설정하고 Attribute 변화를 감지하는 기능 제공
 */
UCLASS()
class MYCPPPROJECT_API UMyUserWidget : public UCommonUserWidget
{
	GENERATED_BODY()
public:
	// ASC 설정 및 초기화 (이 함수를 통해 어떤 캐릭터의 정보를 보여줄지 정합니다)
	UFUNCTION(BlueprintCallable, Category = "GAS")
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* InASC);

	// 특정 Attribute 값 변경 감지 등록
	virtual void BindAttribute(const FGameplayAttribute& Attribute);

	// Attribute 변경 시 호출됨 (C++ 내부 로직용)
	virtual void OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue);
	
	virtual void NativeConstruct() override;

	// BP에서 오버라이드하여 UI 업데이트를 처리할 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "GAS")
	void BP_OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue);

protected:
	UPROPERTY(BlueprintReadOnly, Category = "GAS", meta = (AllowPrivateAccess = "true"))
	TWeakObjectPtr<UAbilitySystemComponent> AbilitySystemComponent;

	// 델리게이트 핸들 관리 (위젯이 사라질 때 바인딩 해제용)
	TMap<FGameplayAttribute, FDelegateHandle> AttributeChangeDelegateHandles;
	
};
