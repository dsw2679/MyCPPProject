// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimNotifies/AnimNotifyState.h"
#include "GameplayTagContainer.h"
#include "AnimNotifyState_AttackHitCheck.generated.h"

class UGameplayEffect;

/**
 * 
 */
UENUM(BlueprintType)
enum class EAttackHitShape : uint8
{
	// 전방 구체 (기존)
	Sphere      UMETA(DisplayName = "Sphere (Forward)"),
	// 전방 부채꼴
	Cone        UMETA(DisplayName = "Cone (Sector)"),
	// 주변 360도
	Radial      UMETA(DisplayName = "Radial (Around)")
};

UCLASS()
class MYCPPPROJECT_API UAnimNotifyState_AttackHitCheck : public UAnimNotifyState
{
	GENERATED_BODY()
	
public:
	UAnimNotifyState_AttackHitCheck();
	
	virtual void NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float TotalDuration, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, float FrameDeltaTime, const FAnimNotifyEventReference& EventReference) override;
	virtual void NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation, const FAnimNotifyEventReference& EventReference) override;
	
protected:
	// 판정 형태 선택
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitCheck")
	EAttackHitShape HitShape = EAttackHitShape::Sphere;
	
	// 공격 반경 (Sphere, Radial, Cone 모두 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitCheck")
	float AttackRadius = 50.0f;
	
	// 공격 사거리 (Sphere 모드에서 전방 거리 / Cone, Radial에서는 무시되거나 반경과 동일하게 취급)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitCheck", meta = (EditCondition = "HitShape == EAttackHitShape::Sphere"))
	float AttackRange = 150.0f;

	// 부채꼴 각도 (Cone 모드 전용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "HitCheck", meta = (EditCondition = "HitShape == EAttackHitShape::Cone"))
	float ConeAngle = 90.0f;
	
	// 디버그 드로잉 여부
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Debug")
	bool bDrawDebug = false;

	// 적용할 GameplayEffect 클래스 (데미지용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TSubclassOf<UGameplayEffect> DamageEffectClass;
	
	// 공격 배율 (1.0 = 100%, 1.5 = 150%)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float DamageMultiplier = 1.0f;
	
	// 데미지 수치를 구분할 태그 (보통 Data.Damage 같은 태그 사용)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag DamageEventTag;
	
	// 무력화 수치 (예: 20.0이면 20만큼 깎음)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Damage")
	float StaggerMultiplier = 0.0f;

	// 무력화 수치를 전달할 태그 (SetByCaller.Stagger)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag StaggerEventTag;
	
	// 타격 성공 시 재생할 GameplayCue 태그
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	FGameplayTag HitGameplayCueTag;

	// 추가로 적용할 GE 목록 (디버프, 상태이상 등)
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "GAS")
	TArray<TSubclassOf<UGameplayEffect>> AdditionalGameplayEffects;
private:
	// 이번 공격에서 이미 맞은 액터들 목록 (중복 타격 방지)
	TArray<uint32> HitActors;
};
