// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MyLevelTransitionTrigger.h"

#include "MyCPPProjectPlayerController.h"
#include "Advanced/MyGameInstance.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"

// Sets default values
AMyLevelTransitionTrigger::AMyLevelTransitionTrigger()
{
	PrimaryActorTick.bCanEverTick = false;

	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;

	// 플레이어만 감지하도록 설정
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMyLevelTransitionTrigger::OnOverlapBegin);
}

void AMyLevelTransitionTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		if (AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0)))
		{
			if (UMyGameInstance* GI = Cast<UMyGameInstance>(GetGameInstance())) {
				GI->bIsLoadingInProgress = true;
				// 배틀 레벨로 이동함을 명시! (그래야 이미지가 맞게 나옵니다)
				GI->CurrentLoadingType = ELoadingScreenType::ToBattle;
			}

			FString LevelPath = TargetLevel.GetAssetName();
			PC->PrepareLevelTransition(LevelPath);
		}
	}
}


