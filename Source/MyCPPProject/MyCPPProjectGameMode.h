// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/GameModeBase.h"
#include "MyCPPProjectGameMode.generated.h"

class UMyExperienceDefinition;

/**
 *  Simple Game Mode for a top-down perspective game
 *  Sets the default gameplay framework classes
 *  Check the Blueprint derived class for the set values
 */
UCLASS(minimalapi)
class AMyCPPProjectGameMode : public AGameModeBase
{
	GENERATED_BODY()

public:
	/** Constructor */
	AMyCPPProjectGameMode();
	
	// 게임이 시작될 때 호출
	virtual void InitGame(const FString& MapName, const FString& Options, FString& ErrorMessage) override;

protected:
	// 에디터에서 선택할 기본 Experience 데이터
	UPROPERTY(EditDefaultsOnly, Category = "Lyra|Experience")
	TObjectPtr<UMyExperienceDefinition> DefaultExperience;
};



