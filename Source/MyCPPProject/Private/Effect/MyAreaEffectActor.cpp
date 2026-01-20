// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/MyAreaEffectActor.h"

#include "AbilitySystemInterface.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemComponent.h"
#include "NiagaraComponent.h"

// Sets default values
AMyAreaEffectActor::AMyAreaEffectActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;
	
	// 1. 충돌체 생성
	OverlapSphere = CreateDefaultSubobject<USphereComponent>(TEXT("OverlapSphere"));
	RootComponent = OverlapSphere;
	OverlapSphere->SetSphereRadius(200.0f);
	OverlapSphere->SetCollisionProfileName(TEXT("OverlapAllDynamic")); // 겹침 감지용

	// 2. FX 컴포넌트 (옵션)
	NiagaraFX = CreateDefaultSubobject<UNiagaraComponent>(TEXT("NiagaraFX"));
	NiagaraFX->SetupAttachment(RootComponent);
}

// Called when the game starts or when spawned
void AMyAreaEffectActor::BeginPlay()
{
	Super::BeginPlay();
	
	if (HasAuthority()) // 서버에서만 데미지 판정
	{
		// 1. 주기적인 타이머 실행
		FTimerHandle TimerHandle;
		GetWorld()->GetTimerManager().SetTimer(TimerHandle, this, &ThisClass::OnTimerTick, TickInterval, true);

		// 2. 수명 설정 (자동 파괴)
		if (MaxTickCount > 0.0f)
		{
			SetLifeSpan(MaxTickCount);
		}
	}
}

void AMyAreaEffectActor::OnTimerTick()
{
	if (!EffectClass) return;

	// 겹친 액터들 가져오기
	TArray<AActor*> OverlappingActors;
	OverlapSphere->GetOverlappingActors(OverlappingActors);
	
	// 시전자(Instigator) 정보 가져오기 (이 액터를 소환한 주인)
	AActor* InstigatorActor = GetInstigator();
	// 만약 SetOwner로 설정했다면 GetOwner() 사용 가능.
	// 보통 GA에서 SpawnActor 할 때 Owner를 설정해줍니다.

	for (AActor* TargetActor : OverlappingActors)
	{
		// 자기 자신이나 아군은 제외하는 로직이 필요하다면 여기에 추가 (TeamID 비교 등)
		if (TargetActor == InstigatorActor) continue; // 시전자는 데미지 안 받음

		// GAS 컴포넌트 찾기
		IAbilitySystemInterface* ASI = Cast<IAbilitySystemInterface>(TargetActor);
		if (ASI)
		{
			UAbilitySystemComponent* TargetASC = ASI->GetAbilitySystemComponent();
			if (TargetASC)
			{
				// 이펙트 적용 (Context 생성 포함)
				FGameplayEffectContextHandle ContextHandle = TargetASC->MakeEffectContext();
				ContextHandle.AddSourceObject(this); // 이 장판이 소스
				ContextHandle.AddInstigator(InstigatorActor, InstigatorActor); // 시전자가 가해자

				// Spec 생성
				FGameplayEffectSpecHandle SpecHandle = TargetASC->MakeOutgoingSpec(EffectClass, 1.0f, ContextHandle);

				if (SpecHandle.IsValid())
				{
					SpecHandle.Data->SetSetByCallerMagnitude(FGameplayTag::RequestGameplayTag(FName("SetByCaller.Damage")), DamagePerTick);
					
					TargetASC->ApplyGameplayEffectSpecToSelf(*SpecHandle.Data.Get());
				}
			}
		}
	}
	// 시각 효과 재실행
	if (NiagaraFX)
	{
		// 이미 켜져 있다면 강제로 재시작 (Reset)
		NiagaraFX->Activate(true);
	}

	// // 만약 3번만 터지고 끝내야 한다면
	// CurrentTickCount++;
	// if (CurrentTickCount >= MaxTickCount)
	// {
	// 	// 수명 종료
	// 	Destroy();
	// }
}
