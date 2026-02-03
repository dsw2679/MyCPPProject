// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "PrimaryGameLayout.h"
#include "Widgets/CommonActivatableWidgetContainer.h"
#include "MyPrimaryGameLayout.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyPrimaryGameLayout : public UPrimaryGameLayout
{
	GENERATED_BODY()
public:
	UCommonActivatableWidget* GetGameLayerWidget() const;
	
protected:
	// 위젯 초기화 시 호출
	virtual void NativeConstruct() override;

	// 블루프린트에서 만든 스택 위젯들을 C++로 가져옴
	// 이름이 반드시 BP와 일치해야 함 (BindWidget)
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Stack_Game;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Stack_Menu;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UCommonActivatableWidgetStack> Stack_Modal;
	
};
