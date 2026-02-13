// Fill out your copyright notice in the Description page of Project Settings.


#include "Component/MyCinematicComponent.h"

#include "LevelSequenceActor.h"
#include "LevelSequencePlayer.h"
#include "MyCPPProjectPlayerController.h"
#include "Kismet/GameplayStatics.h"

UMyCinematicComponent::UMyCinematicComponent()
{
	PrimaryComponentTick.bCanEverTick = false;
}

void UMyCinematicComponent::PlayIntroSequence()
{
	AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(GetOwner());
	if (!PC) return;

	// 레벨에서 시퀀서 찾기
	AActor* FoundActor = UGameplayStatics::GetActorOfClass(GetWorld(), ALevelSequenceActor::StaticClass());
	CurrentSequenceActor = Cast<ALevelSequenceActor>(FoundActor);

	if (CurrentSequenceActor)
	{
		ULevelSequencePlayer* Player = CurrentSequenceActor->GetSequencePlayer();
		if (Player)
		{
			Player->OnFinished.AddDynamic(this, &UMyCinematicComponent::OnSequenceFinished);

			// 연출 시작 시 세팅
			PC->SetIgnoreMoveInput(true);
			PC->bShowMouseCursor = false;
			PC->SetHUDVisibility(false);

			Player->Play();
			UE_LOG(LogTemp, Log, TEXT("[Cinematic] Intro Sequence Started."));
		}
	}
}

void UMyCinematicComponent::OnSequenceFinished()
{
	AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(GetOwner());
	if (!PC) return;

	// 시네카메라 -> 플레이어 카메라 블렌딩
	PC->SetViewTargetWithBlend(PC->GetPawn(), BlendTime, VTBlend_Cubic);

	// 블렌딩 시간만큼 대기 후 조작 복구
	FTimerHandle RestoreTimer;
	GetWorld()->GetTimerManager().SetTimer(RestoreTimer, this, &UMyCinematicComponent::RestorePlayerControl, BlendTime, false);
}

void UMyCinematicComponent::RestorePlayerControl()
{
	if (AMyCPPProjectPlayerController* PC = Cast<AMyCPPProjectPlayerController>(GetOwner()))
	{
		PC->CreateHUD();
		PC->SetIgnoreMoveInput(false);
		PC->bShowMouseCursor = true;
		UE_LOG(LogTemp, Log, TEXT("[Cinematic] Control Restored."));
		
		
	}
}


