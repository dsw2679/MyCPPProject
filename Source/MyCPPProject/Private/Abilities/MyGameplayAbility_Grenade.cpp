// Fill out your copyright notice in the Description page of Project Settings.


#include "Abilities/MyGameplayAbility_Grenade.h"

#include "Advanced/MyAttributeSet.h"
#include "Effect/MyGrenadeProjectile.h"

void UMyGameplayAbility_Grenade::SpawnProjectile(TSubclassOf<AMyGrenadeProjectile> ProjectileClass,
                                                 FVector TargetLocation)
{
	// 서버 권한 확인
	if (!GetOwningActorFromActorInfo()->HasAuthority()) return;
	if (!ProjectileClass) return;

	AActor* Avatar = GetAvatarActorFromActorInfo();
	if (!Avatar) return;

	// 캐릭터 위치에서 약간 위쪽에서 생성
	FVector SpawnLocation = Avatar->GetActorLocation() + Avatar->GetActorForwardVector() * 50.f + FVector(0,0,100.f);

	// 목표 지점을 향한 각도 계산
	FRotator SpawnRotation = (TargetLocation - SpawnLocation).Rotation();

	FActorSpawnParameters SpawnParams;
	SpawnParams.Owner = Avatar;
	SpawnParams.Instigator = Cast<APawn>(Avatar);

	// 투사체 스폰
	if (AMyGrenadeProjectile* Projectile = GetWorld()->SpawnActor<AMyGrenadeProjectile>(ProjectileClass, SpawnLocation, SpawnRotation, SpawnParams))
	{
		// 이펙트 스펙 생성 후 투사체에 주입
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(GrenadeEffectClass, GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// 공격력 계산 및 주입
			if (UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo())
			{
				float BaseAttackPower = ASC->GetNumericAttribute(UMyAttributeSet::GetAttackPowerAttribute());
				if (BaseAttackPower <= 0.0f) BaseAttackPower = 10.0f;

				float FinalDamage = BaseAttackPower * DamageMultiplier;

				// Damage와 Stagger 수치 주입
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(DamageTag, FinalDamage);
				SpecHandle.Data.Get()->SetSetByCallerMagnitude(StaggerTag, StaggerValue);
			}

			// 투사체에 전달
			Projectile->DamageEffectSpecHandle = SpecHandle;
		}
	}
}
