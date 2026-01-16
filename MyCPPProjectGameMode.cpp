// Copyright Epic Games, Inc. All Rights Reserved.

#include "MyCPPProjectGameMode.h"
#include "MyCPPProjectPlayerController.h"
#include "MyCPPProjectCharacter.h"
#include "Advanced/MyCPPProjectGameState.h"
#include "Advanced/MyExperienceManagerComponent.h"
#include "Experience/MyExperienceDefinition.h"
#include "Advanced/MyCPPProjectPlayerState.h"
#include "GameFramework/GameStateBase.h"
#include "UObject/ConstructorHelpers.h"

AMyCPPProjectGameMode::AMyCPPProjectGameMode()
{
	// 기본 클래스 설정 (기존 코드 유지 또는 정리)
	PlayerStateClass = AMyCPPProjectPlayerState::StaticClass();
	PlayerControllerClass = AMyCPPProjectPlayerController::StaticClass();
	DefaultPawnClass = AMyCPPProjectCharacter::StaticClass();
	GameStateClass = AMyCPPProjectGameState::StaticClass();
}

void AMyCPPProjectGameMode::InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage)
{
	Super::InitGame(MapName, Options, ErrorMessage);

	// 다음 프레임에 Experience 로드 (GameState가 완전히 초기화된 후 실행하기 위함)
	GetWorld()->GetTimerManager().SetTimerForNextTick([this]()
	{
		if (AGameStateBase* GS = GameState)
		{
			if (UMyExperienceManagerComponent* ExperienceMgr = GS->FindComponentByClass<UMyExperienceManagerComponent>())
			{
				ExperienceMgr->ServerSetCurrentExperience(DefaultExperience);
			}
		}
	});
}