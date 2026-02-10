// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "UI/MySkillSlotWidget.h"
#include "UI/MyItemSlotWidget.h"
#include "Message/MyInventoryMessages.h"
#include "MyPlayerHUDWidget.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyPlayerHUDWidget : public UMyUserWidget
{
	GENERATED_BODY()
	
public:
	// ASC가 설정될 때 스탯 바인딩을 수행합니다.
	virtual void SetAbilitySystemComponent(UAbilitySystemComponent* InASC) override;

protected:
	virtual void NativeConstruct() override;
	
	virtual void NativeOnActivated() override;
	
	// Attribute(스탯) 변경 시 호출됨
	virtual void OnAttributeChanged(const FGameplayAttribute& Attribute, float NewValue) override;

	// 쿨타임 태그 변경 감지용 (Q, W, E 등 스킬 쿨타임 표시)
	virtual void OnCooldownTagChanged(const FGameplayTag Tag, int32 NewCount);

	// 바인딩할 스킬 슬롯들
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_Q;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_W;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_E;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_R;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_A;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_S;
	
	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_D;

	UPROPERTY(meta = (BindWidget))
	TObjectPtr<UMySkillSlotWidget> Slot_F;
	
	// 배틀 아이템 슬롯 4개 추가
	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player UI|Item")
	TObjectPtr<UMyItemSlotWidget> ItemSlot_1;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player UI|Item")
	TObjectPtr<UMyItemSlotWidget> ItemSlot_2;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player UI|Item")
	TObjectPtr<UMyItemSlotWidget> ItemSlot_3;

	UPROPERTY(BlueprintReadOnly, meta = (BindWidget), Category = "Player UI|Item")
	TObjectPtr<UMyItemSlotWidget> ItemSlot_4;
	

	// BP에서 실제 UI 요소를 갱신할 이벤트들
	UFUNCTION(BlueprintImplementableEvent, Category = "Player UI")
	void BP_UpdateHealth(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player UI")
	void BP_UpdateMana(float Current, float Max);

	UFUNCTION(BlueprintImplementableEvent, Category = "Player UI")
	void BP_UpdateIdentity(float Current, float Max);

	// 쿨타임 시작/종료를 BP에 알림 (TimeRemaining 등은 BP에서 처리 가능)
	UFUNCTION(BlueprintImplementableEvent, Category = "Player UI")
	void BP_OnCooldownChanged(FGameplayTag SkillTag, bool bIsCooldown);
	
	// 인벤토리 메시지 수신시 호출될 bp 이벤트
	UFUNCTION(BlueprintImplementableEvent, Category = "Player UI")
	void BP_UpdateInventory(const FMyInventoryMessage& Message);
	
	// 메시지라우터 수신용 콜백 함수
	void OnInventoryMessageReceived(FGameplayTag Channel, const FMyInventoryMessage& Message);
	
	// 입력모드 전환
	virtual TOptional<FUIInputConfig> GetDesiredInputConfig() const override;

private:
	// 현재 값을 저장하여 비율 계산에 사용
	float CurrentHealth = 0.0f;
	float MaxHealth = 1.0f;
	float CurrentMana = 0.0f;
	float MaxMana = 1.0f;
	
	// 태그로 슬롯을 빨리 찾기 위한 맵
	TMap<FGameplayTag, TObjectPtr<UMySkillSlotWidget>> SkillSlotMap;
};
