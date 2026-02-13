// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyGameInstance.h"

#include "Blueprint/UserWidget.h"
#include "Widgets/SWeakWidget.h"
#include "Engine/GameViewportClient.h"

void UMyGameInstance::StartLoadingScreen()
{
	if (LoadingWidgetInstance || LoadingSlateWidget.IsValid()) return;

	UClass* LoadedClass = LoadingWidgetClass.LoadSynchronous();
	if (LoadedClass)
	{
		// 소유주(Outer)를 GameInstance(this)로 설정하여 월드 파괴 시에도 살아남게 함
		LoadingWidgetInstance = CreateWidget<UUserWidget>(this, LoadedClass);
		if (LoadingWidgetInstance)
		{
			LoadingSlateWidget = LoadingWidgetInstance->TakeWidget();

			// 엔진 최상위 뷰포트에 직접 추가 (Z-Order 1000으로 최상단 배치)
			if (GetGameViewportClient())
			{
				GetGameViewportClient()->AddViewportWidgetContent(LoadingSlateWidget.ToSharedRef(), 1000);
				bIsLoadingInProgress = true;
			}
		}
	}
}

void UMyGameInstance::StopLoadingScreen()
{
	if (GetGameViewportClient() && LoadingSlateWidget.IsValid())
	{
		GetGameViewportClient()->RemoveViewportWidgetContent(LoadingSlateWidget.ToSharedRef());
	}

	LoadingSlateWidget.Reset();
	LoadingWidgetInstance = nullptr;
	bIsLoadingInProgress = false;
}
