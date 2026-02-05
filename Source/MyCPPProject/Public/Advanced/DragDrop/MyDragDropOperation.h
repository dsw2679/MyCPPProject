// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/DragDropOperation.h"
#include "MyDragDropOperation.generated.h"

class UMyItemDefinition;

/**
 * 
 */
UCLASS()
class MYCPPPROJECT_API UMyDragDropOperation : public UDragDropOperation
{
	GENERATED_BODY()
	
public:
	// 드래그 중인 아이템 데이터
	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "DragDrop", meta = (ExposeOnSpawn = "true"))
	TObjectPtr<const UMyItemDefinition> PayloadItem;
};
