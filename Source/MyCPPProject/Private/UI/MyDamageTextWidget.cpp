// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyDamageTextWidget.h"

#include "Components/TextBlock.h"
#include "Animation/WidgetAnimation.h"

void UMyDamageTextWidget::SetDamage(float Amount)
{
	if (TXT_Damage)
	{
		// 텍스트 설정 (소수점 제거)
		TXT_Damage->SetText(FText::AsNumber((int32)Amount));
	}

	if (Anim_PopUp)
	{
		// 애니메이션 재생 
		PlayAnimation(Anim_PopUp);

		// 종료 콜백 연결 
		// FWidgetAnimationDynamicEvent 델리게이트 사용

		FWidgetAnimationDynamicEvent AnimFinishedDelegate;
		AnimFinishedDelegate.BindDynamic(this, &UMyDamageTextWidget::OnAnimFinished);
		BindToAnimationFinished(Anim_PopUp, AnimFinishedDelegate);
	}
}

void UMyDamageTextWidget::OnAnimFinished()
{
	RemoveFromParent();
}
