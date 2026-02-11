// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyCPPProjectPlayerState.h"
#include "AbilitySystemComponent.h"
#include "Advanced/MyAttributeSet.h"
#include "Component/MyInventoryComponent.h"

AMyCPPProjectPlayerState::AMyCPPProjectPlayerState()
{
	
	// ASC 생성
	AbilitySystemComponent = CreateDefaultSubobject<UAbilitySystemComponent>(TEXT("AbilitySystemComponent"));
	AbilitySystemComponent->SetIsReplicated(true);
	AbilitySystemComponent->SetReplicationMode(EGameplayEffectReplicationMode::Mixed);

	// AttributeSet 생성
	AttributeSet = CreateDefaultSubobject<UMyAttributeSet>(TEXT("AttributeSet"));
	
	InventoryComponent = CreateDefaultSubobject<UMyInventoryComponent>(TEXT("InventoryComponent"));
}

UAbilitySystemComponent* AMyCPPProjectPlayerState::GetAbilitySystemComponent() const
{
	return AbilitySystemComponent;
}

void AMyCPPProjectPlayerState::CopyProperties(APlayerState* PlayerState)
{
	Super::CopyProperties(PlayerState);
	
	// PlayerState는 새로 생성된 새 레벨의 객체입니다.
	if (AMyCPPProjectPlayerState* NewPlayerState = Cast<AMyCPPProjectPlayerState>(PlayerState))
	{
		UMyInventoryComponent* OldInv = this->GetInventoryComponent();
		UMyInventoryComponent* NewInv = NewPlayerState->GetInventoryComponent();

		if (OldInv && NewInv)
		{
			// [바통 터치] 이전 데이터셋을 새 컴포넌트에 통째로 복사
			NewInv->SetInventoryData(OldInv->GetCurrentGold(), OldInv->GetOwnedItems(), OldInv->GetBattleItemSlots());
		}
	}
}
