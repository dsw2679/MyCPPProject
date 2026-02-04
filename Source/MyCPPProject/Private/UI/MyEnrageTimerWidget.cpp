// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyEnrageTimerWidget.h"
#include "Components/TextBlock.h"

void UMyEnrageTimerWidget::StartTimer(float Duration)
{
	RemainingTime = Duration;
	bIsRunning = true;

	UpdateTimeText();
}

void UMyEnrageTimerWidget::StopTimer()
{
	bIsRunning = false;
}

void UMyEnrageTimerWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (!bIsRunning) return;

	RemainingTime -= InDeltaTime;
	
	// 텍스트 갱신
	UpdateTimeText();

	// 1분(60초) 미만이면 경고 이벤트 발생 (BP에서 색깔 빨갛게!)
	if (RemainingTime <= 60.0f)
	{
		BP_OnTimeWarning(true);
	}

	// 시간 종료
	if (RemainingTime <= 0.0f)
	{
		RemainingTime = 0.0f;
		StopTimer();
	}
}

void UMyEnrageTimerWidget::UpdateTimeText()
{
	if (TXT_Time)
	{
		// 분, 초 계산
		int32 Minutes = FMath::FloorToInt(RemainingTime / 60.0f);
		int32 Seconds = FMath::FloorToInt(RemainingTime) % 60;

		// "05:00" 포맷 (Printf 사용)
		FString TimeString = FString::Printf(TEXT("%02d:%02d"), Minutes, Seconds);
		TXT_Time->SetText(FText::FromString(TimeString));
	}
}
