// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/AbilityTask/AbilityTask_InterpMoveTo.h"

UAbilityTask_InterpMoveTo::UAbilityTask_InterpMoveTo(const FObjectInitializer& ObjectInitializer)
{
	bTickingTask = true;
	TimeElapsed = 0.0f;
}

UAbilityTask_InterpMoveTo* UAbilityTask_InterpMoveTo::InterpMoveTo(UGameplayAbility* OwningAbility, AActor* TargetActor,
	FVector TargetLocation, float Duration, UCurveFloat* OptionalCurve)
{
	UAbilityTask_InterpMoveTo* MyTask = NewAbilityTask<UAbilityTask_InterpMoveTo>(OwningAbility);
	MyTask->TargetActor = TargetActor;
	MyTask->TargetLocation = TargetLocation;
	MyTask->Duration = FMath::Max(Duration, 0.001f); // 0 나누기 방지
	MyTask->InterpolationCurve = OptionalCurve;

	return MyTask;
}

void UAbilityTask_InterpMoveTo::Activate()
{
	Super::Activate();
	
	if (TargetActor)
	{
		StartLocation = TargetActor->GetActorLocation();
		// 만약 이미 도착해있다면 바로 종료
		if (StartLocation.Equals(TargetLocation, 1.0f))
		{
			if (ShouldBroadcastAbilityTaskDelegates())
			{
				OnFinished.Broadcast();
			}
			EndTask();
		}
	}
	else
	{
		EndTask();
	}
}

void UAbilityTask_InterpMoveTo::TickTask(float DeltaTime)
{
	if (IsFinished()) return;

	if (!TargetActor)
	{
		EndTask();
		return;
	}

	TimeElapsed += DeltaTime;
	float Alpha = FMath::Clamp(TimeElapsed / Duration, 0.0f, 1.0f);

	// 커브 적용
	float InterpAlpha = Alpha;
	if (InterpolationCurve)
	{
		InterpAlpha = InterpolationCurve->GetFloatValue(Alpha);
	}

	// 위치 보간 및 이동
	FVector NewLoc = FMath::Lerp(StartLocation, TargetLocation, InterpAlpha);
	TargetActor->SetActorLocation(NewLoc, true); // Sweep=true (충돌 처리)

	// 종료 체크
	if (Alpha >= 1.0f)
	{
		if (ShouldBroadcastAbilityTaskDelegates())
		{
			OnFinished.Broadcast();
		}
		EndTask();
	}
}
