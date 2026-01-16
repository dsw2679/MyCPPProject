// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"

UMyGameplayAbility::UMyGameplayAbility()
{
	// 기본적으로 스킬은 인스턴스화되어 캐릭터마다 독립적으로 실행되도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UMyGameplayAbility::IsAbilityActive() const
{
	// 현재 능력이 활성화(시전 중) 상태인지 확인
	return IsActive();
}

FGameplayEffectSpecHandle UMyGameplayAbility::MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass,
	FGameplayTag DataTag, float Magnitude)
{
	// 1. ASC 가져오기
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !EffectClass) return FGameplayEffectSpecHandle();

	// 2. Outgoing Spec 생성
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), Context);

	// 3. SetByCaller 데이터 주입
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
	}

	return SpecHandle;
}

void UMyGameplayAbility::ApplyEffectWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag,
	float Magnitude)
{
	FGameplayEffectSpecHandle SpecHandle = MakeEffectSpecWithSetByCaller(EffectClass, DataTag, Magnitude);

	if (SpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
	}
}

void UMyGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// 쿨타임 시간 주입 (기본 태그: SetByCaller.Cooldown)
			FGameplayTag TagToUse = CooldownTag.IsValid() ? CooldownTag : FGameplayTag::RequestGameplayTag(FName("SetByCaller.Cooldown"));
			SpecHandle.Data->SetSetByCallerMagnitude(TagToUse, CooldownDuration);

			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void UMyGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// Map에 있는 모든 비용 태그와 값을 주입
			for (const TPair<FGameplayTag, float>& Pair : CostMap)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
			}

			FActiveGameplayEffectHandle ActiveGE = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			UE_LOG(LogTemp, Warning, TEXT("ApplyCost Result: %s"), ActiveGE.WasSuccessfullyApplied() ? TEXT("SUCCESS") : TEXT("FAILED"));
		}
	}
}

bool UMyGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	// 1. 기본 비용 GE 가져오기
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (!CostGE)
	{
		return true; // 비용 없으면 통과
	}

	// 2. ActorInfo 및 ASC 유효성 검사
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	// 3. Spec 생성 (임시)
	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	// CDO를 사용해 Spec 생성
	FGameplayEffectSpec Spec(CostGE, Context, GetAbilityLevel());

	// 4. SetByCaller 값 주입 (핵심!)
	for (const TPair<FGameplayTag, float>& Pair : CostMap)
	{
		Spec.SetSetByCallerMagnitude(Pair.Key, Pair.Value);
	}

	// 5. 직접 속성 검사 (Super::CheckCost를 부르면 에러가 나므로 직접 구현)
	for (int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
	{
		const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
		const FGameplayAttribute& Attribute = ModDef.Attribute;

		// 이 Modifier가 SetByCaller를 사용하는지 확인
		if (ModDef.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			// 우리가 주입한 값 가져오기 (CostMap에 있는 값)
			float CostValue = 0.0f;
			FGameplayTag Tag = ModDef.ModifierMagnitude.GetSetByCallerFloat().DataTag;
			if (CostMap.Contains(Tag))
			{
				CostValue = CostMap[Tag];
			}

			// 현재 캐릭터의 Attribute 값 가져오기
			if (ActorInfo->AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
			{
				float CurrentValue = ActorInfo->AbilitySystemComponent->GetNumericAttribute(Attribute);
				
				float AbsCost = FMath::Abs(CostValue);
				
				if (CurrentValue < AbsCost)
				{
					// 실패 태그 추가 (디버깅용)
					if (OptionalRelevantTags)
					{
						const FGameplayTag FailTag = FGameplayTag::RequestGameplayTag(
							FName("Ability.ActivateFail.Cost"));
						OptionalRelevantTags->AddTag(FailTag);
					}
					return false;
				}
			}
		}
	}

	return true;
}

