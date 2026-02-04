// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MySkillSlotWidget.h"


void UMySkillSlotWidget::NativeTick(const FGeometry& MyGeometry, float InDeltaTime)
{
	Super::NativeTick(MyGeometry, InDeltaTime);
	
	if (bIsActiveCooldown)
	{
		const float CurrentTime = GetWorld()->GetTimeSeconds();
		const float TimeLeft = CooldownEndTime - CurrentTime;

		if (TimeLeft <= 0.0f)
		{
			// 쿨타임 끝!
			bIsActiveCooldown = false;
			CurrentRemainingTime = 0.0f;
			
			// 마무리 갱신
			BP_UpdateCooldown(false, 0.0f);
			BP_SetCooldownPercent(0.0f);
		}
		else
		{
			// 진행 중 (0.0 ~ 1.0 비율 계산)
			CurrentRemainingTime = TimeLeft;
			float Percent = FMath::Clamp(TimeLeft / TotalCooldownTime, 0.0f, 1.0f);

			// BP로 보내서 텍스트와 메테리얼만 업데이트
			BP_UpdateCooldown(true, TimeLeft); // 텍스트용 (기존 이벤트)
			BP_SetCooldownPercent(Percent);    // 메테리얼용 (새 이벤트)
		}
	}
}

void UMySkillSlotWidget::UpdateCooldown(bool bIsCooldown, float RemainingTime, float TotalDuration)
{
	CurrentRemainingTime = RemainingTime;
	TotalCooldownTime = TotalDuration;
	bIsActiveCooldown = bIsCooldown;

	if (bIsActiveCooldown && TotalCooldownTime > 0.0f)
	{
		// 종료 예정 시간 계산
		if (const UWorld* World = GetWorld())
		{
			CooldownEndTime = World->GetTimeSeconds() + RemainingTime;

			// 쿨타임이 시작되었으니 Tick을 켭니다. (이때부터만 연산 시작)
			bIsActiveCooldown = true;
		}
	}
	else
	{
		UE_LOG(LogTemp, Warning, TEXT("[UI] Skill Slot [%s] Force Stopped by Event."), *SkillTag.ToString());
		
		// 쿨타임이 아니면 즉시 종료 처리
		bIsActiveCooldown = false;
		BP_UpdateCooldown(false, 0.0f);
		BP_SetCooldownPercent(0.0f); // 0%로 초기화
	}
}
