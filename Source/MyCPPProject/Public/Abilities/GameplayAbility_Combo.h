// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Advanced/MyGameplayAbility.h"
#include "GameplayAbility_Combo.generated.h"

/**
 * 
 */
class UAbilityTask_PlayMontageAndWait;

USTRUCT(BlueprintType)
struct FMyComboStep
{
	GENERATED_BODY()

	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TObjectPtr<UAnimMontage> Montage = nullptr;

	// 이 단계의 공격력 배율 (1.0 = 100%, 1.5 = 150%)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	float DamageMultiplier = 1.0f;
};

/**
 * 평타 및 스킬 콤보를 위한 베이스 클래스
 */

UCLASS()
class MYCPPPROJECT_API UGameplayAbility_Combo : public UMyGameplayAbility
{
	GENERATED_BODY()
public:
	UGameplayAbility_Combo(const FObjectInitializer& ObjectInitializer = FObjectInitializer::Get());

	//~UGameplayAbility Interface
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
	virtual void EndAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, bool bReplicateEndAbility, bool bWasCancelled) override;
	//~End UGameplayAbility Interface

protected:
	// 어빌리티가 활성화된 상태에서 입력(키 누름)이 발생하면 호출됨
	virtual void InputPressed(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo) override;
	
	UFUNCTION()
	void OnGameplayEvent(const FGameplayEventData Payload);
	
	/** [추가] 몽타주 재생 함수 (BP에서 호출하던 걸 C++ 내부로 가져옴) */
	void PlayComboMontage();
	
	UFUNCTION()
	void OnMontageEnded();

protected:
	/**
	 * 콤보 리스트
	 * - 평타라면 4개, Q스킬이라면 2~3개를 에디터에서 등록
	 */
	UPROPERTY(EditDefaultsOnly, Category = "Combo")
	TArray<FMyComboStep> ComboSteps;

	/** 현재 진행 중인 콤보 인덱스 */
	int32 CurrentComboIndex;

	/** 다음 콤보 입력이 예약되었는지 여부 */
	bool bNextComboInputPressed;
	
	bool bCheckWindowOpened = false;
	
	UPROPERTY()
	TObjectPtr<UAbilityTask_PlayMontageAndWait> CurrentMontageTask;

public:
	/**
	 * [BP 호출용] 현재 인덱스의 몽타주 등을 반환하고, 인덱스를 1 증가시킴.
	 * 더 이상 콤보가 없으면 nullptr 반환.
	 */
	UFUNCTION(BlueprintCallable, Category = "Combo")
	UAnimMontage* GetNextComboStep(float& OutDamageMultiplier);

	/** [BP 호출용] 입력이 예약되어 있는지 확인 */
	UFUNCTION(BlueprintCallable, Category = "Combo")
	bool HasNextComboInput() const { return bNextComboInputPressed; }

	/** [BP 호출용] 입력 예약 상태 초기화 (새 몽타주 시작 시 호출) */
	UFUNCTION(BlueprintCallable, Category = "Combo")
	void ClearComboInput() { bNextComboInputPressed = false; }
};
