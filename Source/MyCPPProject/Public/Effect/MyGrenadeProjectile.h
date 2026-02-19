// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "GameplayEffectTypes.h"
#include "MyGrenadeProjectile.generated.h"

class USphereComponent;
class UProjectileMovementComponent;
class UGameplayEffect;

/*
 * 수류탄, 폭탄 투사체 액터 
 */
UCLASS()
class MYCPPPROJECT_API AMyGrenadeProjectile : public AActor
{
	GENERATED_BODY()
public:	
	AMyGrenadeProjectile();

protected:
	virtual void BeginPlay() override;
	
	// 충돌 시 호출될 함수
	UFUNCTION()
	void OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor, UPrimitiveComponent* OtherComp,
		int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult);

public:	
	UPROPERTY(VisibleDefaultsOnly, Category = "Projectile")
	USphereComponent* CollisionComp;

	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, Category = "Movement")
	UProjectileMovementComponent* ProjectileMovement;

	// 폭발 시 적용할 효과 (데미지, 상태이상 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "GameplayEffect")
	TSubclassOf<UGameplayEffect> ExplosionEffectClass;
	
	// 폭발 시 사운드 재생
	UPROPERTY(EditAnywhere, Category = "GameplayEffect")
	TObjectPtr<USoundBase> ExplosionSound;

	// 투사체 스펙 (데미지 수치 등을 담음)
	FGameplayEffectSpecHandle DamageEffectSpecHandle;
};
