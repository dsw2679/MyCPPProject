// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyPlayerHUDWidget.h"
#include "AbilitySystemComponent.h"
#include "AbilitySystemGlobals.h"
#include "Advanced/MyAttributeSet.h"
#include "MyGameplayTags.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "UI/MySkillSlotWidget.h"

void UMyPlayerHUDWidget::SetAbilitySystemComponent(UAbilitySystemComponent* InASC)
{
	Super::SetAbilitySystemComponent(InASC);
	
	if (InASC)
	{
		// 주요 스탯 바인딩
		BindAttribute(UMyAttributeSet::GetHealthAttribute());
		BindAttribute(UMyAttributeSet::GetMaxHealthAttribute());
		BindAttribute(UMyAttributeSet::GetMPAttribute());
		BindAttribute(UMyAttributeSet::GetMaxMPAttribute());
		BindAttribute(UMyAttributeSet::GetIdentityGageAttribute());

		// 쿨타임 태그 이벤트 등록 (Q, W, E, R 스킬 등)
		TArray<FGameplayTag> CooldownTags = {
			MyGameplayTags::Cooldown_Skill_Q, MyGameplayTags::Cooldown_Skill_W,
			MyGameplayTags::Cooldown_Skill_E, MyGameplayTags::Cooldown_Skill_R,
			MyGameplayTags::Cooldown_Skill_A, MyGameplayTags::Cooldown_Skill_S,
			MyGameplayTags::Cooldown_Skill_D, MyGameplayTags::Cooldown_Skill_F,
			MyGameplayTags::Cooldown_Skill_Space
		};

		for (const FGameplayTag& Tag : CooldownTags)
		{
			InASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				 .AddUObject(this, &UMyPlayerHUDWidget::OnCooldownTagChanged);
		}
		
		// 아이템 쿨타임 태그 바인딩
		TArray<FGameplayTag> ItemTags = {
			MyGameplayTags::Cooldown_Item_1, MyGameplayTags::Cooldown_Item_2,
			MyGameplayTags::Cooldown_Item_3, MyGameplayTags::Cooldown_Item_4
		};

		for (const FGameplayTag& Tag : ItemTags)
		{
			InASC->RegisterGameplayTagEvent(Tag, EGameplayTagEventType::NewOrRemoved)
				 .AddUObject(this, &UMyPlayerHUDWidget::OnCooldownTagChanged);
		}
	}
}

void UMyPlayerHUDWidget::NativeConstruct()
{
	Super::NativeConstruct();
	
	if (Slot_Q) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_Q, Slot_Q);
	if (Slot_W) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_W, Slot_W);
	if (Slot_E) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_E, Slot_E);
	if (Slot_R) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_R, Slot_R);
	if (Slot_A) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_A, Slot_A);
	if (Slot_S) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_S, Slot_S);
	if (Slot_D) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_D, Slot_D);
	if (Slot_F) SkillSlotMap.Add(MyGameplayTags::Cooldown_Skill_F, Slot_F);
	if (ItemSlot_1) SkillSlotMap.Add(MyGameplayTags::Cooldown_Item_1, ItemSlot_1);
	if (ItemSlot_2) SkillSlotMap.Add(MyGameplayTags::Cooldown_Item_2, ItemSlot_2);
	if (ItemSlot_3) SkillSlotMap.Add(MyGameplayTags::Cooldown_Item_3, ItemSlot_3);
	if (ItemSlot_4) SkillSlotMap.Add(MyGameplayTags::Cooldown_Item_4, ItemSlot_4);

	// 안전장치: 컨트롤러에서 설정해주지 않았거나 타이밍이 늦었을 경우, 스스로 찾아서 연결
	if (!AbilitySystemComponent.IsValid())
	{
		if (APawn* OwningPawn = GetOwningPlayerPawn())
		{
			if (UAbilitySystemComponent* ASC = UAbilitySystemGlobals::GetAbilitySystemComponentFromActor(OwningPawn))
			{
				SetAbilitySystemComponent(ASC);
				UE_LOG(LogTemp, Warning, TEXT("[UI] MyPlayerHUDWidget: Found ASC in NativeConstruct and Connected."));
			}
		}
	}
	
	// 인벤토리 메시지 구독
	if (UWorld* World = GetWorld())
	{
		UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(World);

		// 메시지 수신 시 OnInventoryMessageReceived 호출하도록 연결
		MessageSubsystem.RegisterListener(MyGameplayTags::Message_Inventory_Updated, this, &UMyPlayerHUDWidget::OnInventoryMessageReceived);
	}
}

