// Fill out your copyright notice in the Description page of Project Settings.


#include "Boss/GameplayAbility_Boss.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Kismet/KismetSystemLibrary.h"
#include "Kismet/GameplayStatics.h"
#include "GameFramework/Character.h"
#include "Kismet/KismetMathLibrary.h"
#include "NavigationSystem.h"

void UGameplayAbility_Boss::ShowTelegraphCircle(FGameplayTag CueTag, FVector CenterLocation, float Radius,
                                                float Duration)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = CenterLocation;
	CueParams.RawMagnitude = Radius;
	CueParams.NormalizedMagnitude = Duration;
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
	//DrawDebugSphere(GetWorld(), CenterLocation, Radius, 12, FColor::Red, false, 2.0f);

	if (bHit)
	{
		// Spec 생성 (한 번만 만들어서 재사용)
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (SpecHandle.IsValid())
		{
			// 데미지 수치 설정 (Data.Damage 태그 사용)
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
			                                         DamageAmount);

			for (AActor* HitActor : OutActors)
			{
				if (HitActor && HitActor != GetAvatarActorFromActorInfo())
				{
					// 2. 각 타겟에게 적용
					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
						HitActor);
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

TArray<FVector> UGameplayAbility_Boss::GetClapLocations(float ForwardDistance, float SideDistance)
{
	TArray<FVector> Locations;
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return Locations;

	FVector Center = Avatar->GetActorLocation();
	FVector Forward = Avatar->GetActorForwardVector();
	FVector Right = Avatar->GetActorRightVector();

	// 바닥 Z축 보정 (필요시 활성화, 보통은 Center Z를 그대로 씀)
	// float FloorZ = Center.Z - Avatar->GetSimpleCollisionHalfHeight();
	// FVector BaseLoc = FVector(Center.X, Center.Y, FloorZ);

	// 순서대로 배열에 추가

	// [0] 앞 (Front)
	Locations.Add(Center + (Forward * ForwardDistance));

	// [1] 중앙 (Center - 보스 위치)
	Locations.Add(Center);

	// [2] 뒤 (Back)
	Locations.Add(Center - (Forward * ForwardDistance));

	// [3] 왼쪽 (Left)
	Locations.Add(Center - (Right * SideDistance));

	// [4] 오른쪽 (Right)
	Locations.Add(Center + (Right * SideDistance));

	return Locations;
}

void UGameplayAbility_Boss::ShowTelegraphRect(FGameplayTag CueTag, FVector CenterLocation, FRotator Rotation,
                                              FVector Extent, float Duration)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = CenterLocation;
	CueParams.Normal = Rotation.Vector(); // 회전 정보를 Normal에 담아 전달
	CueParams.RawMagnitude = Extent.X; // 크기 정보 (X축 기준 스케일로 활용 가능)
	CueParams.NormalizedMagnitude = Duration;
	CueParams.SourceObject = GetAvatarActorFromActorInfo();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UGameplayAbility_Boss::ApplyDamageToRect(FVector CenterLocation, FRotator Rotation, FVector Extent,
                                              float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass)
{
	if (!DamageEffectClass) return;

	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	// Box Overlap 사용
	bool bHit = UKismetSystemLibrary::BoxOverlapActors(
		this,
		CenterLocation,
		Extent,
		HitObjectTypes,
		APawn::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	// 디버그용 박스 그리기 (보라색)
	//DrawDebugBox(GetWorld(), CenterLocation, Extent, Rotation.Quaternion(), FColor::Red, false, 2.0f);

	if (bHit)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
			                                         DamageAmount);

			for (AActor* HitActor : OutActors)
			{
				if (HitActor && HitActor != GetAvatarActorFromActorInfo())
				{
					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
						HitActor);
					if (TargetASC)
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
					}
				}
			}
		}
	}
}

void UGameplayAbility_Boss::ShowTelegraphRect_Explicit(FGameplayTag CueTag, FVector CenterLocation, FRotator Rotation,
                                                       FVector Extent, float Duration)
{
	FGameplayCueParameters CueParams;
	CueParams.Location = CenterLocation; // 위치.
	CueParams.Normal = Extent;
	CueParams.RawMagnitude = Rotation.Yaw;
	CueParams.NormalizedMagnitude = Duration; // 지속 시간
	CueParams.SourceObject = GetAvatarActorFromActorInfo();

	if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
	{
		ASC->ExecuteGameplayCue(CueTag, CueParams);
	}
}

