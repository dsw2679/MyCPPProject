// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "UObject/Interface.h"
#include "MyItemWidgetInterface.generated.h"

class UMyItemDefinition;

UINTERFACE(MinimalAPI, BlueprintType)
class UMyItemWidgetInterface : public UInterface
{
	GENERATED_BODY()
};

/**
 * 아이템 데이터를 시각화하는 위젯들이 상속받을 인터페이스
 */
class MYCPPPROJECT_API IMyItemWidgetInterface
{
	GENERATED_BODY()

public:
	// 위젯에 아이템 정보를 전달하는 함수
	UFUNCTION(BlueprintNativeEvent, BlueprintCallable, Category = "Item Widget")
	void UpdateWidget(const UMyItemDefinition* ItemDef, int32 Count);
};