void UMyPlayerHUDWidget::NativeOnActivated()
{
	Super::NativeOnActivated();
	
}

void UMyPlayerHUDWidget::OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue)
{
	Super::OnAttributeChanged(Attribute, NewValue);
	
	// 각 스탯 변화에 따라 BP 이벤트 호출
	if (Attribute == UMyAttributeSet::GetHealthAttribute())
	{
		CurrentHealth = NewValue;
		BP_UpdateHealth(CurrentHealth, MaxHealth);
	}
	else if (Attribute == UMyAttributeSet::GetMaxHealthAttribute())
	{
		MaxHealth = NewValue;
		BP_UpdateHealth(CurrentHealth, MaxHealth);
	}
	else if (Attribute == UMyAttributeSet::GetMPAttribute())
	{
		CurrentMana = NewValue;
		BP_UpdateMana(CurrentMana, MaxMana);
	}
	else if (Attribute == UMyAttributeSet::GetMaxMPAttribute())
	{
		MaxMana = NewValue;
		BP_UpdateMana(CurrentMana, MaxMana);
	}
	else if (Attribute == UMyAttributeSet::GetIdentityGageAttribute())
	{
		// 아이덴티티 게이지는 일단 Max를 100(기획에 따라 다름)으로 가정하거나
		// 별도 Attribute가 없다면 고정값 전달
		BP_UpdateIdentity(NewValue, 100.0f);
	}
}

void UMyPlayerHUDWidget::OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount)
{
	// [추가] 이 로그가 뜨는데 UI가 반응 없으면 -> SkillSlotMap.Find 실패
	UE_LOG(LogTemp, Warning, TEXT("[UI DEBUG] Cooldown Tag Changed! Tag: %s, Count: %d"), *Tag.ToString(), NewCount);
	
	// 해당 태그를 담당하는 슬롯이 있는지 찾음
	if (TObjectPtr<UMySkillSlotWidget>* FoundSlot = SkillSlotMap.Find(Tag))
	{
		// 현재 쿨타임 상태 (NewCount > 0)
		bool bNewCooldownState = (NewCount > 0);

		// 슬롯의 현재 상태를 가져옴 (변수 접근이 안 된다면 함수 추가 필요)
		// 하지만 더 쉬운 방법: NewCount가 1일 때만 '시작'으로 간주!
		// (NewCount가 2, 3... 으로 늘어나는 건 무시)

		if (bNewCooldownState && NewCount > 1)
		{
			// 이미 쿨타임 중인데 또 태그가 추가된 경우? -> 무시
			return;
		}
		
		float RemainingTime = 0.0f;
		float TotalDuration = 0.0f; // 추가: 총 쿨타임 시간

		if (bNewCooldownState && AbilitySystemComponent.IsValid())
		{
			FGameplayEffectQuery Query = FGameplayEffectQuery::MakeQuery_MatchAnyOwningTags(FGameplayTagContainer(Tag));

			// 1. 남은 시간과 총 지속시간(Duration)을 가져옵니다.
			TArray<TPair<float, float>> DurationArray = AbilitySystemComponent->GetActiveEffectsTimeRemainingAndDuration(Query);

			if (DurationArray.Num() > 0)
			{
				RemainingTime = DurationArray[0].Key;   // 남은 시간
				TotalDuration = DurationArray[0].Value; // 총 쿨타임 시간
			}
		}

		// 위젯의 변수를 직접 설정하거나 함수를 호출합니다.
		(*FoundSlot)->UpdateCooldown(bNewCooldownState, RemainingTime, TotalDuration);
	}
}

void UMyPlayerHUDWidget::OnInventoryMessageReceived(FGameplayTag Channel, const FMyInventoryMessage& Message)
{
	BP_UpdateInventory(Message);
}

TOptional<FUIInputConfig> UMyPlayerHUDWidget::GetDesiredInputConfig() const
{
	return FUIInputConfig(ECommonInputMode::All, EMouseCaptureMode::NoCapture);
}
