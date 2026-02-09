// Fill out your copyright notice in the Description page of Project Settings.


#include "MyHUDLayout.h"
#include "CommonInputSubsystem.h"
#include "Framework/Application/SlateApplication.h"


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
