// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "MyGaugeWidget.generated.h"

class UImage;

/**
 * 메테리얼 파라미터를 제어하여 게이지를 표현
 */
UCLASS()
class MYCPPPROJECT_API UMyGaugeWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
public:
	// 외부(HUD)에서 값을 갱신해줄 때 호출
	UFUNCTION(BlueprintCallable, Category = "UI")
	void UpdateGauge(float CurrentValue, float MaxValue);

protected:
	virtual void NativeConstruct() override;

	// 에디터에서 바인딩할 메테리얼 이미지
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UImage> GaugeImage;

	// 메테리얼 파라미터 이름 (기본값: "Percent")
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "UI")
	FName PercentParameterName = FName("Percent");

private:
	// 다이나믹 메테리얼 인스턴스 캐싱
	UPROPERTY()
	TObjectPtr<UMaterialInstanceDynamic> GaugeMaterial;
};
