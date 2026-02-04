// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ControllerComponent.h"
#include "GameFramework/GameplayMessageSubsystem.h"
#include "Message/MyBossMessageStruct.h"
#include "MyDamageTextManagerComponent.generated.h"

class UUserWidget;

/**
 * 데미지 메시지를 수신하여 화면에 텍스트 위젯을 띄우는 컴포넌트
 * PlayerController에 부착
 */
UCLASS(meta=(BlueprintSpawnableComponent))
class MYCPPPROJECT_API UMyDamageTextManagerComponent : public UControllerComponent
{
	GENERATED_BODY()
	
public:
	UMyDamageTextManagerComponent(const FObjectInitializer& ObjectInitializer);

protected:
	virtual void BeginPlay() override;
	virtual void EndPlay(const EEndPlayReason::Type EndPlayReason) override;

	// 메시지 수신 함수
	void OnDamageMessageReceived(FGameplayTag Channel, const FMyDamageMessage& Message);

	// 스폰할 위젯 클래스 (WBP_DamageText)
	UPROPERTY(EditDefaultsOnly, Category = "UI")
	TSubclassOf<UUserWidget> DamageTextClass;

private:
	FGameplayMessageListenerHandle ListenerHandle;
	
};
