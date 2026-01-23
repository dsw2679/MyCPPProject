// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameplayTagContainer.h"

class UGameplayTagsManager;

/**
 * 프레젝트의 게임플레이 태그를 관리하는 싱글톤
 */
struct FMyGameplayTags
{
public:
	static const FMyGameplayTags& Get() { return GameplayTags; }
	static void InitializeNativeTags();

	// 여기에 태그 변수 선언
	FGameplayTag Message_Boss_Spawned;
	FGameplayTag Message_Boss_Dead;

protected:
	void AddAllTags(UGameplayTagsManager& Manager);
	static FMyGameplayTags GameplayTags;
};
