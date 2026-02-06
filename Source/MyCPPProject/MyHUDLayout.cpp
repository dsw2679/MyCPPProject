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
	
	// // PlayerController를 통해 입력 설정을 직접 제어합니다.
	// if (APlayerController* PC = GetOwningPlayer())
	// {
	// 	// 1. 커서 보이기 (강제)
	// 	PC->bShowMouseCursor = true;
	//
	// 	// 2. 입력 모드 설정: GameAndUI + 커서 숨김 방지 + 가두기 방지
	// 	// FInputModeGameAndUI가 쿼터뷰 조작의 핵심입니다.
	// 	FInputModeGameAndUI InputMode;
	// 	InputMode.SetHideCursorDuringCapture(false); // [중요] 클릭해도 커서 유지
	// 	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock); // 가두지 않음
	// 	PC->SetInputMode(InputMode);
	//
	// 	UE_LOG(LogTemp, Warning, TEXT("[UI] HUD Activated: Forced Quarter-View Input Mode"));
	// }

}

UWidget* UMyHUDLayout::NativeGetDesiredFocusTarget() const
{
	return nullptr;
	
}
