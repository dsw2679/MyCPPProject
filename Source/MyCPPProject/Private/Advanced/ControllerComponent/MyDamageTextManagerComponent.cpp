// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/ControllerComponent/MyDamageTextManagerComponent.h"

#include "GameFramework/PlayerController.h"
#include "Blueprint/UserWidget.h"
#include "Kismet/GameplayStatics.h"
#include "MyGameplayTags.h"
#include "UI/MyDamageTextWidget.h"

UMyDamageTextManagerComponent::UMyDamageTextManagerComponent(const FObjectInitializer& ObjectInitializer) : Super(ObjectInitializer)
{
	// 틱 불필요
	PrimaryComponentTick.bCanEverTick = false;
	PrimaryComponentTick.bStartWithTickEnabled = false;
}

void UMyDamageTextManagerComponent::BeginPlay()
{
	Super::BeginPlay();
	
	// 로컬 플레이어 컨트롤러인지 확인 (서버나 AI는 UI 안 띄움)
	if (APlayerController* PC = GetController<APlayerController>())
	{
		if (PC->IsLocalController())
		{
			UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
			ListenerHandle = MessageSubsystem.RegisterListener(
				MyGameplayTags::Message_Damage,
				this,
				&UMyDamageTextManagerComponent::OnDamageMessageReceived
			);
		}
	}
}

void UMyDamageTextManagerComponent::EndPlay(const EEndPlayReason::Type EndPlayReason)
{
	UGameplayMessageSubsystem& MessageSubsystem = UGameplayMessageSubsystem::Get(this);
	MessageSubsystem.UnregisterListener(ListenerHandle);
	
	Super::EndPlay(EndPlayReason);
}

void UMyDamageTextManagerComponent::OnDamageMessageReceived(FGameplayTag Channel, const FMyDamageMessage& Message)
{
	if (!DamageTextClass || !Message.TargetActor) return;

	UE_LOG(LogTemp, Warning, TEXT("DAMAGE MSG RECEIVED!"));
	
	APlayerController* PC = GetController<APlayerController>();
	if (!PC) return;

	// 위치 보정 (머리 위)
	// 나중에는 TargetActor의 Mesh 소켓(HeadSocket) 위치를 가져오는 게 더 정확합니다.
	FVector WorldLocation = Message.TargetActor->GetActorLocation() + FVector(0.f, 0.f, 100.f); // 높이 조절 필요

	// 랜덤 퍼짐 (텍스트 겹침 방지)
	float RandomX = FMath::RandRange(-40.f, 40.f);
	float RandomY = FMath::RandRange(-40.f, 40.f);

	// 좌표 변환
	FVector2D ScreenPosition;
	bool bProjected = UGameplayStatics::ProjectWorldToScreen(PC, WorldLocation, ScreenPosition);

	if (bProjected)
	{
		// 위젯 생성
		UUserWidget* Widget = CreateWidget<UUserWidget>(PC, DamageTextClass);
		if (Widget)
		{
			Widget->AddToViewport();
			Widget->SetPositionInViewport(ScreenPosition + FVector2D(RandomX, RandomY));

			// 값 전달 (C++ 클래스 캐스팅 권장)
			if (UMyDamageTextWidget* DmgWidget = Cast<UMyDamageTextWidget>(Widget))
			{
				DmgWidget->SetDamage(Message.DamageAmount);
			}
			// 만약 BP만 쓴다면 아래처럼 호출 (느림)
			/*
			else
			{
				FOutputDeviceNull Ar;
				FString Cmd = FString::Printf(TEXT("SetDamage %f"), Message.DamageAmount);
				Widget->CallFunctionByNameWithArguments(*Cmd, Ar, NULL, true);
			}
			*/
		}
	}
}
