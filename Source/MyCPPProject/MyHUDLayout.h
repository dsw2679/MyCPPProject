// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MyHUDLayout.generated.h"

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "My HUD Layout"))
class MYCPPPROJECT_API UMyHUDLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()
	
protected:
	// 활성화될 때 입력 설정을 CommonUI에 전달
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	virtual void NativeConstruct() override;
	
	// 활성화될 때 실행될 로직
	virtual void NativeOnActivated() override;
	
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	
};
