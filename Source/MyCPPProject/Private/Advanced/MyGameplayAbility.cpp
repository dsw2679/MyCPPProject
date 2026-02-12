// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyGameplayAbility.h"
#include "AbilitySystemComponent.h"
#include "MyCPPProjectPlayerController.h"
#include "MyGameplayTags.h"
#include "Advanced/MyAttributeSet.h"
#include "GameFramework/PlayerController.h"
#include "GameFramework/Character.h"
#include "GameFramework/CharacterMovementComponent.h"
#include "Message/MyBossMessageStruct.h"

UMyGameplayAbility::UMyGameplayAbility()
{
	// 기본적으로 스킬은 인스턴스화되어 캐릭터마다 독립적으로 실행되도록 설정
	InstancingPolicy = EGameplayAbilityInstancingPolicy::InstancedPerActor;
}

bool UMyGameplayAbility::IsAbilityActive() const
{
	// 현재 능력이 활성화(시전 중) 상태인지 확인
	return IsActive();
}

FGameplayEffectSpecHandle UMyGameplayAbility::MakeEffectSpecWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass,
	FGameplayTag DataTag, float Magnitude)
{
	// 1. ASC 가져오기
	UAbilitySystemComponent* ASC = GetAbilitySystemComponentFromActorInfo();
	if (!ASC || !EffectClass) return FGameplayEffectSpecHandle();

	// 2. Outgoing Spec 생성
	FGameplayEffectContextHandle Context = ASC->MakeEffectContext();
	Context.AddSourceObject(GetAvatarActorFromActorInfo());

	FGameplayEffectSpecHandle SpecHandle = ASC->MakeOutgoingSpec(EffectClass, GetAbilityLevel(), Context);

	// 3. SetByCaller 데이터 주입
	if (SpecHandle.IsValid())
	{
		SpecHandle.Data->SetSetByCallerMagnitude(DataTag, Magnitude);
	}

	return SpecHandle;
}

void UMyGameplayAbility::ApplyEffectWithSetByCaller(TSubclassOf<UGameplayEffect> EffectClass, FGameplayTag DataTag,
	float Magnitude)
{
	FGameplayEffectSpecHandle SpecHandle = MakeEffectSpecWithSetByCaller(EffectClass, DataTag, Magnitude);

	if (SpecHandle.IsValid())
	{
		ApplyGameplayEffectSpecToOwner(CurrentSpecHandle, CurrentActorInfo, CurrentActivationInfo, SpecHandle);
	}
}

const FGameplayTagContainer* UMyGameplayAbility::GetCooldownTags() const
{
	FGameplayTagContainer* MutableTags = const_cast<FGameplayTagContainer*>(&TempCooldownTags);
	MutableTags->Reset(); // 초기화

	if (CooldownTag.IsValid())
	{
		// 설정된 태그만 반환
		MutableTags->AddTag(CooldownTag);
		return MutableTags;
	}

	// 설정된 게 없으면 부모 로직(GE의 태그 확인)을 따름
	return Super::GetCooldownTags();
}

bool UMyGameplayAbility::GetHeroCursorHit(FVector& OutLocation)
{
	// 1. 현재 어빌리티의 주인(ActorInfo)으로부터 플레이어 컨트롤러를 가져옵니다.
	APlayerController* PC = GetActorInfo().PlayerController.Get();

	// 혹시 ActorInfo에 캐싱이 안 되어 있다면 아바타(캐릭터)를 통해 다시 시도
	if (!PC)
	{
		if (APawn* Avatar = Cast<APawn>(GetAvatarActorFromActorInfo()))
		{
			PC = Cast<APlayerController>(Avatar->GetController());
		}
	}

	// 2. 컨트롤러가 있다면 Hit Result를 계산합니다.
	if (PC)
	{
		FHitResult Hit;
		// Visibility 채널을 사용하여 마우스 아래를 검사
		if (PC->GetHitResultUnderCursor(ECollisionChannel::ECC_Visibility, true, Hit))
		{
			OutLocation = Hit.Location;
			return true; // 성공!
		}
	}

	// 실패 시 0 벡터 반환
	OutLocation = FVector::ZeroVector;
	return false;
}

