// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "AbilitySystemInterface.h"
#include "GameplayTagContainer.h"
#include "MyAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	// 초기화 함수 (BeginPlay와 유사)
	virtual void NativeInitializeAnimation() override;

	// 매 프레임 업데이트 함수 (Tick과 유사)
	virtual void NativeUpdateAnimation(float DeltaSeconds) override;
	
protected:
	// 블루프린트에서 가져다 쓸 변수들
	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class AMyCPPProjectCharacter* Character;

	UPROPERTY(BlueprintReadOnly, Category = Movement)
	class UCharacterMovementComponent* MovementComponent;
	
	// GAS 시스템에 접근하기 위한 포인터
	UPROPERTY(BlueprintReadOnly, Category = "GAS")
	class UAbilitySystemComponent* AbilitySystemComponent;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	float GroundSpeed;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bShouldMove;

	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Movement)
	bool bIsFalling;
	
	// 특정 태그가 있는지 검사하는 헬퍼 함수
	UFUNCTION(BlueprintCallable, Category = "GAS")
	bool HasGameplayTag(FGameplayTag TagToCheck) const;
};
