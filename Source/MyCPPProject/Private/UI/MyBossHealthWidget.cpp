// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyBossHealthWidget.h"
#include "AbilitySystemComponent.h"
#include "MyGameplayTags.h"
#include "Advanced/MyAttributeSet.h"
#include "NativeGameplayTags.h"


void UMyBossHealthWidget::SetAbilitySystemComponent(UAbilitySystemComponent* InASC)
{
	if (!InASC) return;

	AbilitySystemComponent = InASC;

	// 1. 초기값 설정
	float CurrentHealth = InASC->GetNumericAttribute(UMyAttributeSet::GetHealthAttribute());
	float MaxHealth = InASC->GetNumericAttribute(UMyAttributeSet::GetMaxHealthAttribute());
	UpdateHealth(CurrentHealth, MaxHealth);

	float CurrentStagger = InASC->GetNumericAttribute(UMyAttributeSet::GetStaggerAttribute());
	float MaxStagger = InASC->GetNumericAttribute(UMyAttributeSet::GetMaxStaggerAttribute());
	UpdateStagger(CurrentStagger, MaxStagger);

	// 2. 어트리뷰트 변경 델리게이트 바인딩
	InASC->GetGameplayAttributeValueChangeDelegate(UMyAttributeSet::GetHealthAttribute()).AddUObject(this, &UMyBossHealthWidget::OnHealthChanged);
	InASC->GetGameplayAttributeValueChangeDelegate(UMyAttributeSet::GetMaxHealthAttribute()).AddUObject(this, &UMyBossHealthWidget::OnMaxHealthChanged);
	InASC->GetGameplayAttributeValueChangeDelegate(UMyAttributeSet::GetStaggerAttribute()).AddUObject(this, &UMyBossHealthWidget::OnStaggerChanged);
	InASC->GetGameplayAttributeValueChangeDelegate(UMyAttributeSet::GetMaxStaggerAttribute()).AddUObject(this, &UMyBossHealthWidget::OnMaxStaggerChanged);
}

void UMyBossHealthWidget::OnHealthChanged(const FOnAttributeChangeData& Data)
{
	float MaxHealth = AbilitySystemComponent->GetNumericAttribute(UMyAttributeSet::GetMaxHealthAttribute());
	UpdateHealth(Data.NewValue, MaxHealth);
}

void UMyBossHealthWidget::OnMaxHealthChanged(const FOnAttributeChangeData& Data)
{
	float CurrentHealth = AbilitySystemComponent->GetNumericAttribute(UMyAttributeSet::GetHealthAttribute());
	UpdateHealth(CurrentHealth, Data.NewValue);
}

void UMyBossHealthWidget::OnStaggerChanged(const FOnAttributeChangeData& Data)
{
	float MaxStagger = AbilitySystemComponent->GetNumericAttribute(UMyAttributeSet::GetMaxStaggerAttribute());
	UpdateStagger(Data.NewValue, MaxStagger);
}

void UMyBossHealthWidget::OnMaxStaggerChanged(const FOnAttributeChangeData& Data)
{
	float CurrentStagger = AbilitySystemComponent->GetNumericAttribute(UMyAttributeSet::GetStaggerAttribute());
	UpdateStagger(CurrentStagger, Data.NewValue);
}

void UMyBossHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	BossSpawnListenerHandle = MessageSubsystem.RegisterListener(FMyGameplayTags::Get().Message_Boss_Spawned, this, &UMyBossHealthWidget::OnBossSpawned);
	
	// 처음에 안 보이게 숨김 (보스 나올 때까지)
	//SetVisibility(ESlateVisibility::Collapsed);
}

void UMyBossHealthWidget::NativeDestruct()
{
	// 구독 해제
	if (BossSpawnListenerHandle.IsValid())
	{
		UGameplayMessageSubsystem::Get(this).UnregisterListener(BossSpawnListenerHandle);
	}
	Super::NativeDestruct();
}

void UMyBossHealthWidget::OnBossSpawned(FGameplayTag Channel, const FMyBossMessageStruct& Payload)
{
	if (Payload.BossASC)
	{
		// 아까 만든 설정 함수 호출
		SetAbilitySystemComponent(Payload.BossASC);
		// UI 짠! 하고 보여주기
		SetVisibility(ESlateVisibility::HitTestInvisible);
	}
}
