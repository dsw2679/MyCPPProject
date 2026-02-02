// Fill out your copyright notice in the Description page of Project Settings.


#include "GameplayCue/MyGameplayCueActor_Looping.h"
#include "GameFramework/Character.h"
#include "Components/SkeletalMeshComponent.h"

void AMyGameplayCueActor_Looping::HandleGameplayCue(AActor* MyTarget, EGameplayCueEvent::Type EventType,
	const FGameplayCueParameters& Parameters)
{
	Super::HandleGameplayCue(MyTarget, EventType, Parameters);
	
	// OnActive(시작)일 때만 처리
	if (EventType == EGameplayCueEvent::OnActive)
	{
		if (ACharacter* Character = Cast<ACharacter>(MyTarget))
		{
			// 소켓 이름이 설정되어 있다면 메시에 부착
			if (!SocketName.IsNone())
			{
				AttachToComponent(Character->GetMesh(), FAttachmentTransformRules::SnapToTargetNotIncludingScale, SocketName);
			}
			else
			{
				// 소켓 없으면 그냥 루트에 부착
				AttachToActor(MyTarget, FAttachmentTransformRules::SnapToTargetNotIncludingScale);
			}
		}
	}
}
