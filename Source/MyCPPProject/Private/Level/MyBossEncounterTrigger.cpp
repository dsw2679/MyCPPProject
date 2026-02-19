// Fill out your copyright notice in the Description page of Project Settings.


#include "Level/MyBossEncounterTrigger.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MyCPPProjectPlayerController.h"
#include "MyHUDLayout.h"
#include "Advanced/MyCPPProjectGameState.h"
#include "Advanced/MyPrimaryGameLayout.h"
#include "Components/AudioComponent.h"
#include "Components/BoxComponent.h"
#include "GameFramework/Character.h"
#include "Kismet/GameplayStatics.h"
#include "Sound/AmbientSound.h"


AMyBossEncounterTrigger::AMyBossEncounterTrigger()
{
	PrimaryActorTick.bCanEverTick = false;
	
	// 트리거 박스 생성 및 설정
	TriggerBox = CreateDefaultSubobject<UBoxComponent>(TEXT("TriggerBox"));
	RootComponent = TriggerBox;
	TriggerBox->SetCollisionProfileName(TEXT("Trigger"));

	// 델리게이트 바인딩
	TriggerBox->OnComponentBeginOverlap.AddDynamic(this, &AMyBossEncounterTrigger::OnOverlapBegin);
	TriggerBox->OnComponentEndOverlap.AddDynamic(this, &AMyBossEncounterTrigger::OnOverlapEnd);

	BossSpawnLocationComponent = CreateDefaultSubobject<USceneComponent>(TEXT("BossSpawnLocation"));
	BossSpawnLocationComponent->SetupAttachment(RootComponent);
}

void AMyBossEncounterTrigger::OnOverlapBegin(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex, bool bFromSweep, const FHitResult& SweepResult)
{
	if (bHasTriggered) return;

	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		// 3초 타이머 시작
		GetWorldTimerManager().SetTimer(CutsceneTimerHandle, this, &AMyBossEncounterTrigger::StartBossCutscene, 3.0f, false);
		// AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
		// if (PC && BossSequenceActor)
		// {
		// 	bHasTriggered = true;
		//
		// 	// HUD 숨기기
		// 	PC->SetHUDVisibility(false);
		//
		// 	// 입력 차단 및 마우스 커서 숨김
		// 	PC->SetIgnoreMoveInput(true);
		// 	PC->SetIgnoreLookInput(true);
		// 	PC->bShowMouseCursor = false;
		//
		// 	// 시퀀스 재생 및 종료 이벤트 바인딩
		// 	ULevelSequencePlayer* SeqPlayer = BossSequenceActor->GetSequencePlayer();
		// 	if (SeqPlayer)
		// 	{
		// 		// 시퀀스가 끝나면 OnSequenceFinished가 호출되도록 등록
		// 		SeqPlayer->OnFinished.AddDynamic(this, &AMyBossEncounterTrigger::OnSequenceFinished);
		// 		SeqPlayer->Play();
		// 	}
		// }
		// else
		// {
		// 	// 시퀀스 에셋이 할당되지 않았다면 즉시 순간이동 로직 실행
		// 	UE_LOG(LogTemp, Warning, TEXT("[Test] No Sequence found. Teleporting immediately."));
		// 	OnSequenceFinished();
		// }
	}
}

void AMyBossEncounterTrigger::OnOverlapEnd(UPrimitiveComponent* OverlappedComp, AActor* OtherActor,
	UPrimitiveComponent* OtherComp, int32 OtherBodyIndex)
{
	if (bHasTriggered) return;

	if (OtherActor && OtherActor->IsA(ACharacter::StaticClass()))
	{
		// 3초가 되기 전에 나가면 타이머 취소
		GetWorldTimerManager().ClearTimer(CutsceneTimerHandle);
	}
}

