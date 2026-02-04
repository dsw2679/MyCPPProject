// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyAttributeSet.h"
#include "GameplayEffectExtension.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "MyGameplayTags.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "Net/UnrealNetwork.h"

UMyAttributeSet::UMyAttributeSet()
{
	InitDamageScale(1.0f);
	InitIncomingStagger(0.0f);
}

void UMyAttributeSet::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);
	
	// DOREPLIFETIME_CONDITION_NOTIFY: 값이 바뀔 때만 전송하고, 클라이언트에서 OnRep를 호출함
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Health, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxHealth, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxMP, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, IdentityGage, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, Stagger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MaxStagger, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, DamageScale, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, AttackPower, COND_None, REPNOTIFY_Always);
	DOREPLIFETIME_CONDITION_NOTIFY(UMyAttributeSet, MoveSpeed, COND_None, REPNOTIFY_Always);
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
	
	if (Attribute == GetMoveSpeedAttribute())
	{
		NewValue = FMath::Max(NewValue, 0.0f); // 음수 방지
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
	
	// 이동 속도 변경 시 실제 캐릭터 무브먼트에 적용
	if (Data.EvaluatedData.Attribute == GetMoveSpeedAttribute())
	{
		ACharacter* AvatarChar = Cast<ACharacter>(Data.Target.AbilityActorInfo->AvatarActor.Get());
		if (AvatarChar && AvatarChar->GetCharacterMovement())
		{
			AvatarChar->GetCharacterMovement()->MaxWalkSpeed = GetMoveSpeed();
		}
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
			
			AActor* TargetActorForTag = Data.Target.AbilityActorInfo->AvatarActor.Get();
			if (TargetActorForTag)
			{
				// 대상의 ASC를 가져와서 'Data.Boss' 태그가 있는지 확인
				UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(TargetActorForTag);
				if (TargetASC && TargetASC->HasMatchingGameplayTag(MyGameplayTags::Data_Boss))
				{
					FMyBossHealthMessage HealthMsg;
					HealthMsg.CurrentHealth = NewHealth;
					HealthMsg.MaxHealth = GetMaxHealth();

					UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(
						MyGameplayTags::Message_Boss_HealthChanged, HealthMsg);
				}
			}
			
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
			
			// 데미지 폰트 출력 메시지 전송
			if (FinalDamage > 0.0f && TargetActor)
			{
				// 안전한 방식으로 ASC 가져오기
				UAbilitySystemComponent* TargetASC = Data.Target.AbilityActorInfo->AbilitySystemComponent.Get();

				if (TargetASC)
				{
					if (TargetASC->HasMatchingGameplayTag(MyGameplayTags::Data_Boss))
					{
						// 보스: 정상 전송
						FMyDamageMessage DmgMsg;
						DmgMsg.TargetActor = TargetActor;
						DmgMsg.DamageAmount = FinalDamage;

						UGameplayMessageSubsystem::Get(GetWorld()).BroadcastMessage(
							MyGameplayTags::Message_Damage, DmgMsg);

						UE_LOG(LogTemp, Warning, TEXT("[DamageText] Message Sent for BOSS: %s"), *TargetActor->GetName());
					}
					else
					{
						// 보스 아님 (일반 몹 or 플레이어) -> 여기서도 띄워줘야 한다면 로직 추가 필요
						UE_LOG(LogTemp, Warning, TEXT("[DamageText] Target %s is NOT Boss (No Tag). Skipping."), *TargetActor->GetName());
					}
				}
				else
				{
					UE_LOG(LogTemp, Error, TEXT("[DamageText] Failed to get ASC for %s"), *TargetActor->GetName());
				}
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
				if (SourceTags.HasTag(MyGameplayTags::Effect_Hit_Stun))
				{
					EventTag = MyGameplayTags::Effect_Hit_Stun;
				}
				else if (SourceTags.HasTag(MyGameplayTags::Effect_Hit_Knockdown))
				{
					EventTag = MyGameplayTags::Effect_Hit_Knockdown;
				}
				else if (SourceTags.HasTag(MyGameplayTags::Effect_Hit_Knockback))
				{
					EventTag = MyGameplayTags::Effect_Hit_Knockback;
				}
				else if (SourceTags.HasTag(MyGameplayTags::Effect_Hit_Light))
				{
					EventTag = MyGameplayTags::Effect_Hit_Light;
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
	// 무력화 게이지(Stagger) 변동 시 처리
	if (Data.EvaluatedData.Attribute == GetIncomingStaggerAttribute())
	{
		// GetIncomingStagger() 대신 Magnitude를 직접 사용합니다.
		const float LocalIncomingStagger = Data.EvaluatedData.Magnitude;
		SetIncomingStagger(0.0f); // 메타 속성 초기화
		
		AActor* TargetActor = Data.Target.AbilityActorInfo->AvatarActor.Get();
		
		if (TargetActor)
		{
			FGameplayTagContainer TargetTags;
			Data.Target.AbilityActorInfo->AbilitySystemComponent->GetOwnedGameplayTags(TargetTags);

			// 면역 태그(State.Boss.Immune.Stagger)가 있다면 무력화 수치 무시
			if (TargetTags.HasTag(MyGameplayTags::State_Boss_Immune_Stagger)) // 태그 이름은 실제 정의에 맞춰 수정
			{
				UE_LOG(LogTemp, Warning, TEXT("[Stagger] %s is IMMUNE to Stagger! Ignoring value: %.1f"),
					*TargetActor->GetName(), LocalIncomingStagger);
				return; // 여기서 함수 종료 -> 게이지 차감 안 함
			}
		}
		if (LocalIncomingStagger > 0.0f)
		{
			const float CurrentStagger = GetStagger();
			const float MaxStaggerValue = GetMaxStagger();

			// 차감 계산
			const float NewStagger = FMath::Clamp(CurrentStagger - LocalIncomingStagger, 0.0f, MaxStaggerValue);
			SetStagger(NewStagger);


			UE_LOG(LogTemp, Warning, TEXT("[Stagger] %s : -%.1f (Current: %.1f / Max: %.1f)"),
				TargetActor ? *TargetActor->GetName() : TEXT("None"),
				LocalIncomingStagger,
				NewStagger,
				MaxStaggerValue);

			// 무력화 발생 체크
			if (NewStagger <= 0.0f)
			{
				if (TargetActor)
				{
					FGameplayTagContainer TargetTags;
					Data.Target.AbilityActorInfo->AbilitySystemComponent->GetOwnedGameplayTags(TargetTags);

					if (!TargetTags.HasTag(MyGameplayTags::State_Boss_Staggered))
					{
						UE_LOG(LogTemp, Error, TEXT("[Stagger] !!! BOSS STAGGERED !!!"));

						FGameplayEventData Payload;
						Payload.EventTag = MyGameplayTags::Event_Boss_Staggered;
						Payload.Target = TargetActor;
						UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(TargetActor, MyGameplayTags::Event_Boss_Staggered, Payload);
					}
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

void UMyAttributeSet::OnRep_MoveSpeed(const FGameplayAttributeData& OldMoveSpeed)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MoveSpeed, OldMoveSpeed);
}

void UMyAttributeSet::OnRep_Health(const FGameplayAttributeData& OldHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Health, OldHealth);
}

void UMyAttributeSet::OnRep_MaxHealth(const FGameplayAttributeData& OldMaxHealth)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxHealth, OldMaxHealth);
}

void UMyAttributeSet::OnRep_MP(const FGameplayAttributeData& OldMP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MP, OldMP);
}

void UMyAttributeSet::OnRep_MaxMP(const FGameplayAttributeData& OldMaxMP)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxMP, OldMaxMP);
}

void UMyAttributeSet::OnRep_IdentityGage(const FGameplayAttributeData& OldIdentityGage)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, IdentityGage, OldIdentityGage);
}

void UMyAttributeSet::OnRep_Stagger(const FGameplayAttributeData& OldStagger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, Stagger, OldStagger);
}

void UMyAttributeSet::OnRep_MaxStagger(const FGameplayAttributeData& OldMaxStagger)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, MaxStagger, OldMaxStagger);
}

void UMyAttributeSet::OnRep_DamageScale(const FGameplayAttributeData& OldDamageScale)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, DamageScale, OldDamageScale);
}

void UMyAttributeSet::OnRep_AttackPower(const FGameplayAttributeData& OldAttackPower)
{
	GAMEPLAYATTRIBUTE_REPNOTIFY(UMyAttributeSet, AttackPower, OldAttackPower);
}
