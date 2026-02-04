// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UI/MyUserWidget.h"
#include "UI/MySkillSlotWidget.h"
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

private:
	// 현재 값을 저장하여 비율 계산에 사용
	float CurrentHealth = 0.0f;
	float MaxHealth = 1.0f;
	float CurrentMana = 0.0f;
	float MaxMana = 1.0f;
	
	// 태그로 슬롯을 빨리 찾기 위한 맵
	TMap<FGameplayTag, TObjectPtr<UMySkillSlotWidget>> SkillSlotMap;
	
};
