// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimNotifyState/AnimNotifyState_AttackHitCheck.h"

#include "Engine/World.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/KismetMathLibrary.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemInterface.h"
#include "Advanced/MyAttributeSet.h"
#include "GameplayEffect.h"


UAnimNotifyState_AttackHitCheck::UAnimNotifyState_AttackHitCheck()
{
}

void UAnimNotifyState_AttackHitCheck::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                  float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);

	// 타격 리스트 초기화
	HitActors.Empty();
}

void UAnimNotifyState_AttackHitCheck::NotifyTick(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                 float FrameDeltaTime, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyTick(MeshComp, Animation, FrameDeltaTime, EventReference);

	if (!MeshComp || !MeshComp->GetOwner()) return;
	AActor* OwnerActor = MeshComp->GetOwner();
	UWorld* World = MeshComp->GetWorld();

	// Trace 설정 준비
	TArray<FHitResult> OutHits;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(OwnerActor);

	FVector TraceStart = OwnerActor->GetActorLocation();
	FVector TraceEnd = TraceStart; // 기본값
	float TraceRadius = AttackRadius;

	// 모드별 Trace 범위 설정
	if (HitShape == EAttackHitShape::Sphere)
	{
		// 전방으로 길게 뻗는 형태
		TraceEnd = TraceStart + (OwnerActor->GetActorForwardVector() * AttackRange);
	}
	else // Radial, Cone
	{
		// 제자리에서 주변을 검사 (반경 = AttackRadius)
		TraceEnd = TraceStart;
		// 주의: SphereTrace의 Start와 End가 같으면 Sweep이 안 될 수 있으니 아주 살짝 이동
		TraceEnd.Z += 1.0f;
	}

	// 물리적 충돌 검사 (일단 다 찾음)
	bool bResult = UKismetSystemLibrary::SphereTraceMulti(
		World, TraceStart, TraceEnd, TraceRadius,
		UEngineTypes::ConvertToTraceType(ECC_GameTraceChannel1),
		false, ActorsToIgnore,
		bDrawDebug ? EDrawDebugTrace::ForDuration : EDrawDebugTrace::None,
		OutHits, true
	);

	if (bResult)
	{
		for (const FHitResult& Hit : OutHits)
		{
			AActor* HitActor = Hit.GetActor();
			if (!HitActor || HitActors.Contains(HitActor->GetUniqueID())) continue;

			// --- [추가 검증: 부채꼴(Cone) 각도 계산] ---
			if (HitShape == EAttackHitShape::Cone)
			{
				FVector ToTarget = (HitActor->GetActorLocation() - TraceStart).GetSafeNormal();
				FVector OwnerForward = OwnerActor->GetActorForwardVector();

				// 내적(Dot)으로 각도 계산
				float DotProduct = FVector::DotProduct(ToTarget, OwnerForward);
				float AngleRad = FMath::Acos(DotProduct);
				float AngleDeg = FMath::RadiansToDegrees(AngleRad);

				// 설정한 각도(ConeAngle)의 절반보다 크면 범위 밖
				if (AngleDeg > (ConeAngle * 0.5f))
				{
					continue; // 범위 밖이므로 무시
				}
			}
			// ------------------------------------------

			HitActors.Add(HitActor->GetUniqueID());

			// 데미지 적용 (기존 로직 동일)
			IAbilitySystemInterface* OwnerASI = Cast<IAbilitySystemInterface>(OwnerActor);
			IAbilitySystemInterface* TargetASI = Cast<IAbilitySystemInterface>(HitActor);

			if (OwnerASI && TargetASI)
			{
				UAbilitySystemComponent* SourceASC = OwnerASI->GetAbilitySystemComponent();
				UAbilitySystemComponent* TargetASC = TargetASI->GetAbilitySystemComponent();

				if (SourceASC && TargetASC && DamageEffectClass)
				{
					FGameplayEffectContextHandle Context = SourceASC->MakeEffectContext();
					Context.AddHitResult(Hit);
					FGameplayEffectSpecHandle SpecHandle = SourceASC->
						MakeOutgoingSpec(DamageEffectClass, 1.0f, Context);

					if (SpecHandle.IsValid())
					{
						float BaseAttackPower = SourceASC->GetNumericAttribute(
							UMyAttributeSet::GetAttackPowerAttribute());
						if (BaseAttackPower <= 0.0f) BaseAttackPower = 10.0f;

						float FinalDamage = BaseAttackPower * DamageMultiplier;
						SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageEventTag, FinalDamage);
						
						if (HitGameplayCueTag.IsValid())
						{
							// 이펙트 명세서(Spec)에 "이 태그에 해당하는 큐를 재생해라"라고 추가
							SpecHandle.Data.Get()->AddDynamicAssetTag(HitGameplayCueTag);
						}

						SourceASC->ApplyGameplayEffectSpecToTarget(*SpecHandle.Data.Get(), TargetASC);
						
						if (HitGameplayCueTag.IsValid())
						{
							FGameplayCueParameters CueParams;
							CueParams.Location = Hit.Location;
							CueParams.Normal = Hit.Normal;
							CueParams.Instigator = OwnerActor;
							CueParams.EffectCauser = OwnerActor;
							CueParams.SourceObject = OwnerActor;
							// 필요하면 HitResult 전체 복사
							CueParams.EffectContext = Context;
						
							// 즉시 실행 (Instant)
							SourceASC->ExecuteGameplayCue(HitGameplayCueTag, CueParams);
						}
						
						// 부가 효과(디버프) 적용 
						for (const TSubclassOf<UGameplayEffect>& BuffEffectClass : AdditionalGameplayEffects)
						{
							if (BuffEffectClass)
							{
								// 부가 효과용 Spec 생성
								FGameplayEffectContextHandle BuffContext = SourceASC->MakeEffectContext();
								BuffContext.AddHitResult(Hit); // 피격 위치 정보 포함

								FGameplayEffectSpecHandle BuffSpecHandle = SourceASC->MakeOutgoingSpec(BuffEffectClass, 1.0f, BuffContext);

								if (BuffSpecHandle.IsValid())
								{
									// 부가 효과 적용!
									SourceASC->ApplyGameplayEffectSpecToTarget(*BuffSpecHandle.Data.Get(), TargetASC);
								}
							}
						}
					}
				}
			}
		}
	}
}

void UAnimNotifyState_AttackHitCheck::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                                const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	HitActors.Empty();
}
