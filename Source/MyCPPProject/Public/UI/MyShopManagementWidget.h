#pragma once

#include "CoreMinimal.h"
#include "CommonActivatableWidget.h"
#include "MyShopManagementWidget.generated.h"

class UMyShopCollection;

/**
 * 상점과 인벤토리를 통합 관리하는 활성 위젯 (ESC 닫기 지원)
 */
UCLASS()
class MYCPPPROJECT_API UMyShopManagementWidget : public UCommonActivatableWidget
{
	GENERATED_BODY()

protected:
	virtual void NativeConstruct() override;
	virtual void NativeOnActivated() override;
	virtual void NativeOnDeactivated() override;

	// 상점 데이터 에셋 
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Management")
	TObjectPtr<UMyShopCollection> ShopDataCollection;

	// 하위 위젯: 상점 패널 (BindWidget) 
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonUserWidget> WBP_Shop;

	// 하위 위젯: 인벤토리 패널 (BindWidget)
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget))
	TObjectPtr<UCommonUserWidget> WBP_Inventory;
};
