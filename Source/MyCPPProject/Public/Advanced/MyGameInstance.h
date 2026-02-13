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
	
	// 로딩창 시작
	void StartLoadingScreen();
	// 로딩창 종료
	void StopLoadingScreen();

	UPROPERTY(EditDefaultsOnly, Category = "Loading")
	TSoftClassPtr<UUserWidget> LoadingWidgetClass;
	
private:
	UPROPERTY()
	TObjectPtr<UUserWidget> LoadingWidgetInstance;

	// 엔진 뷰포트에 추가된 위젯을 관리하기 위한 변수
	TSharedPtr<SWidget> LoadingSlateWidget;
};
