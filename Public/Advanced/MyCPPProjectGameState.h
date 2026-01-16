// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularGameState.h"
#include "MyCPPProjectGameState.generated.h"

class UMyExperienceManagerComponent;
/**
* 프로젝트의 메인 GameState 클래스입니다.
ExperienceManagerComponent를 소유하여 게임 모드 설정을 관리합니다.
 */
UCLASS()
class MYCPPPROJECT_API AMyCPPProjectGameState : public AModularGameStateBase
{
	GENERATED_BODY()
	
public:
	AMyCPPProjectGameState();

	// 외부에서 컴포넌트에 접근할 수 있는 Getter
	UMyExperienceManagerComponent* GetExperienceManagerComponent() const { return ExperienceManagerComponent; }

protected:
	// 우리가 만든 경험 관리자 컴포넌트
	UPROPERTY(VisibleAnywhere, Category = "Experience")
	TObjectPtr<UMyExperienceManagerComponent> ExperienceManagerComponent;
};
