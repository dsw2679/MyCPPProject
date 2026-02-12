// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/GameInstance.h"
#include "MyGameInstance.generated.h"

/**
 * 
 */
UENUM(BlueprintType)
enum class ELoadingScreenType : uint8 {
	InitialStart, // 첫 게임 시작
	ToBattle      // 상점에서 배틀 레벨로 이동
};

UCLASS()
class MYCPPPROJECT_API UMyGameInstance : public UGameInstance
{
	GENERATED_BODY()
	
public:
	UPROPERTY()
	bool bIsLoadingInProgress = true;
	
	UPROPERTY(BlueprintReadWrite, Category = "Loading")
	ELoadingScreenType CurrentLoadingType = ELoadingScreenType::InitialStart;
};
