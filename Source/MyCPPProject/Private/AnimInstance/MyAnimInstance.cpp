// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/MyAnimInstance.h"

#include "MyCPPProjectCharacter.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "GameFramework/CharacterMovementComponent.h"

void UMyAnimInstance::NativeInitializeAnimation()
{
	Super::NativeInitializeAnimation();
	Character = Cast<AMyCPPProjectCharacter>(TryGetPawnOwner());
	if (Character)
	{
		MovementComponent = Character->GetCharacterMovement();
		AbilitySystemComponent = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(Character);
	}
}

void UMyAnimInstance::NativeUpdateAnimation(float DeltaSeconds)
{
	Super::NativeUpdateAnimation(DeltaSeconds);
	
	// 캐릭터가 유효하지 않으면 리턴
	if (!Character || !MovementComponent)
	{
		return;
	}

	// 속력 구하기 (XY 평면만)
	FVector Velocity = Character->GetVelocity();
	FVector LateralVelocity = FVector(Velocity.X, Velocity.Y, 0.f);
	GroundSpeed = LateralVelocity.Size();

	// 움직이는지 확인 (속력이 3 이상이고 가속도가 있을 때)
	//bool bHasInput = MovementComponent->GetCurrentAcceleration().Size() > 0.f;
	bShouldMove = GroundSpeed > 3.0f;

	// 공중에 떴는지 확인
	bIsFalling = MovementComponent->IsFalling();
	
	// [Lyra 스타일 활용 예시 - 나중에 구현할 것]
	// if (HasGameplayTag(FGameplayTag::RequestGameplayTag("Status.Stun")))
	// {
	//      // 기절 모션 변수 켜기
	// }
}

bool UMyAnimInstance::HasGameplayTag(FGameplayTag TagToCheck) const
{
	if (AbilitySystemComponent && TagToCheck.IsValid())
	{
		return AbilitySystemComponent->HasMatchingGameplayTag(TagToCheck);
	}
	return false;
}
