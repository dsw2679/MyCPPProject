#include "UI/MyShopManagementWidget.h"
#include "Kismet/GameplayStatics.h"

void UMyShopManagementWidget::NativeConstruct()
{
	Super::NativeConstruct();
}

void UMyShopManagementWidget::NativeOnActivated()
{
	Super::NativeOnActivated();

	// 창이 열리면 마우스 커서 보이기
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(true);
		
		FInputModeGameAndUI InputMode;
		InputMode.SetWidgetToFocus(TakeWidget());
		InputMode.SetLockMouseToViewportBehavior(EMouseLockMode::DoNotLock);
		PC->SetInputMode(InputMode);
	}
}

void UMyShopManagementWidget::NativeOnDeactivated()
{
	Super::NativeOnDeactivated();

	// 창이 닫히면 마우스 커서 숨기고 게임 입력 모드로 복귀
	if (APlayerController* PC = GetOwningPlayer())
	{
		PC->SetShowMouseCursor(false);
		
		FInputModeGameOnly InputMode;
		PC->SetInputMode(InputMode);
	}
}
