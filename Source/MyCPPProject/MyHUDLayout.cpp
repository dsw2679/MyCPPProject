// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUDLayout.h"
#include "CommonInputSubsystem.h"
#include "Components/NamedSlot.h"
#include "Framework/Application/SlateApplication.h"


void UMyHUDLayout::AddBossInfoWidget(UUserWidget* BossWidget)
{
	if (BossInfoSlot && BossWidget) BossInfoSlot->SetContent(BossWidget);
}

void UMyHUDLayout::SetPressINotificationVisible(bool bVisible, TSubclassOf<UUserWidget> NotificationClass)
{
	if (!NotificationSlot) return;

	if (bVisible && NotificationClass)
	{
		UUserWidget* NotifyWidget = CreateWidget<UUserWidget>(this, NotificationClass);
		NotificationSlot->SetContent(NotifyWidget);
	}
	else
	{
		NotificationSlot->SetContent(nullptr);
	}
}

TOptional<FUIInputConfig> UMyHUDLayout::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}

void UMyHUDLayout::NativeConstruct()
{
	SetConsumePointerInput(false);
	
	bSupportsActivationFocus = true;
	
	
	Super::NativeConstruct();
}

void UMyHUDLayout::NativeOnActivated()
{
	Super::NativeOnActivated();
	

}

UWidget* UMyHUDLayout::NativeGetDesiredFocusTarget() const
{
	return nullptr;
	
}