void UMyGameplayAbility::RotateToCursor()
{
	// 아바타(캐릭터) 가져오기
	AActor* AvatarActor = GetAvatarActorFromActorInfo();
	if (!AvatarActor) return;

	// 마우스 커서 위치 가져오기
	FVector CursorLocation;
	if (GetHeroCursorHit(CursorLocation))
	{
		// 캐릭터 위치 가져오기
		FVector StartLocation = AvatarActor->GetActorLocation();

		// 바라볼 회전값 계산 (FindLookAtRotation 기능)
		// KismetMathLibrary::FindLookAtRotation와 동일한 로직
		FRotator LookAtRotation = (CursorLocation - StartLocation).Rotation();

		// Yaw(수평 회전)만 적용하고 Pitch/Roll은 0으로 고정 (기울어짐 방지)
		FRotator NewRotation = FRotator(0.0f, LookAtRotation.Yaw, 0.0f);

		// 회전 적용
		AvatarActor->SetActorRotation(NewRotation);
	}
}

void UMyGameplayAbility::LaunchAvatarCharacter(FVector LaunchVelocity, bool bXYOverride, bool bZOverride)
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		Character->LaunchCharacter(LaunchVelocity, bXYOverride, bZOverride);
	}
}

void UMyGameplayAbility::SetGravityScale(float NewGravityScale)
{
	if (ACharacter* Character = Cast<ACharacter>(GetAvatarActorFromActorInfo()))
	{
		if (UCharacterMovementComponent* CMC = Character->GetCharacterMovement())
		{
			CMC->GravityScale = NewGravityScale;
		}
	}
}

void UMyGameplayAbility::ApplyCooldown(const FGameplayAbilitySpecHandle Handle,
                                       const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CooldownGE = GetCooldownGameplayEffect();
	if (CooldownGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CooldownGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// 쿨타임 시간 주입 (기본 태그: SetByCaller.Cooldown)
			FGameplayTag DurationTag = FGameplayTag::RequestGameplayTag(FName("SetByCaller.Cooldown"));
			SpecHandle.Data->SetSetByCallerMagnitude(DurationTag, CooldownDuration);
			
			// 쿨다운 태그 동적 주입
			if (CooldownTag.IsValid())
			{
				SpecHandle.Data->DynamicGrantedTags.AddTag(CooldownTag);
			}
			ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
		}
	}
}

void UMyGameplayAbility::ApplyCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
	const FGameplayAbilityActivationInfo ActivationInfo) const
{
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	
	if (CostGE)
	{
		FGameplayEffectSpecHandle SpecHandle = MakeOutgoingGameplayEffectSpec(CostGE->GetClass(), GetAbilityLevel());
		if (SpecHandle.IsValid())
		{
			// Map에 있는 모든 비용 태그와 값을 주입
			for (const TPair<FGameplayTag, float>& Pair : CostMap)
			{
				SpecHandle.Data->SetSetByCallerMagnitude(Pair.Key, Pair.Value);
			}
	
			FActiveGameplayEffectHandle ActiveGE = ApplyGameplayEffectSpecToOwner(Handle, ActorInfo, ActivationInfo, SpecHandle);
			UE_LOG(LogTemp, Warning, TEXT("ApplyCost Result: %s"), ActiveGE.WasSuccessfullyApplied() ? TEXT("SUCCESS") : TEXT("FAILED"));
		}
	}
}

