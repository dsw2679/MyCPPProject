// Fill out your copyright notice in the Description page of Project Settings.


#include "UI/MyBossHealthWidget.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "MyGameplayTags.h"


void UMyBossHealthWidget::NativeConstruct()
{
	Super::NativeConstruct();

	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);

	// 체력 메시지 리스너 등록
	// 보스가 "Message.Boss.Health" 채널로 메시지를 보내면 OnHealthMessageReceived가 실행됨
	HealthMessageHandle = MessageSubsystem.RegisterListener(
		MyGameplayTags::Message_Boss_HealthChanged,
		this,
		&UMyBossHealthWidget::OnHealthMessageReceived
	);
	
	// 무력화 메시지 리스너
	StaggerMessageHandle = MessageSubsystem.RegisterListener(
	MyGameplayTags::Message_Boss_StaggerChanged,
	this,
	&UMyBossHealthWidget::OnStaggerMessageReceived
	);
	
	InitInfoMessageHandle = MessageSubsystem.RegisterListener(
	MyGameplayTags::Message_Boss_InitInfo,
	this,
	&UMyBossHealthWidget::OnInitInfoMessageReceived
);
}

void UMyBossHealthWidget::NativeDestruct()
{
	// 위젯이 파괴될 때 리스너 해제 (메모리 누수 방지)
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(HealthMessageHandle);
	MessageSubsystem.UnregisterListener(StaggerMessageHandle);
	MessageSubsystem.UnregisterListener(InitInfoMessageHandle);

	Super::NativeDestruct();
}

void UMyBossHealthWidget::OnHealthMessageReceived(FGameplayTag Channel, const FMyBossHealthMessage& Message)
{
	BP_UpdateBossHealth(Message.CurrentHealth, Message.MaxHealth);
}

void UMyBossHealthWidget::OnStaggerMessageReceived(FGameplayTag Channel, const FMyBossStaggerMessage& Message)
{
	BP_UpdateBossStagger(Message.CurrentStagger, Message.MaxStagger);
}

void UMyBossHealthWidget::OnInitInfoMessageReceived(FGameplayTag Channel, const FMyBossInfoMessage& Message)
{
	BP_InitBossInfo(Message.BossName, Message.EnrageTimeLimit);
}
