// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyExperienceDefinition.generated.h"

class UMyPawnData;
UCLASS(BlueprintType, Const)
class MYCPPPROJECT_API UMyExperienceDefinition : public UDataAsset
{
	GENERATED_BODY()
	
public:
	UMyExperienceDefinition();

	// 이 Experience에서 사용할 기본 Pawn 데이터
	UPROPERTY(EditDefaultsOnly, BlueprintReadOnly, Category = "Lyra|Experience")
	TObjectPtr<const UMyPawnData> DefaultPawnData;
};
