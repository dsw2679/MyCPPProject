// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ModularAIController.h"
#include "MyBossAIController.generated.h"

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API AMyBossAIController : public AModularAIController
{
	GENERATED_BODY()
public:
	AMyBossAIController();

protected:
	virtual void OnPossess(APawn* InPawn) override;
	
};
