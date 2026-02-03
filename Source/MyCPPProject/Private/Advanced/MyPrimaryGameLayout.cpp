// Fill out your copyright notice in the Description page of Project Settings.


#include "Advanced/MyPrimaryGameLayout.h"
#include "MyGameplayTags.h"

UCommonActivatableWidget* UMyPrimaryGameLayout::GetGameLayerWidget() const
{
	if (Stack_Game)
	{
		return Stack_Game->GetActiveWidget();
	}
	return nullptr;
}

void UMyPrimaryGameLayout::NativeConstruct()
{
	Super::NativeConstruct();
	
	// 레이어 태그 등록
	if (Stack_Game)
	{
		RegisterLayer(MyGameplayTags::UI_Layer_Game, Stack_Game);
	}
	if (Stack_Menu)
	{
		RegisterLayer(MyGameplayTags::UI_Layer_Menu, Stack_Menu);
	}
	if (Stack_Modal)
	{
		RegisterLayer(MyGameplayTags::UI_Layer_Modal, Stack_Modal);
	}
}
