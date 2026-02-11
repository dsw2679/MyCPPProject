// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MyShopAreaTrigger.h"

#include "MyCPPProjectPlayerController.h"
#include "MyHUDLayout.h"
#include "Advanced/MyPrimaryGameLayout.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

AMyShopAreaTrigger::AMyShopAreaTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMyShopAreaTrigger::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMyShopAreaTrigger::OnOverlapEnd);
}

void AMyShopAreaTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if (AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			PC->SetCanOpenInventory(true);
			
			// HUD를 찾아 WBP_PressI 표시
			if (UMyPrimaryGameLayout* RootLayout = Cast<UMyPrimaryGameLayout>(PC->GetRootLayout()))
			{
				// RootLayout에서 게임 레이어(HUD)를 가져옴
				if (UMyHUDLayout* HUD = Cast<UMyHUDLayout>(RootLayout->GetGameLayerWidget()))
				{
					// MyHUDLayout에 알림 함수 호출
					HUD->SetPressINotificationVisible(true, PressIWidgetClass);
				}
			}
		}
	}
	
}

void AMyShopAreaTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if (AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			PC->SetCanOpenInventory(false);
			// HUD를 찾아 WBP_PressI 표시
			if (UMyPrimaryGameLayout* RootLayout = Cast<UMyPrimaryGameLayout>(PC->GetRootLayout()))
			{
				// RootLayout에서 게임 레이어(HUD)를 가져옴
				if (UMyHUDLayout* HUD = Cast<UMyHUDLayout>(RootLayout->GetGameLayerWidget()))
				{
					// MyHUDLayout에 알림 함수 호출
					HUD->SetPressINotificationVisible(false, PressIWidgetClass);
				}
			}
		}
	}
}
