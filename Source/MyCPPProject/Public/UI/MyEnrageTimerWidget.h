// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "MyEnrageTimerWidget.generated.h"

class UTextBlock;

/**
 * 광폭화 타이머 위젯
 */

UCLASS()
class MYCPPPROJECT_API UMyEnrageTimerWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
public:
	// 타이머 시작 (초 단위)
	UFUNCTION(BlueprintCallable, Category = "Boss Timer")
	void StartTimer(float Duration);

	// 타이머 멈춤
	UFUNCTION(BlueprintCallable, Category = "Boss Timer")
	void StopTimer();

protected:
	virtual void NativeTick(const FGeometry& MyGeometry, float InDeltaTime) override;

	// 에디터의 TextBlock 연결
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UTextBlock> TXT_Time;

	// 시간 경고 (1분 미만일 때 등) 처리를 위한 BP 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Boss Timer")
	void BP_OnTimeWarning(bool bIsWarning);

private:
	bool bIsRunning = false;
	float RemainingTime = 0.0f;

	// 분:초 텍스트 갱신 헬퍼
	void UpdateTimeText();
};
