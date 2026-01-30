// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Abilities/Tasks/AbilityTask.h"
#include "AbilityTask_InterpMoveTo.generated.h"

DECLARE_DYNAMIC_MULTICAST_DELEGATE(FInterpMoveToDelegate);

class UCurveFloat;

/**
 * 액터를 특정 위치로 부드럽게 이동시키는 태스크
 */

UCLASS()
class MYCPPPROJECT_API UAbilityTask_InterpMoveTo : public UAbilityTask
{
	GENERATED_BODY()
	
public:
	UAbilityTask_InterpMoveTo(const FObjectInitializer& ObjectInitializer);

	// 태스크 생성 함수 (블루프린트에서 호출)
	UFUNCTION(BlueprintCallable, Category = "Ability|Tasks", meta = (HidePin = "OwningAbility", DefaultToSelf = "OwningAbility", BlueprintInternalUseOnly = "TRUE"))
	static UAbilityTask_InterpMoveTo* InterpMoveTo(
		UGameplayAbility* OwningAbility,
		AActor* TargetActor,
		FVector TargetLocation,
		float Duration,
		UCurveFloat* OptionalCurve
	);

	// 태스크가 활성화될 때 (Tick 켜기)
	virtual void Activate() override;
	// 매 프레임 실행
	virtual void TickTask(float DeltaTime) override;

public:
	// 완료 시 실행될 델리게이트
	UPROPERTY(BlueprintAssignable)
	FInterpMoveToDelegate OnFinished;

protected:
	UPROPERTY()
	AActor* TargetActor;

	FVector StartLocation;
	FVector TargetLocation;
	float Duration;
	float TimeElapsed;

	UPROPERTY()
	UCurveFloat* InterpolationCurve;
};