void UGameplayAbility_Boss::FacePlayer(float RotationSpeed)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// 플레이어 위치 가져오기 (기존 함수 활용)
	FVector PlayerLoc = GetPlayerLocation();
	FVector MyLoc = Avatar->GetActorLocation();

	// 높이(Z) 무시하고 방향 계산 (평면 회전)
	PlayerLoc.Z = MyLoc.Z;

	// 바라봐야 할 회전값 계산 (FindLookAtRotation)
	FRotator TargetRotation = UKismetMathLibrary::FindLookAtRotation(MyLoc, PlayerLoc);

	// 회전 적용
	if (RotationSpeed <= 0.0f)
	{
		// 즉시 회전
		Avatar->SetActorRotation(TargetRotation);
	}
	else
	{
		// (간단 구현: 일단 즉시 회전으로 통일)
		Avatar->SetActorRotation(TargetRotation);
	}
}

void UGameplayAbility_Boss::ApplyDamageToCone(FVector CenterLocation, FRotator Rotation, float Radius, float HalfAngle,
                                              float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass)
{
	if (!DamageEffectClass) return;

	// 일단 원형(Sphere)으로 주변 적들을 다 찾습니다.
	TArray<AActor*> OutActors;
	TArray<AActor*> ActorsToIgnore;
	ActorsToIgnore.Add(GetAvatarActorFromActorInfo());

	bool bHit = UKismetSystemLibrary::SphereOverlapActors(
		this,
		CenterLocation,
		Radius,
		HitObjectTypes, // 기존에 정의된 HitObjectTypes 사용
		APawn::StaticClass(),
		ActorsToIgnore,
		OutActors
	);

	// 디버그: 부채꼴 모양 그리기 (대략적인 표현)
	// 중심선
	FVector Forward = Rotation.Vector();
	//DrawDebugLine(GetWorld(), CenterLocation, CenterLocation + Forward * Radius, FColor::Red, false, 2.0f);

	// 부채꼴 양 끝선 (간단 디버그용)
	FVector LeftDir = Forward.RotateAngleAxis(-HalfAngle, FVector::UpVector);
	FVector RightDir = Forward.RotateAngleAxis(HalfAngle, FVector::UpVector);
	//DrawDebugLine(GetWorld(), CenterLocation, CenterLocation + LeftDir * Radius, FColor::Yellow, false, 2.0f);
	//DrawDebugLine(GetWorld(), CenterLocation, CenterLocation + RightDir * Radius, FColor::Yellow, false, 2.0f);


	if (bHit)
	{
		// Spec 생성
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(DamageEffectClass);
		if (SpecHandle.IsValid())
		{
			SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("Data.Damage")),
			                                         DamageAmount);

			// 찾은 적들 중, "각도" 안에 있는 녀석만 골라냅니다.
			for (AActor* HitActor : OutActors)
			{
				if (!HitActor || HitActor == GetAvatarActorFromActorInfo()) continue;

				// 타겟 방향 벡터 계산
				FVector DirectionToTarget = (HitActor->GetActorLocation() - CenterLocation).GetSafeNormal();

				// 내적(Dot Product)을 이용한 각도 계산
				// Forward와 TargetDir의 내적값 = cos(각도)
				float DotResult = FVector::DotProduct(Forward, DirectionToTarget);

				// 내적값을 각도로 변환 (Acos -> Radian -> Degree)
				float AngleToTarget = FMath::RadiansToDegrees(FMath::Acos(DotResult));

				// 각도가 HalfAngle보다 작거나 같으면 범위 안!
				if (AngleToTarget <= HalfAngle)
				{
					UAbilitySystemComponent* TargetASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(
						HitActor);
					if (TargetASC)
					{
						TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());

						// 디버그: 맞은 놈 표시
						//DrawDebugSphere(GetWorld(), HitActor->GetActorLocation(), 30.0f, 12, FColor::Green, false, 1.0f);
					}
				}
			}
		}
	}
}

