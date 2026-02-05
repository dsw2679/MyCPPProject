// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/DataAsset.h"
#include "MyShopCollection.generated.h"

class UMyItemDefinition;

/**
 * 상점에서 취급할 아이템 목록
 */
UCLASS(BlueprintType)
class MYCPPPROJECT_API UMyShopCollection : public UPrimaryDataAsset
{
	GENERATED_BODY()
	
public:
	// 이 상점에서 취급할 아이템 목록
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = "Shop")
	TArray<TObjectPtr<UMyItemDefinition>> ItemsForSale;
};
