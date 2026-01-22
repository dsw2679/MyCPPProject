// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"

UMyAttributeSet::UMyAttributeSet()
{
	InitDamageScale(1.0f);
}

void UMyAttributeSet::PreAttributeChange(const FGameplayAttribute& Attribute, float& NewValue)
{
	Super::PreAttributeChange(Attribute, NewValue);

	// MaxHealth가 변경될 때 현재 Health가 Max를 넘지 않도록 조정
	if (Attribute == GetMaxHealthAttribute())
	{
		AdjustAttributeForMaxChange(Health, MaxHealth, NewValue, GetHealthAttribute());
	}

	// MaxMP 변경 시 MP 비율 조정
	else if (Attribute == GetMaxMPAttribute())
	{
		AdjustAttributeForMaxChange(MP, MaxMP, NewValue, GetMPAttribute());
	}
}

void UMyAttributeSet::PostGameplayEffectExecute(const FGameplayEffectModCallbackData& Data)
{
	Super::PostGameplayEffectExecute(Data);

	if (Data.EvaluatedData.Attribute == GetHealthAttribute())
	{
		// Health를 0에서 MaxHealth 사이로 제한
		SetHealth(FMath::Clamp(GetHealth(), 0.0f, GetMaxHealth()));
	}
	// MP를 0에서 MaxMP 사이로 제한
	else if (Data.EvaluatedData.Attribute == GetMPAttribute())
	{
		float OldValue = GetMP();
		float ClampedValue = FMath::Clamp(GetMP(), 0.0f, GetMaxMP());

		SetMP(ClampedValue);
	}
	// 데미지가 들어왔을 때
	if (Data.EvaluatedData.Attribute == GetDamageAttribute()) // 혹은 Health가 깎였을 때
	{
		const float LocalDamageDone = GetDamage();
		SetDamage(0.0f); // 데미지 메타 속성 초기화

		if (LocalDamageDone > 0.0f)
		{
			// 현재 피증 수치 가져오기 (기본값 1.0이라고 가정)
			float DamageMultiplier = GetDamageScale();
			
			if (DamageMultiplier <= 0.0f) DamageMultiplier = 1.0f;
			
			float FinalDamage = LocalDamageDone * DamageMultiplier;
			
			// 체력 차감
			const float OldHealth = GetHealth();
			const float NewHealth = FMath::Clamp(OldHealth - FinalDamage, 0.0f, GetMaxHealth());
			SetHealth(NewHealth);
			
			// 데미지 로그
			AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
			if (TargetActor)
			{
				// 로그 출력: [누가] [얼마의 데미지를 입음] [남은 체력]
				UE_LOG(LogTemp, Warning, TEXT("[MyAttributeSet] %s took %.1f damage. Current Health: %.1f / %.1f"),
					*TargetActor->GetName(),
					FinalDamage,
					NewHealth,
					GetMaxHealth());
			}
			
			// 공격자(Source)와 피해자(Target) 파악
			AActor* SourceActor = Data.EffectSpec.GetContext().GetInstigator();
			UAbilitySystemComponent* TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();

			if (TargetActor && TargetASC)
			{
				// 피격 리액션 결정 (태그 기반)
				// 들어온 GE(공격)에 붙어있는 태그를 검사합니다.
				FGameplayTagContainer SourceTags;
				Data.EffectSpec.GetAllAssetTags(SourceTags); // GE에 설정된 태그 가져오기

				FGameplayTag EventTag;

				// 우선순위: 다운 > 넉백 > 헤비 > 라이트
				if (SourceTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Stun"))))
				{
					EventTag = FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Stun"));
				}
				else if (SourceTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Knockdown"))))
				{
					EventTag = FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Knockdown"));
				}
				else if (SourceTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Knockback"))))
				{
					EventTag = FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Knockback"));
				}
				else if (SourceTags.HasTag(FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Light"))))
				{
					// 기본은 약한 경직
					EventTag = FGameplayTag::RequestGameplayTag(FName("Effect.Hit.Light"));
				}
				
				if (EventTag.IsValid())
				{
					// 리액션 이벤트 전송 -> GA_HitReaction 발동 유도
					FGameplayEventData Payload;
					Payload.EventTag = EventTag;
					Payload.Instigator = SourceActor;
					Payload.Target = TargetActor;
					Payload.EventMagnitude = FinalDamage; // 데미지량 전달
					// 공격자가 있다면 밀려날 방향 계산 (공격자 -> 나)
					if (SourceActor)
					{
						FVector KnockbackDir = (TargetActor->GetActorLocation() - SourceActor->GetActorLocation()).GetSafeNormal();
						KnockbackDir.Z = 0.0f; // 수평으로만 밀리게
						Payload.ContextHandle = Data.EffectSpec.GetContext();
					}

					UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, EventTag, Payload);
				}
			}
		}
	}
}

// Max값 변경 시 현재값 비율 조정 헬퍼 함수
void UMyAttributeSet::AdjustAttributeForMaxChange(FGameplayAttributeData& AffectedAttribute,
                                                  const FGameplayAttributeData& MaxAttribute, float NewMaxValue, const
                                                  FGameplayAttribute& AffectedAttributeProperty)
{
	UAbilitySystemComponent* AbilityComp = GetOwningAbilitySystemComponent();
	const float CurrentMaxValue = MaxAttribute.GetCurrentValue();
	if (!FMath::IsNearlyEqual(CurrentMaxValue, NewMaxValue) && AbilityComp)
	{
		const float CurrentValue = AffectedAttribute.GetCurrentValue();
		float NewDelta = (CurrentMaxValue > 0.f)
			                 ? (CurrentValue * NewMaxValue / CurrentMaxValue) - CurrentValue
			                 : NewMaxValue;

		AbilityComp->ApplyModToAttributeUnsafe(AffectedAttributeProperty, EGameplayModOp::Additive, NewDelta);
	}
}
