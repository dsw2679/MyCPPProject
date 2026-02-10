// Fill out your copyright notice in the Description page of Project Settings.


#include "Effect/MyGrenadeProjectile.h"
#include "GameFramework/ProjectileMovementComponent.h"
#include "Components/SphereComponent.h"
#include "AbilitySystemBlueprintLibrary.h"
#include "AbilitySystemComponent.h"
#include "MyGameplayTags.h"

AMyGrenadeProjectile::AMyGrenadeProjectile()
{
	PrimaryActorTick.bCanEverTick = false;
	bReplicates = true; // 서버에서 생성되어 클라이언트로 복제됨

	// 충돌 컴포넌트 설정
	CollisionComp = CreateDefaultSubobject<USphereComponent>(TEXT("SphereComp"));
	CollisionComp->InitSphereRadius(15.0f);
	CollisionComp->SetCollisionProfileName(TEXT("Projectile"));
	CollisionComp->OnComponentBeginOverlap.AddDynamic(this, &AMyGrenadeProjectile::OnOverlapBegin);
	RootComponent = CollisionComp;

	// 발사체 이동 컴포넌트
	ProjectileMovement = CreateDefaultSubobject<UProjectileMovementComponent>(TEXT("ProjectileComp"));
	ProjectileMovement->UpdatedComponent = CollisionComp;
	ProjectileMovement->InitialSpeed = 1000.f;
	ProjectileMovement->MaxSpeed = 1000.f;
	ProjectileMovement->bRotationFollowsVelocity = true;
	ProjectileMovement->ProjectileGravityScale = 1.0f;
	ProjectileMovement->bShouldBounce = false;
}

// Called when the game starts or when spawned
void AMyGrenadeProjectile::BeginPlay()
{
	Super::BeginPlay();
	SetLifeSpan(3.0f);
}

void AMyGrenadeProjectile::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor != GetInstigator() && OtherActor != this)
	{
		// 서버에서만 로직 처리
		if (HasAuthority())
		{
			// GAS를 통해 이펙트(데미지/상태이상) 적용
			if (UAbilitySystemComponent* TargetASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(OtherActor))
			{
				if (DamageEffectSpecHandle.IsValid())
				{
					TargetASC->ApplyGameplayEffectSpecToSelf(*DamageEffectSpecHandle.Data.Get());
				}
			}
			
			if (AActor* InstigatorActor = GetInstigator())
			{
				if (UAbilitySystemComponent* InstigatorASC = UAbilitySystemBlueprintLibrary::GetAbilitySystemComponent(InstigatorActor))
				{
					FGameplayCueParameters Params;
					Params.Location = GetActorLocation();
					Params.Normal = SweepResult.ImpactNormal;
					Params.Instigator = InstigatorActor;
					Params.EffectCauser = this;

					// 던진 사람의 ASC를 통해 모든 클라이언트에게 "여기서 터졌다"고 방송
					InstigatorASC->ExecuteGameplayCue(MyGameplayTags::GameplayCue_Grenade_Explosion, Params);
				}
			}
			Destroy();
		}
	}
}


