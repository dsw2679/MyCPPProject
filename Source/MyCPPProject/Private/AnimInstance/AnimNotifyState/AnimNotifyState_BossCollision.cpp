// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimNotifyState/AnimNotifyState_BossCollision.h"

#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"

void UAnimNotifyState_BossCollision::NotifyBegin(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	float TotalDuration, const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyBegin(MeshComp, Animation, TotalDuration, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
		if (ASC && Tag.IsValid())
		{
			// 태그 추가
			ASC->AddLooseGameplayTag(Tag);
		}
	}
}

void UAnimNotifyState_BossCollision::NotifyEnd(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
	const FAnimNotifyEventReference& EventReference)
{
	Super::NotifyEnd(MeshComp, Animation, EventReference);
	
	if (MeshComp && MeshComp->GetOwner())
	{
		UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(MeshComp->GetOwner());
		if (ASC && Tag.IsValid())
		{
			// 태그 제거
			ASC->RemoveLooseGameplayTag(Tag);
		}
	}
}
