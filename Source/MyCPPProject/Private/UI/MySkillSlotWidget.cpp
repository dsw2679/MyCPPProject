// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MySkillSlotWidget.h"

void UMySkillSlotWidget::UpdateCooldown(bool bIsCooldown, float RemainingTime, float TotalDuration)
{
	CurrentRemainingTime = RemainingTime;
	TotalCooldownTime = TotalDuration;
	
	BP_UpdateCooldown(bIsCooldown, RemainingTime);
}
