// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyExperienceManagerComponent.h"
#include "Experience/MyExperienceDefinition.h"
#include "Net/UnrealNetwork.h"

UMyExperienceManagerComponent::UMyExperienceManagerComponent(const FObjectInitializer& ObjectInitializer)
	: Super(ObjectInitializer)
{
	// 이 컴포넌트는 네트워크 복제가 필요합니다.
	SetIsReplicatedByDefault(true);
}

void UMyExperienceManagerComponent::GetLifetimeReplicatedProps(TArray<FLifetimeProperty>& OutLifetimeProps) const
{
	Super::GetLifetimeReplicatedProps(OutLifetimeProps);

	// CurrentExperience 변수를 클라이언트로 복제합니다.
	DOREPLIFETIME(ThisClass, CurrentExperience);
}

bool UMyExperienceManagerComponent::IsExperienceLoaded() const
{
	return (CurrentExperience != nullptr);
}

const UMyExperienceDefinition* UMyExperienceManagerComponent::GetCurrentExperienceChecked() const
{
	// 로드되지 않았을 때 호출하면 크래시를 유발하여 문제를 빨리 찾도록 함
	check(CurrentExperience != nullptr);
	return CurrentExperience;
}

void UMyExperienceManagerComponent::ServerSetCurrentExperience(const UMyExperienceDefinition* Experience)
{
	// 서버 권한 확인
	if (GetOwner()->GetLocalRole() != ROLE_Authority)
	{
		return;
	}

	if (CurrentExperience != Experience)
	{
		CurrentExperience = Experience;
		OnRep_CurrentExperience(); // 서버에서도 델리게이트 호출을 위해 직접 실행
	}
}

void UMyExperienceManagerComponent::OnRep_CurrentExperience()
{
	// Experience가 변경되면 델리게이트를 통해 리스너들에게 알림
	if (CurrentExperience)
	{
		OnExperienceLoaded.Broadcast(CurrentExperience);
	}
}