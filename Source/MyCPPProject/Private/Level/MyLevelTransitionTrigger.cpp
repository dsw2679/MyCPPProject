// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MyLevelTransitionTrigger.h"

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
		if (!TargetLevel.IsNull())
		{
			// 레벨 전환 실행
			//UGameplayStatics::OpenLevelBySoftObjectPtr(this, TargetLevel);
			UWorld* World = GetWorld();
			if (World)
			{
				// 더 안전한 경로 추출 방식
				FString LevelPath = TargetLevel.GetAssetName();

				UE_LOG(LogTemp, Warning, TEXT("[Travel] Attempting ServerTravel to: %s"), *LevelPath);
				// ServerTravel 실행 (Seamless Travel의 핵심 함수)
				// 첫 번째 인자는 이동할 레벨 경로, 두 번째는 절대 경로 여부
				World->ServerTravel(LevelPath, false);
			}
		}
	}
}


