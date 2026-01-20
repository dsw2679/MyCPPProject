// Fill out your copyright notice in the Description page of Project Settings.


#include "AnimInstance/AnimNotify/MyAnimNotify_GameplayEvent.h"

#include "Abilities/GameplayAbilityTypes.h"
#include "AbilitySystemBlueprintLibrary.h"

void UMyAnimNotify_GameplayEvent::Notify(USkeletalMeshComponent* MeshComp, UAnimSequenceBase* Animation,
                                         const FAnimNotifyEventReference& EventReference)
{
	Super::Notify(MeshComp, Animation, EventReference);
	
	if (!MeshComp) return;

	// 메시를 소유한 액터 가져오기
	AActor* Owner = MeshComp->GetOwner();
	if (Owner)
	{
		// GAS 라이브러리를 이용해 해당 캐릭터의 ASC에 이벤트 전송
		// Payload에 정보를 담을 수도 있지만, 태그만 전달하면 충분합니다.
		FGameplayEventData Payload;
		Payload.EventTag = EventTag;
		Payload.Instigator = Owner;

		UAbilitySystemBlueprintLibrary::SendGameplayEventToActor(Owner, EventTag, Payload);
	}
}
