// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Advanced/MyGameplayAbility.h"
#include "GameplayAbility_Boss.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UGameplayAbility_Boss : public UMyGameplayAbility
{
	GENERATED_BODY()
	
public:
	// 전조(Telegraph) 원형 실행 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ShowTelegraphCircle(FGameplayTag CueTag, FVector CenterLocation, float Radius, float Duration);

	// 광역 공격(Area Attack) 원형 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ApplyDamageToArea(FVector CenterLocation, float Radius, float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass);
	
	// 플레이어의 위치  계산 함수
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	FVector GetPlayerLocation(float ForwardDistance = 0.0f);
	
	// Clap 패턴용 위치 반환 (0:앞, 1:중앙, 2:뒤, 3:왼쪽, 4:오른쪽)
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	TArray<FVector> GetClapLocations(float ForwardDistance, float SideDistance);
	
	// 정사각형(박스) 전조 보여주기
	// Extent: 박스의 절반 크기 (x값만 작동)
	// Rotation: 박스의 회전 각도 (보스의 방향 등)
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ShowTelegraphRect(FGameplayTag CueTag, FVector CenterLocation, FRotator Rotation, FVector Extent, float Duration);
	
	// 사각형(박스) 범위 데미지 주기
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ApplyDamageToRect(FVector CenterLocation, FRotator Rotation, FVector Extent, float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass);

	// 직사각형 전용 전조
	// Extent: 박스의 절반 크기
	// Rotation: 박스의 회전 각도 (보스의 방향 등)
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ShowTelegraphRect_Explicit(FGameplayTag CueTag, FVector CenterLocation, FRotator Rotation, FVector Extent, float Duration);
	
	// 보스가 플레이어 방향을 바라보게 회전시키는 함수
	// RotationSpeed: 회전 속도 (0.0f = 즉시 회전)
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void FacePlayer(float RotationSpeed = 0.0f);
	
	// 부채꼴(Cone) 범위 데미지 주기 (Swing 공격용)
	// Radius: 사거리, HalfAngle: 부채꼴 절반 각도 (도 단위, 예: 45도 = 전체 90도)
	UFUNCTION(BlueprintCallable, Category = "Boss Ability")
	void ApplyDamageToCone(FVector CenterLocation, FRotator Rotation, float Radius, float HalfAngle, float DamageAmount, TSubclassOf<UGameplayEffect> DamageEffectClass);
	
protected:
	// 공격 대상(플레이어)을 찾기 위한 설정
	UPROPERTY(EditDefaultsOnly, Category = "Boss Ability")
	TArray<TEnumAsByte<EObjectTypeQuery>> HitObjectTypes;
};
