#include "UI/MyShopManagementWidget.h"

void UMyShopManagementWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyShopManagementWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// // 창이 열리면 마우스 커서 보이기	
	// if (APlayerController* PC = GetOwningPlayer())
	// {
	// 	PC->SetShowMouseCursor(true);
	// 	
	// 	FInputModeGameAndUI InputMode;
	// 	InputMode.SetWidgetToFocus(TakeWidget());
	// 	InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
	// 	PC->SetInputMode(InputMode);
	// }
}

void UMyShopManagementWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();
}

TOptional<FUIInputConfig> UMyShopManagementWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}

