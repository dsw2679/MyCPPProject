// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyUserWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UMyUserWidget::SetAbilitySystemComponent(UAbilitySystemComponent* InASC)
{
	if (AbilitySystemComponent.Get() == InASC)
	{
		return;
	}

	// 기존 ASC에 연결된 바인딩이 있다면 해제
	if (UAbilitySystemComponent* OldASC = AbilitySystemComponent.Get())
	{
		for (auto& Pair : AttributeChangeDelegateHandles)
		{
			OldASC->GetGameplayAttributeValueChangeDelegate(Pair.Key).Remove(Pair.Value);
		}
	}
	AttributeChangeDelegateHandles.Empty();

	AbilitySystemComponent = InASC;

	// 새로 설정된 ASC가 있다면 추가 초기화 로직을 여기에 작성
}

void UMyUserWidget::BindAttribute(const FGameplayAttribute& Attribute)
{
	UAbilitySystemComponent* ASC = AbilitySystemComponent.Get();
	if (!ASC)
	{
		return;
	}

	if (AttributeChangeDelegateHandles.Contains(Attribute))
	{
		return; // 이미 바인딩 된 속성입니다.
	}

	// 값 변경 시 호출될 델리게이트 등록 (WeakLambda를 사용하여 위젯이 파괴되면 안전하게 무시됨)
	FDelegateHandle Handle = ASC->GetGameplayAttributeValueChangeDelegate(Attribute).AddWeakLambda(this,
		[this, Attribute](const FOnAttributeChangeData& Data)
		{
			OnAttributeChanged(Attribute, Data.NewValue);
		});

	AttributeChangeDelegateHandles.Add(Attribute, Handle);

	// 바인딩 시점의 현재 값으로 초기 업데이트 실행
	if (ASC->HasAttributeSetForAttribute(Attribute))
	{
		float CurrentValue = ASC->GetNumericAttribute(Attribute);
		OnAttributeChanged(Attribute, CurrentValue);
	}
}

void UMyUserWidget::OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue)
{
	BP_OnAttributeChanged(Attribute, NewValue);
}

void UMyUserWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 내 주인이 플레이어 컨트롤러라면
	if (APlayerController* PC = GetOwningPlayer())
	{
		// 그 컨트롤러가 조종하는 폰(Pawn)이 있다면
		if (APawn* Pawn = PC->GetPawn())
		{
			// 그 폰의 ASC를 찾아서 나한테 연결해라!
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Pawn))
			{
				SetAbilitySystemComponent(ASC);
			}
		}
	}
}
