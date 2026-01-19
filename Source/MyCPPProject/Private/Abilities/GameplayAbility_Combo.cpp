// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/GameplayAbility_Combo.h"
#include "Abilities/Tasks/AbilityTask_PlayMontageAndWait.h"
#include "Abilities/Tasks/AbilityTask_WaitGameplayEvent.h"

UGameplayAbility_Combo::UGameplayAbility_Combo(const FObjectInitializer& ObjectInitializer)
{
	// 이 어빌리티는 한 번 켜지면 계속 유지되며 상태(인덱스)를 관리해야 함
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;

	CurrentComboIndex = 0;
	bNextComboInputPressed = false;
	
}

void UGameplayAbility_Combo::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
	
	CurrentComboIndex = 0;
	bNextComboInputPressed = false;
	bCheckWindowOpened = false;
	PlayComboMontage();
}

void UGameplayAbility_Combo::EndAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	bool bReplicateEndAbility, bool bWasCancelled)
{
	Super::EndAbility(Handle, ActorInfo, ActivationInfo, bReplicateEndAbility, bWasCancelled);
	
	// 종료 시 정리
	CurrentComboIndex = 0;
	bNextComboInputPressed = false;
	CurrentMontageTask = nullptr;
}

void UGameplayAbility_Combo::InputPressed(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo)
{
	Super::InputPressed(Handle, ActorInfo, ActivationInfo);
	
	// 마지막 콤보(MaxIndex)라면 입력을 받지 않음!
	if (CurrentComboIndex >= ComboSteps.Num())
	{
		return;
	}
	
	// 다음 콤보 예약
	bNextComboInputPressed = true;
	if (bCheckWindowOpened)
	{
		bNextComboInputPressed = false;

		if (CurrentMontageTask)
		{
			CurrentMontageTask->EndTask();
		}
		PlayComboMontage();
	}
}

void UGameplayAbility_Combo::OnGameplayEvent(const FGameplayEventData Payload)
{
	bCheckWindowOpened = true;
	// 입력이 미리 들어와 있었나?
	if (bNextComboInputPressed)
	{
		// 입력 사용 처리
		bNextComboInputPressed = false;

		// 현재 재생 중인 몽타주 태스크 정리 (안 해도 되지만 깔끔하게)
		if (CurrentMontageTask)
		{
			CurrentMontageTask->EndTask();
		}

		// 다음 몽타주 즉시 재생!
		PlayComboMontage();
	}
}

void UGameplayAbility_Combo::OnMontageEnded()
{
	// 입력이 있어서 이미 다음 몽타주로 넘어갔다면(Interrupted), 여기서 할 일은 없음.
	// 하지만 입력이 없어서 끝까지 재생된 거라면(BlendOut), 콤보 종료.

	// 여기서 좀 까다로운데, BlendOut은 '완료'로 칩니다.
	// 만약 다음 몽타주를 재생하면 이전 몽타주는 'Interrupted' 되거나 'BlendOut' 됩니다.
	// 따라서 "내가 의도해서 끊은 건지" 구분해야 합니다.

	// 간단하게: 더 이상 재생할 몽타주가 없거나, 입력이 없어서 끝난 경우 종료.
	if (!bNextComboInputPressed)
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
	}
}

void UGameplayAbility_Combo::PlayComboMontage()
{
	if (!ComboSteps.IsValidIndex(CurrentComboIndex))
	{
		EndAbility(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, true, false);
		return;
	}

	const FMyComboStep& Step = ComboSteps[CurrentComboIndex];
	UAnimMontage* MontageToPlay = Step.Montage;
	bCheckWindowOpened = false;

	// 1. 몽타주 재생 태스크 생성
	CurrentMontageTask = UAbilityTask_PlayMontageAndWait::CreatePlayMontageAndWaitProxy(
		this,
		FName("ComboTask"),
		MontageToPlay,
		1.0f, NAME_None, false, 1.0f, 0.0f // Rate, Section, StopWhenAbilityEnds...
	);

	// 2. 몽타주 종료(BlendOut) 처리 -> 입력이 없어서 끝까지 재생된 경우
	CurrentMontageTask->OnBlendOut.AddDynamic(this, &ThisClass::OnMontageEnded);
	CurrentMontageTask->OnInterrupted.AddDynamic(this, &ThisClass::OnMontageEnded);
	CurrentMontageTask->OnCancelled.AddDynamic(this, &ThisClass::OnMontageEnded);
	CurrentMontageTask->ReadyForActivation();

	// 3. 이벤트 대기 태스크 생성 (병렬 실행)
	// "Event.Montage.ComboCheck" 태그가 오면 OnGameplayEvent 함수 호출
	UAbilityTask_WaitGameplayEvent* WaitTask = UAbilityTask_WaitGameplayEvent::WaitGameplayEvent(
		this,
		FGameplayTag::RequestGameplayTag(FName("Event.Montage.ComboCheck")),
		nullptr, // OptionalExternalTarget
		false,   // OnlyTriggerOnce (계속 받아야 하므로 false? 아니, 몽타주마다 새로 걸 거니까 true여도 됨)
		false    // OnlyMatchExact
	);

	WaitTask->EventReceived.AddDynamic(this, &ThisClass::OnGameplayEvent);
	WaitTask->ReadyForActivation();

	// 인덱스 증가 (다음 공격 준비)
	CurrentComboIndex++;

	// 입력 플래그 초기화 (이번 몽타주 재생 중의 입력을 받기 위해)
	bNextComboInputPressed = false;
}

UAnimMontage* UGameplayAbility_Combo::GetNextComboStep(float& OutDamageMultiplier)
{
	if (ComboSteps.IsValidIndex(CurrentComboIndex))
	{
		const FMyComboStep& Step = ComboSteps[CurrentComboIndex];
		OutDamageMultiplier = Step.DamageMultiplier;

		// 다음을 위해 인덱스 증가
		CurrentComboIndex++;

		return Step.Montage;
	}

	// 콤보 끝
	return nullptr;
}

