// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "MyDamageTextWidget.generated.h"

class UTextBlock;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyDamageTextWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
protected:
	// WBP에서 만든 애니메이션 이름과 변수명이 같아야 함!
	UPROPERTY(Transient, meta = (BindWidgetAnim))
	TObjectPtr<UWidgetAnimation> Anim_PopUp;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Damage;

public:
	// 데미지 설정 및 애니메이션 재생
	UFUNCTION(BlueprintCallable)
	void SetDamage(float Amount);

	// 애니메이션 끝나면 호출될 델리게이트
	UFUNCTION()
	void OnAnimFinished();
};