bool UMyGameplayAbility::CheckCost(const FGameplayAbilitySpecHandle Handle, const FGameplayAbilityActorInfo* ActorInfo,
                                   FGameplayTagContainer* OptionalRelevantTags) const
{
	// 기본 비용 GE 가져오기
	UGameplayEffect* CostGE = GetCostGameplayEffect();
	if (!CostGE)
	{
		return true;
	}

	// ActorInfo 및 ASC 유효성 검사
	if (!ActorInfo || !ActorInfo->AbilitySystemComponent.IsValid())
	{
		return false;
	}

	// Spec 생성
	FGameplayEffectContextHandle Context = ActorInfo->AbilitySystemComponent->MakeEffectContext();
	Context.AddSourceObject(this);

	// CDO를 사용해 Spec 생성
	FGameplayEffectSpec Spec(CostGE, Context, GetAbilityLevel());

	// SetByCaller 값 주입
	for (const TPair<FGameplayTag, float>& Pair : CostMap)
	{
		Spec.SetSetByCallerMagnitude(Pair.Key, Pair.Value);
	}

	// 직접 속성 검사 (Super::CheckCost를 부르면 에러가 나므로 직접 구현)
	for (int32 ModIdx = 0; ModIdx < Spec.Modifiers.Num(); ++ModIdx)
	{
		const FGameplayModifierInfo& ModDef = Spec.Def->Modifiers[ModIdx];
		const FGameplayAttribute& Attribute = ModDef.Attribute;

		// 이 Modifier가 SetByCaller를 사용하는지 확인
		if (ModDef.ModifierMagnitude.GetMagnitudeCalculationType() == EGameplayEffectMagnitudeCalculation::SetByCaller)
		{
			// 우리가 주입한 값 가져오기 (CostMap에 있는 값)
			float CostValue = 0.0f;
			FGameplayTag Tag = ModDef.ModifierMagnitude.GetSetByCallerFloat().DataTag;
			if (CostMap.Contains(Tag))
			{
				CostValue = CostMap[Tag];
			}

			// 현재 캐릭터의 Attribute 값 가져오기
			if (ActorInfo->AbilitySystemComponent->HasAttributeSetForAttribute(Attribute))
			{
				float CurrentValue = ActorInfo->AbilitySystemComponent->GetNumericAttribute(Attribute);
				
				float AbsCost = FMath::Abs(CostValue);
				
				if (CurrentValue < AbsCost)
				{
					// 실패 태그 추가 (디버깅용)
					if (OptionalRelevantTags)
					{
						const FGameplayTag FailTag = FGameplayTag::RequestGameplayTag(
							FName("Ability.ActivateFail.Cost"));
						OptionalRelevantTags->AddTag(FailTag);
					}
					return false;
				}
			}
		}
	}

	return true;
}

void UMyGameplayAbility::ActivateAbility(const FGameplayAbilitySpecHandle Handle,
	const FGameplayAbilityActorInfo* ActorInfo, const FGameplayAbilityActivationInfo ActivationInfo,
	const FGameplayEventData* TriggerEventData)
{
	// 컨트롤러 가져오기
	if (ActorInfo->PlayerController.IsValid())
	{
		// 이동 멈추기 (StopMovement)
		ActorInfo->PlayerController->StopMovement();
		
	}
	Super::ActivateAbility(Handle, ActorInfo, ActivationInfo, TriggerEventData);
}

AMyCPPProjectPlayerController* UMyGameplayAbility::GetMyPlayerController() const
{
	return Cast<AMyCPPProjectPlayerController>(GetActorInfo().PlayerController.Get());
}

void UMyGameplayAbility::BroadcastBossDeathMessage()
{
	if (UWorld* World = GetWorld())
	{
		// 전송할 메시지(Payload) 구성
		FMyBossMessageStruct Message;
		Message.BossActor = GetAvatarActorFromActorInfo(); // 죽은 보스 자신
		Message.BossASC = GetAbilitySystemComponentFromActorInfo();

		// 메시지 시스템을 통해 전송 (Channel: Message.Boss.Dead)
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);
		MessageSubsystem.BroadcastMessage(MyGameplayTags::Message_Boss_Dead, Message);
		
	}
}

