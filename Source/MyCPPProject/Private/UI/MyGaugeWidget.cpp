// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyGaugeWidget.h"
#include "Components/Image.h"

void UMyGaugeWidget::UpdateGauge(float CurrentValue, float MaxValue)
{
	if (GaugeMaterial && MaxValue > 0.0f)
	{
		const float Percent = FMath::Clamp(CurrentValue / MaxValue, 0.0f, 1.0f);
		GaugeMaterial->SetScalarParameterValue(PercentParameterName, Percent);
	}
}

void UMyGaugeWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (GaugeImage)
	{
		// 이미지에 할당된 메테리얼을 동적 인스턴스로 변환
		GaugeMaterial = GaugeImage->GetDynamicMaterial();
	}
}
