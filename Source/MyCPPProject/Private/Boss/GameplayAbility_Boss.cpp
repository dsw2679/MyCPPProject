// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/GameplayAbility_Boss.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"

void UGameplayAbility_Boss::ShowTelegraphCircle(FGameplayTag CueTag, FVector CenterLocation, float Radius,
	float Duration)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = CenterLocation;
	CueParams.RawMagnitude = Radius;
	CueParams.SourceObject = GetAvatarActorFromActorInfo();
	// Duration은 GC 내부에서 처리하거나, EffectCauser 등을 통해 전달 가능
	// 여기서는 일단 인스턴스형 GC라면 파라미터로 넘겨줄 수 있습니다.

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		// C++에서 GC 실행 (Params 사용)
		ASC->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UGameplayAbility_Boss::ApplyDamageToArea(FVector CenterLocation, float Radius, float DamageAmount,
	TSubclassOf<UGameplayEffect> DamageEffectClass)
{
	if (!DamageEffectClass) return;

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	// 범위 내 액터 찾기 (Sphere Overlap)
	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		CenterLocation,
		Radius,
		HitObjectTypes, // 미리 설정해둔 Pawn 타입 등
		APawn::StaticClass(), // Pawn만 필터링
		ActorsToIgnore,
		OutActors
	);

	// 디버그용 그리기 (빨간 원)
	DrawDebugSphere(GetWorld(), CenterLocation, Radius, 12, FColor::Red, false, 2.0f);

	if (bHit)
	{
		// Spec 생성 (한 번만 만들어서 재사용)
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (SpecHandle.IsValid())
		{
			// 데미지 수치 설정 (Data.Damage 태그 사용)
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")), DamageAmount);

			for (AActor* HitActor : OutActors)
			{
				if (HitActor && HitActor != GetAvatarActorFromActorInfo())
				{
					// 2. 각 타겟에게 적용
					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(HitActor);
					if (TargetASC)
					{
						// TargetASC에 적용 (ApplyGameplayEffectSpecToSelf 사용)
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

FVector UGameplayAbility_Boss::GetPlayerLocation(float ForwardDistance)
{
	// 싱글 플레이어 기준 (0번 플레이어)
	ACharacter* PlayerChar = UGameplayStatics::GetPlayerCharacter(GetWorld(), 0);

	if (PlayerChar)
	{
		FVector Loc = PlayerChar->GetActorLocation();

		// 필요하다면 플레이어의 '앞쪽' 위치를 계산
		if (ForwardDistance != 0.0f)
		{
			Loc += PlayerChar->GetActorForwardVector() * ForwardDistance;
		}

		// 바닥 보정 (발밑 좌표를 정확히 잡기 위해 Z축 조정이 필요할 수 있음)
		// Loc.Z -= PlayerChar->GetCapsuleComponent()->GetScaledCapsuleHalfHeight();

		return Loc;
	}

	// 플레이어를 못 찾았으면 그냥 보스 위치 반환 (안전장치)
	return GetAvatarActorFromActorInfo()->GetActorLocation();
}
