// Fill out your copyright notice in the Description page of Project Settings.


#include "MyGameplayTags.h"
#include "GameplayTagsManager.h"

FMyGameplayTags FMyGameplayTags::GameplayTags;

void FMyGameplayTags::InitializeNativeTags()
{
	UGameplayTagsManager& Manager = UGameplayTagsManager::Get();
	GameplayTags.AddAllTags(Manager);
}

void FMyGameplayTags::AddAllTags(UGameplayTagsManager& Manager)
{
	// 실제 태그 문자열 정의 
	Message_Boss_Spawned = Manager.AddNativeGameplayTag(FName("Message.Boss.Spawned"), TEXT("보스 등장 메시지"));
	Message_Boss_Dead = Manager.AddNativeGameplayTag(FName("Message.Boss.Dead"), TEXT("보스 사망 메시지"));
}