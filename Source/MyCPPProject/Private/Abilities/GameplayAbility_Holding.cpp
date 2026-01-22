// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Holding.h"

#include "Abilities/Tasks/AbilityTask_WaitInputRelease.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Kismet/KismetSystemLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"
#include "Engine/World.h"
#include "GameFramework/Pawn.h"

UGameplayAbility_Holding::UGameplayAbility_Holding()
{
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

void UGameplayAbility_Holding::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
                                               const FGameplayAbilityActorInfo* ActorInfo,
                                               const FGameplayAbilityActivationInfo ActivationInfo,
                                               const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);

	// 몽타주 재생 (루프 모션)
	if (LoopMontage)
	{
		UAbilityTask_PlayMontageAndWait* MontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
			this, NAME_None, LoopMontage, 1.0f, StartSection); // 여기서 섹션 지정
		MontageTask->ReadyForActivation();
	}

	// 주기적 데미지 타이머 시작
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().SetTimer(
			DamageTimerHandle,
			this,
			&ThisClass::OnTickDamage,
			DamageInterval,
			true // 반복
		);

		// 첫 틱 바로 실행 
		OnTickDamage();

		// 데미지 타이머 & 3초 종료 타이머 (이전과 동일)
		GetWorld()->GetTimerManager().SetTimer(DamageTimerHandle, this, &ThisClass::OnTickDamage, DamageInterval, true);

		GetWorld()->GetTimerManager().SetTimer(DurationTimerHandle, FTimerDelegate::CreateLambda([this]()
		{
			OnInputReleased(0.0f); // 3초 지나면 키 뗀 것과 동일하게 처리
		}), MaxHoldDuration, false);

		// 입력 해제 대기 (Wait Input Release)
		InputReleaseTask = UAbilityTask_WaitInputRelease::WaitInputRelease(this, true);
		if (InputReleaseTask)
		{
			InputReleaseTask->OnRelease.AddDynamic(this, &ThisClass::OnInputReleased);
			InputReleaseTask->ReadyForActivation();
		}
	}
}

void UGameplayAbility_Holding::EndAbility(const FGameplayAbilitySpecHandle Handle,
                                          const FGameplayAbilityActorInfo* ActorInfo,
                                          const FGameplayAbilityActivationInfo ActivationInfo,
                                          bool bReplicateEndAbility, bool bWasCancelled)
{
	// 타이머 정리
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	// 몽타주 정지
	if (ActorInfo->AbilitySystemComponent.IsValid() && LoopMontage)
	{
		ActorInfo->AbilitySystemComponent->CurrentMontageStop();
	}
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
}

void UGameplayAbility_Holding::OnTickDamage()
{
	if (!GetAvatarActorFromActorInfo()) return;

	// 주변 적 찾기 (Sphere Overlap)
	TArray<AActor*> OverlappedActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	UKismetSystemLibrary::SphereOverlapActors(
		GetWorld(),
		GetAvatarActorFromActorInfo()->GetActorLocation(),
		DamageRadius,
		TArray<TEnumAsByte<EObjectTypeQuery>>(), // Object Types (필요 시 설정)
		APawn::StaticClass(), // Pawn 클래스만 필터링
		ActorsToIgnore,
		OverlappedActors
	);

	// 데미지 주기
	UAbilitySystemComponent* SourceASC = GetAbilitySystemComponentFromActorInfo();
	if (!SourceASC || !DamageEffectClass) return;

	// [중요] 공격력 스탯 가져오기
	float AttackPower = SourceASC->GetNumericAttribute(UMyAttributeSet::GetAttackPowerAttribute());
	if (AttackPower <= 0.0f) AttackPower = 10.0f; // 안전장치
	float FinalDamage = AttackPower * DamageMultiplier;
	
	for (AActor* HitActor : OverlappedActors)
	{
		// GAS 컴포넌트 찾기
		UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(HitActor);
		if (TargetASC)
		{
			// Spec 생성
			FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
			FGameplayEffectSpecHandle SpecHandle = SourceASC->MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

			if (SpecHandle.IsValid())
			{
				// SetByCaller로 데미지 수치 전달
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageEventTag, FinalDamage);
				
				if (HitGameplayCueTag.IsValid())
				{
					SpecHandle.Data.Get()->AddDynamicAssetTag(HitGameplayCueTag);
				}

				SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
				
				if (HitGameplayCueTag.IsValid())
				{
					FGameplayCueParameters CueParams;

					// 광역 공격이므로, 맞은 적의 발밑이나 가슴 높이 위치를 지정합니다.
					// 여기서는 맞은 적의 현재 위치를 사용합니다.
					CueParams.Location = HitActor->GetActorLocation();
					CueParams.Instigator = GetAvatarActorFromActorInfo();
					CueParams.EffectCauser = GetAvatarActorFromActorInfo();

					// 이펙트 실행
					SourceASC->ExecuteGameplayCue(HitGameplayCueTag, CueParams);
				}
			}
		}
	}
}

void UGameplayAbility_Holding::OnInputReleased(float TimeHeld)
{
	if (GetWorld())
	{
		GetWorld()->GetTimerManager().ClearTimer(DamageTimerHandle);
		GetWorld()->GetTimerManager().ClearTimer(DurationTimerHandle);
	}

	// 1. "End" 섹션으로 점프 (블루프린트의 MontageJumpToSection과 동일)
	if (LoopMontage && CurrentActorInfo->AbilitySystemComponent.IsValid())
	{
		CurrentActorInfo->AbilitySystemComponent->CurrentMontageJumpToSection(EndSection);
	}

	K2_EndAbility();
}