float UGameplayAbility_Boss::StartZoneLoop(int32 Count, float Interval, float SpawnRadius, float ExplosionDelay,
	float DamageRadius, float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass, FGameplayTag CueTag)
{
	if (Count <= 0) return 0.0f;

	UWorld* World = GetWorld();
	if (!World) return 0.0f;

	// 이번 회차 장판 생성 (SpawnRandomZoneExplosion 로직 인라인 또는 호출)
	SpawnRandomZoneExplosion(SpawnRadius, ExplosionDelay, DamageRadius, DamageAmount, DamageEffectClass, CueTag);

	// 남은 횟수가 있다면 다음 타이머 예약
	int32 RemainingCount = Count - 1;
	if (RemainingCount > 0)
	{
		FTimerHandle RecursiveHandle; // Fire & Forget
		FTimerDelegate TimerDel;

		// 중요: WeakLambda를 써서 어빌리티가 소멸되면 안전하게 중단되도록 함 (또는 람다 캡처로 값 전달)
		// 여기서는 필요한 값들을 모두 캡처(복사)하여 다음 호출로 넘깁니다.
		TimerDel.BindWeakLambda(this, [=, this]()
		{
			StartZoneLoop(RemainingCount, Interval, SpawnRadius, ExplosionDelay, DamageRadius, DamageAmount, DamageEffectClass, CueTag);
		});

		World->GetTimerManager().SetTimer(RecursiveHandle, TimerDel, Interval, false);
	}
	// 총 지속 시간 계산: (총 횟수 - 1) * 생성 간격 + 마지막 장판의 폭발 대기 시간
	float TotalDuration = (static_cast<float>(Count) - 1.0f) * Interval + ExplosionDelay;

	return TotalDuration;
}

void UGameplayAbility_Boss::SpawnRandomZoneExplosion(float SpawnRadius, float ExplosionDelay, float DamageRadius,
                                                     float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass, FGameplayTag CueTag)
{
	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// 랜덤 위치 계산 (Navigation System 활용)
	FVector Origin = Avatar->GetActorLocation();
	FVector TargetLocation = Origin;

	if (UWorld* World = GetWorld())
	{
		if (UNavigationSystemV1* NavSystem = FNavigationSystem::GetCurrent<UNavigationSystemV1>(World))
		{
			FNavLocation RandomNavLoc;
			// 네비게이션 메시 위에서 유효한 위치를 찾습니다.
			if (NavSystem->GetRandomPointInNavigableRadius(Origin, SpawnRadius, RandomNavLoc))
			{
				TargetLocation = RandomNavLoc.Location;
			}
		}

		// 전조(Telegraph) 즉시 실행 (GameplayCue)
		ShowTelegraphCircle(CueTag, TargetLocation, DamageRadius, ExplosionDelay);

		// 지연 후 폭발 (TimerDelegate 사용)
		// 람다를 사용하지 않고 멤버 함수를 바인딩하여 안전하게 처리
		FTimerDelegate TimerDel;
		TimerDel.BindUObject(this, &UGameplayAbility_Boss::Internal_ExplodeZone, TargetLocation, DamageRadius, DamageAmount, DamageEffectClass);

		// 타이머 핸들은 저장하지 않음 (Fire & Forget). 어빌리티가 종료되어도 장판은 터져야 자연스럽기 때문.
		// 만약 어빌리티 취소 시 장판도 사라져야 한다면 핸들 관리가 필요하지만, 장판 패턴은 보통 깔아두면 끝까지 가는 게 일반적입니다.
		FTimerHandle TempHandle;
		World->GetTimerManager().SetTimer(TempHandle, TimerDel, ExplosionDelay, false);
	}

}

void UGameplayAbility_Boss::Internal_ExplodeZone(FVector TargetLocation, float Radius, float Amount,
	TSubclassOf<UGameplayEffect> EffectClass)
{
	// 기존에 구현된 광역 데미지 함수 재사용
	ApplyDamageToArea(TargetLocation, Radius, Amount, EffectClass);
}

