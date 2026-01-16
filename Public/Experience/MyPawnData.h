// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "GameplayTagContainer.h"
#include "MyPawnData.generated.h"

class APawn;
class UGameplayAbility;
class UMyInputConfig;
class UInputMappingContext;
class UNiagaraSystem;
class UGameplayEffect;

USTRUCT(BlueprintType)
struct FMyAbilitySet_GameplayAbility
{
	GENERATED_BODY()

public:
	// 부여할 스킬 클래스
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly)
	TSubclassOf<UGameplayAbility> Ability = nullptr;

	// 이 스킬을 발동시킬 입력 태그 (예: InputTag.Ability.Skill1)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Meta = (Categories = "InputTag"))
	FGameplayTag InputTag;
};

UCLASS(BlueprintType, Const)
class MYCPPPROJECT_API UMyPawnData : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UMyPawnData();

	// 스폰할 Pawn 클래스 
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Pawn")
	TSubclassOf<APawn> PawnClass;

	// 초기 부여할 스킬 목록
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	TArray<FMyAbilitySet_GameplayAbility> Abilities;
	
	// 기본 입력 매핑 컨텍스트
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	TObjectPtr<UInputMappingContext> DefaultMappingContext;

	// 입력 설정 (IMC, Action 매핑 등)	
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Input")
	TObjectPtr<UMyInputConfig> InputConfig;
	
	// 이동 클릭 시 발생할 이펙트 (비워두면 안 나옴)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Visuals")
	TObjectPtr<UNiagaraSystem> MovementClickEffect;
	
	// 게임 시작 시 적용할 GameplayEffect 목록 (스텟 초기화 등)
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Abilities")
	TArray<TSubclassOf<UGameplayEffect>> StartupGameplayEffects;
};