void AMyBossEncounterTrigger::StartBossCutscene()
{
	if (bHasTriggered) return;

	AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (PC)
	{
		bHasTriggered = true;

		// 보스 소환
		if (BossClass)
		{
			// 트랜스폼 가져오기
			FTransform SpawnTransform = BossSpawnTransform * GetActorTransform();

			// 스케일 강제 고정 (부모의 스케일 상속 방지)
			SpawnTransform.SetScale3D(FVector(1.0f, 1.0f, 1.0f));
			
			FActorSpawnParameters SpawnParams;
			SpawnParams.SpawnCollisionHandlingOverride = ESpawnActorCollisionHandlingMethod::AdjustIfPossibleButAlwaysSpawn;

			APawn* SpawnedBoss = GetWorld()->SpawnActor<APawn>(BossClass, SpawnTransform, SpawnParams);
			if (SpawnedBoss)
			{
				SpawnedBoss->SpawnDefaultController();
			}
		}

		// 입력 차단 및 HUD 숨기기 (기본 로직)
		PC->SetHUDVisibility(false);
		PC->SetIgnoreMoveInput(true);
		PC->SetIgnoreLookInput(true);
		PC->bShowMouseCursor = false;

		// 시퀀스 재생
		if (BossSequenceActor)
		{
			ULevelSequencePlayer* SeqPlayer = BossSequenceActor->GetSequencePlayer();
			if (SeqPlayer)
			{
				SeqPlayer->OnFinished.AddDynamic(this, &AMyBossEncounterTrigger::OnSequenceFinished);
				SeqPlayer->Play();
			}
		}
		else
		{
			OnSequenceFinished(); // 시퀀서가 없으면 즉시 전투 시작
		}
		
		// BGM 전환 로직
		if (BossBGM)
		{
			// 레벨에 배치된 기존의 모든 AmbientSound 액터를 찾아 페이드 아웃 
			TArray<AActor*> FoundBGMs;
			UGameplayStatics::GetAllActorsOfClass(GetWorld(), AAmbientSound::StaticClass(), FoundBGMs);

			for (AActor* Actor : FoundBGMs)
			{
				if (AAmbientSound* AmbientActor = Cast<AAmbientSound>(Actor))
				{
					// 2초 동안 서서히 소리를 끕니다.
					AmbientActor->GetAudioComponent()->FadeOut(2.0f, 0.0f);
				}
			}

			// 새로운 보스 BGM을 재생합니다
			BossBGMComponent = UGameplayStatics::CreateSound2D(GetWorld(), BossBGM);
			if (BossBGMComponent)
			{
				BossBGMComponent->FadeIn(4.0f, 1.0f);
			}
		}
	}
}

void AMyBossEncounterTrigger::OnSequenceFinished()
{
	AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(UGameplayStatics::GetPlayerController(this, 0));
	if (!PC) return;
	
	// 플레이어 공간 이동 (건물 안으로)
	if (APawn* PlayerPawn = PC->GetPawn())
	{
		PlayerPawn->SetActorLocation(AfterCutsceneLocation);
	}
	
	// HUD 다시 표시
	PC->SetHUDVisibility(true);

	// 입력 복구
	PC->SetIgnoreMoveInput(false);
	PC->SetIgnoreLookInput(false);
	PC->bShowMouseCursor = true;

	// 보스 UI 동적 추가 
	// PC의 RootLayoutInstance를 통해 HUD를 찾아옵니다.
	if (UMyPrimaryGameLayout* RootLayout = Cast<UMyPrimaryGameLayout>(PC->GetRootLayout())) // GetRootLayout() Getter가 필요함
	{
		if (UMyHUDLayout* HUD = Cast<UMyHUDLayout>(RootLayout->GetGameLayerWidget()))
		{
			if (BossInfoWidgetClass)
			{
				UUserWidget* BossUI = CreateWidget<UUserWidget>(PC, BossInfoWidgetClass);
				HUD->AddBossInfoWidget(BossUI);
			}
		}
	}

	// 전투 타이머 시작 (기록용)
	if (AMyCPPProjectGameState* GS = Cast<AMyCPPProjectGameState>(GetWorld()->GetGameState()))
	{
		GS->StartCombatTimer();
	}
}
