// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MyHUDLayout.generated.h"

class UNamedSlot;

/**
 * 
 */
UCLASS(Abstract, BlueprintType, Blueprintable, Meta = (DisplayName = "My HUD Layout"))
class MYCPPPROJECT_API UMyHUDLayout : public UCommonActivatableWidget
{
	GENERATED_BODY()
public:
	UFUNCTION(BlueprintCallable)
	void AddBossInfoWidget(UUserWidget* BossWidget);
	
	UFUNCTION(BlueprintCallable, Category = "UI")
	void SetPressINotificationVisible(bool bVisible, TSubclassOf<UUserWidget> NotificationClass = nullptr);
	
protected:
	// 활성화될 때 입력 설정을 CommonUI에 전달
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

	virtual void NativeConstruct() override;
	
	// 활성화될 때 실행될 로직
	virtual void NativeOnActivated() override;
	
	virtual UWidget* NativeGetDesiredFocusTarget() const override;
	
	// 보스 인포 위젯 슬롯
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UNamedSlot> BossInfoSlot;
	
	// I키 알림 위젯이 표시될 슬롯
	UPROPERTY(meta = (BindWidgetOptional))
	TObjectPtr<UNamedSlot> NotificationSlot;
	
};
