// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/GameplayAbility.h"
#include "LevelSequencePlayer.h"
#include "MyGameplayAbility.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyGameplayAbility : public UGameplayAbility
{
	GENERATED_BODY()
	
public:
	UMyGameplayAbility();

	// 스킬을 식별하기 위한 태그 (예: Ability.Skill.Q)
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Ability")
	FGameplayTag StartupInputTag;

	// '시전 중'인지 체크하기 위한 유틸리티 함수 (나중에 UI 등에 활용)
	UFUNCTION(BlueprintCallable, Category = "Ability")
	bool IsAbilityActive() const;
	
	// SetByCaller를 사용하여 이펙트를 적용하는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	FGameplayEffectSpecHandle MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude);

	// 위에서 만든 Spec을 실제로 자신에게 적용하는 함수
	UFUNCTION(BlueprintCallable, Category = "Ability|Effect")
	void ApplyEffectWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag, float Magnitude);
	
	// [설정] 쿨타임 시간 (초 단위)+
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	float CooldownDuration = 0.0f;

	// [설정] 쿨타임용 태그 (SetByCaller로 시간을 넘겨줄 때 쓸 태그)
	// 기본값: SetByCaller.Cooldown
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cooldown")
	FGameplayTag CooldownTag;

	// [설정] 비용 목록 (태그 : 소모량)
	// 예: SetByCaller.Cost.MP : 20.0
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Cost")
	TMap<FGameplayTag, float> CostMap;
	
	// 쿨타임 태그를 GE에서 가져오지 않고, 우리가 설정한 CooldownTag 변수에서 직접 가져오도록 변경
	virtual const FGameplayTagContainer* GetCooldownTags() const override;
	
	/**
	* [Helper] 마우스 커서가 가리키는 바닥 위치를 가져옵니다.
	* @param OutLocation : 결과 위치가 담길 변수
	* @return : 성공적으로 가져왔으면 true 반환
	*/
	UFUNCTION(BlueprintCallable, Category = "Lyra|Input")
	bool GetHeroCursorHit(FVector& OutLocation);
	
	/**
	* [Helper] 캐릭터를 마우스 커서 방향으로 회전시킵니다. (Yaw축만 회전)
	*/
	UFUNCTION(BlueprintCallable, Category = "Lyra|Input")
	void RotateToCursor();
	
	/**
	* 캐릭터를 강제로 이동시킵니다. (Launch Character 래핑)
	*/
	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	void LaunchAvatarCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride);

	/**
	 * 캐릭터의 중력을 조절합니다.
	 */
	UFUNCTION(BlueprintCallable, Category = "Lyra|Ability")
	void SetGravityScale(float NewGravityScale);
	
	// --- 오버라이드 함수 ---

	// 쿨타임 GE에 수치를 주입하여 반환
	virtual void ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
		const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// 비용 GE에 수치를 주입하여 반환
	virtual void ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		const FGameplayAbilityActivationInfo ActivationInfo) const override;
	
	// 비용 검사 (SetByCaller 값을 주입하여 검사)
	virtual bool CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo, 
		OUT FGameplayTagContainer* OptionalRelevantTags = nullptr) const override;
	
	// activateability하면 이동을 멈추기위해 함수 오버라이드
	virtual void ActivateAbility(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
		const FGameplayAbilityActivationInfo ActivationInfo, const FGameplayEventData* TriggerEventData) override;
	
private:
	// 태그 컨테이너를 반환하기 위한 임시 저장소
	UPROPERTY(Transient)
	FGameplayTagContainer TempCooldownTags;
	
protected:
	// 컨트롤러를 미리 캐스팅해서 반환하는 헬퍼 함수
	UFUNCTION(BlueprintPure, Category = "Lyra|Ability")
	AMyCPPProjectPlayerController* GetMyPlayerController() const;
	
	// 보스 사망 메시지를 전송하는 함수
	UFUNCTION(BlueprintCallable, Category = "Boss")
	void BroadcastBossDeathMessage();
	
	UFUNCTION(BlueprintCallable, Category = "Ability|Cinematic")
	ULevelSequencePlayer* PlaySequenceAtAvatarLocation(ULevelSequence* InSequence);
};
